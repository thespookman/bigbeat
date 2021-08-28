#ifndef INSTRUMENT_H
#define INSTRUMENT_H
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

class Instrument {
  private:
    uint8_t                                  volume;
    std::string                              file_name;
    std::vector<std::pair<int32_t, int32_t>> samples;
    std::vector<std::pair<int32_t, int32_t>> output_track;

  public:
    Instrument (uint8_t _volume, std::string _file_name);
    void                                      add_beat (int position, unsigned char velocity);
    std::vector<std::pair<int32_t, int32_t>>* get_track ();
    void read_samples (short total_volume, unsigned int output_frequency, bool b24);
    std::vector<std::pair<int32_t, int32_t>> get_sample ();
    uint8_t                                  get_volume ();
};

class File_Not_Open : public std::runtime_error {
  public:
    explicit File_Not_Open (std::string file);
};

class Bad_File_Format : public std::runtime_error {
  public:
    Bad_File_Format (std::string file, std::string error);
};
#endif
