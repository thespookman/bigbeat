#ifndef PATTERN_H
#define PATTERN_H
#include "beat.h"
#include "environment.h"

#include <string>
#include <vector>

class Beat;
class Environment;

class Pattern {
  private:
    std::vector<Beat*>       instrument_lines;
    std::vector<std::string> instruments;

  public:
    Pattern ();
    Pattern (std::string instrument, Beat* b);
    void        add (Pattern* p);
    int         get_lines ();
    Beat*       get_line (int index);
    std::string get_instrument (int index);
    void        evaluate (int granularity, Environment* env);
    ~Pattern ();
};
#endif
