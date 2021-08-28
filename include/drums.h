#ifndef DRUMS_H
#define DRUMS_H
#include "environment.h"

#include <stdexcept>

class Beat;
class Module;
class Pattern;

void parse (const char* file_name, Environment* env);

class File_Not_Found : public std::runtime_error {
  public:
    File_Not_Found () : std::runtime_error ("Input file could not be opened.") {}
};
#endif
