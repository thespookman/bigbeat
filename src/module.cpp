#include "module.h"

#include "environment.h"

#include <iostream>

Module::Module () {}

Module::Module (int granularity, Pattern* _pattern) : speed {granularity}, pattern {_pattern} {}

Module::Module (std::string submodule, int repeat) {
    for (int i = 0; i < repeat; ++i) submodules.push_back (submodule);
}

Module::Module (int tempo) : speed {tempo} {}

void Module::add (Module* module) {
    for (int i = 0; i < module->length (); ++i) submodules.push_back (module->get (i));
}

std::string Module::get (int index) { return submodules[index]; }

int Module::length () { return submodules.size (); }

void Module::evaluate (Environment* env) {
    if (pattern == NULL)
        if (submodules.empty ())
            env->set_tempo (speed);
        else
            for (std::string s : submodules) {
                try {
                    env->get_module (s)->evaluate (env);
                } catch (const std::exception& e) { std::cerr << e.what () << std::endl; }
            }
    else
        pattern->evaluate (speed, env);
}

Module::~Module () {
    if (pattern) delete pattern;
}
