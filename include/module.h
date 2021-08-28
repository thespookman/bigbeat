#ifndef MODULE_H
#define MODULE_H
#include "environment.h"
#include "pattern.h"

#include <string>
#include <vector>

class Environment;
class Pattern;

class Module {
  private:
    int                      speed;
    Pattern*                 pattern;
    std::vector<std::string> submodules;

  public:
    Module ();
    Module (int granularity, Pattern* _pattern);
    Module (std::string submodule, int repeat);
    explicit Module (int tempo);
    void        add (Module* module);
    std::string get (int index);
    int         length ();
    void        evaluate (Environment* env);
    ~Module ();
};
#endif
