#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include "instrument.h"
#include "module.h"

#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

class Module;
class Instrument;

typedef std::unordered_map<std::string, Module*>     Module_Map;
typedef std::unordered_map<std::string, Instrument*> Instrument_Map;

class Environment {
  private:
    Module_Map     modules;
    Instrument_Map instruments;
    int            frequency;
    int            tempo;
    long           track_position;
    bool           b24;

  public:
    Environment ();
    Environment (int output_frequency, int _tempo, bool _b24);
    void add_beat (std::string instrument, int granularity, int position, unsigned char velocity);
    void add_module (std::string name, Module* module);
    void add_instrument (std::string name, Instrument* instrument);
    void export_track (std::string file_name);
    template <typename Word>
    std::ostream& write_word (std::ostream& outs, Word value, unsigned size = sizeof (Word));
    Module*       get_module (std::string name);
    Instrument*   get_instrument (std::string name);
    void          process_samples ();
    void          set_tempo (int _tempo);
    void          update_track_position (int granularity, int length);
    ~Environment ();
};

class Instrument_Not_Found : public std::runtime_error {
  public:
    explicit Instrument_Not_Found (std::string module);
};

class Module_Not_Found : public std::runtime_error {
  public:
    explicit Module_Not_Found (std::string module);
};
#endif
