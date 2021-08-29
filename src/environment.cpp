#include "environment.h"

#include <fstream>
#include <iostream>
#include <string>
#include <utility>

Environment::Environment () {}

Environment::Environment (int output_frequency, int _tempo, bool _b24)
    : frequency {output_frequency}, track_position {0}, b24 {_b24} {
    set_tempo (_tempo);
}

void Environment::add_beat (std::string instrument, int granularity, int position,
                            unsigned char velocity) {
    try {
        get_instrument (instrument)
            ->add_beat (track_position + (position * (tempo / granularity)), velocity);
    } catch (const std::exception& e) { std::cerr << e.what () << std::endl; }
}

void Environment::add_module (std::string name, Module* module) {
    Module_Map::iterator it = modules.find (name);
    if (it == modules.end ())
        modules.insert ({name, module});
    else {
        delete it->second;
        it->second = module;
    }
}

void Environment::add_instrument (std::string name, Instrument* instrument) {
    Instrument_Map::iterator it = instruments.find (name);
    if (it == instruments.end ())
        instruments.insert ({name, instrument});
    else {
        delete it->second;
        it->second = instrument;
    }
}

void Environment::export_track (std::string file_name) {
    long    length = 0;
    uint8_t bytes  = b24 ? 3 : 2;
    for (std::pair<std::string, Instrument*> i : instruments) {
        long track_length = i.second->get_track ()->size ();
        if (track_length > length) length = track_length;
    }
    std::vector<std::pair<int32_t, int32_t>> output_track (length);
    for (unsigned int i = 0; i < output_track.size (); ++i)
        output_track[i].first = output_track[i].second = 0;
    for (std::pair<std::string, Instrument*> i : instruments) {
        std::vector<std::pair<int32_t, int32_t>>* instrument_track = i.second->get_track ();
        for (unsigned int j = 0; j < instrument_track->size (); ++j) {
            output_track[j].first += instrument_track->at (j).first;
            output_track[j].second += instrument_track->at (j).second;
        }
    }
    std::ofstream of (file_name, std::ios::binary);
    of << "RIFF----WAVEfmt ";                  // (chunk size to be filled in later)
    write_word (of, 16, 4);                    // no extension data
    write_word (of, 1, 2);                     // PCM - integer samples
    write_word (of, 2, 2);                     // two channels (stereo file)
    write_word (of, frequency, 4);             // samples per second (Hz)
    write_word (of, frequency * bytes * 2, 4); // (Sample Rate * BitsPerSample * Channels) / 8
    write_word (of, bytes * 2,
                2); // data block size (size of two integer samples, one for each channel)
    write_word (of, bytes * 8, 2); // number of bits per sample (use a multiple of 8)
    size_t data_chunk_pos = of.tellp ();
    of << "data----";
    for (std::pair<int32_t, int32_t> p : output_track) {
        write_word (of, p.first, bytes);
        write_word (of, p.second, bytes);
    }
    size_t file_length = of.tellp ();
    of.seekp (data_chunk_pos + 4);
    write_word (of, file_length - data_chunk_pos + 8);
    of.seekp (0 + 4);
    write_word (of, file_length - 8, 4);
    of.close ();
}

template <typename Word>
std::ostream& Environment::write_word (std::ostream& outs, Word value, unsigned size) {
    for (; size; --size, value >>= 8) outs.put (static_cast<char> (value & 0xFF));
    return outs;
}

Module* Environment::get_module (std::string name) {
    Module_Map::iterator it = modules.find (name);
    if (it == modules.end ())
        throw Module_Not_Found (name);
    else
        return it->second;
}

Instrument* Environment::get_instrument (std::string name) {
    Instrument_Map::iterator it = instruments.find (name);
    if (it == instruments.end ())
        throw Instrument_Not_Found (name);
    else
        return it->second;
}

void Environment::process_samples () {
    short total_volume = 0;
    for (std::pair<std::string, Instrument*> i : instruments)
        total_volume += i.second->get_volume ();
    for (std::pair<std::string, Instrument*> i : instruments) try {
            i.second->read_samples (total_volume, frequency, b24);
        } catch (std::exception& e) { std::cerr << e.what () << std::endl; }
}

void Environment::set_tempo (int _tempo) { tempo = (60 * frequency) / _tempo; }

void Environment::update_track_position (int granularity, int length) {
    track_position += (tempo / granularity) * length;
}

Environment::~Environment () {
    for (std::pair<std::string, Module*> m : modules) delete m.second;
    for (std::pair<std::string, Instrument*> i : instruments) delete i.second;
}

Instrument_Not_Found::Instrument_Not_Found (std::string instrument)
    : std::runtime_error (std::string ("Instrument not defined: ") + instrument) {}

Module_Not_Found::Module_Not_Found (std::string module)
    : std::runtime_error (std::string ("Module not defined: ") + module) {}
