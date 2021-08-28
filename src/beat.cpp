#include "beat.h"

#include <stdexcept>

Beat::Beat () {}

Beat::Beat (int number) { volumes.push_back (number); }

Beat::Beat (Beat* b) {
    for (unsigned int i = 0; i < b->length (); ++i) volumes.push_back (b->get (i));
}

unsigned int Beat::length () { return volumes.size (); }

int Beat::get (int index) {
    try {
        return volumes.at (index);
    } catch (std::out_of_range& e) { return 0; }
}

void Beat::add (Beat* b) {
    for (unsigned int i = 0; i < b->length (); ++i) volumes.push_back (b->get (i));
}

Beat::~Beat () {}
