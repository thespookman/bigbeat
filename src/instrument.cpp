#include "instrument.h"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>

Instrument::Instrument (uint8_t _volume, std::string _file_name)
    : volume {_volume}, file_name {_file_name} {}

void Instrument::add_beat (int position, unsigned char velocity) {
    size_t new_end = position + samples.size ();
    if (new_end >= output_track.size ()) output_track.resize (new_end, {0, 0});
    float scale = (float) velocity / 9;
    for (unsigned int i = 0; i < samples.size (); ++i) {
        output_track[position + i].first  = samples[i].first * scale;
        output_track[position + i].second = samples[i].second * scale;
    }
}

std::vector<std::pair<int32_t, int32_t>>* Instrument::get_track () { return &output_track; }

union convert {
    int32_t i;
    struct {
        unsigned char c[4];
    } s;
};

void Instrument::read_samples (short total_volume, unsigned int output_frequency, bool b24) {
    FILE* infile = fopen (file_name.c_str (), "rb");
    if (!infile) throw File_Not_Open (file_name);
    char str[5];
    fread (str, 1, 4, infile);
    str[4] = '\0';
    if (strcmp (str, "RIFF")) {
        fclose (infile);
        throw Bad_File_Format ("Missing RIFF chunk", file_name);
    }
    fseek (infile, 4, SEEK_CUR);
    fread (str, 1, 4, infile);
    str[4] = '\0';
    if (strcmp (str, "WAVE")) {
        fclose (infile);
        throw Bad_File_Format ("Missing WAVE chunk", file_name);
    }
    fseek (infile, 10, SEEK_CUR);
    uint16_t channels, bit_depth;
    fread (&channels, 2, 1, infile);
    if (channels == 0 || channels > 2) {
        fclose (infile);
        throw Bad_File_Format ("Must have one or two channels", file_name);
    }
    uint32_t sample_rate, data_chunk_size;
    fread (&sample_rate, 4, 1, infile);
    fseek (infile, 6, SEEK_CUR);
    fread (&bit_depth, 2, 1, infile);
    int max = pow (2, bit_depth);
    if (bit_depth > 8) max /= 2;
    --max;
    int outmax = std::numeric_limits<int16_t>::max ();
    if (b24) outmax *= 255;
    double scale = ((double) outmax * volume) / (total_volume * max);
    fread (str, 1, 4, infile);
    str[4] = '\0';
    while (strcmp (str, "data")) {
        uint32_t chunk_size;
        fread (&chunk_size, 4, 1, infile);
        fseek (infile, chunk_size, SEEK_CUR);
        fread (str, 1, 4, infile);
        if (feof (infile)) {
            fclose (infile);
            throw Bad_File_Format ("Missing data chunk", file_name);
        }
        str[4] = '\0';
    }
    fread (&data_chunk_size, 4, 1, infile);
    int                                      frames = data_chunk_size / (bit_depth / 8) / channels;
    unsigned char                            difference = (32 - bit_depth) / 8;
    union convert                            temp_l, temp_r;
    std::vector<std::pair<int32_t, int32_t>> data;
    for (int x = 0; x < frames; ++x) {
        temp_l.i = temp_r.i = 0;
        fread (&temp_l.s.c[0], bit_depth / 8, 1, infile);
        if (temp_l.s.c[3 - difference] & 128)
            for (int i = 4 - difference; i < 4; ++i) temp_l.s.c[i] = 255;
        if (channels > 1) {
            if (feof (infile)) {
                fclose (infile);
                throw Bad_File_Format ("Unexpected EOF: ", file_name);
            }
            fread (&temp_r.s.c[0], bit_depth / 8, 1, infile);
            if (temp_r.s.c[3 - difference] & 128)
                for (int i = 4 - difference; i < 4; ++i) temp_r.s.c[i] = 255;
        } else
            temp_r = temp_l;
        data.push_back ({temp_l.i * scale, temp_r.i * scale});
        if (feof (infile)) {
            fclose (infile);
            throw Bad_File_Format ("Unexpected EOF: ", file_name);
        }
    }
    fclose (infile);
    if (sample_rate == output_frequency)
        for (std::pair<int32_t, int32_t> p : data) samples.push_back ({p.first, p.second});
    else {
        double        in_sample_time  = ((double) 1) / sample_rate;
        double        out_sample_time = ((double) 1) / output_frequency;
        unsigned long sample_count    = data.size () * in_sample_time * output_frequency;
        for (unsigned int i = 0; i < sample_count; ++i) {
            double       time            = i * out_sample_time;
            unsigned int in_sample_index = time / in_sample_time;
            double       sample_offset   = std::fmod (time, in_sample_time) / in_sample_time;
            std::pair<int32_t, int32_t> next_sample = {0, 0};
            if (in_sample_index < data.size () - 1) next_sample = data[in_sample_index + 1];
            double out_amp_l = ((double) data[in_sample_index].first * (1 - sample_offset)) +
                               ((double) next_sample.first * sample_offset);
            double out_amp_r = ((double) data[in_sample_index].second * (1 - sample_offset)) +
                               ((double) next_sample.second * sample_offset);
            samples.push_back ({out_amp_l, out_amp_r});
        }
    }
}

std::vector<std::pair<int32_t, int32_t>> Instrument::get_sample () { return samples; }

uint8_t Instrument::get_volume () { return volume; }

File_Not_Open::File_Not_Open (std::string file)
    : std::runtime_error (std::string ("Could not open ") + file) {};

Bad_File_Format::Bad_File_Format (std::string file, std::string error)
    : std::runtime_error (file + " is malformed: " + error) {};
