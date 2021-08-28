#include "pattern.h"

#include <iostream>

Pattern::Pattern () {}

Pattern::Pattern (std::string instrument, Beat* b) {
    instrument_lines.push_back (b);
    instruments.push_back (instrument);
}

void Pattern::add (Pattern* p) {
    for (int i = 0; i < p->get_lines (); ++i) {
        instrument_lines.push_back (new Beat (p->get_line (i)));
        instruments.push_back (p->get_instrument (i));
    }
}

int Pattern::get_lines () { return instrument_lines.size (); }

Beat* Pattern::get_line (int index) { return instrument_lines[index]; }

std::string Pattern::get_instrument (int index) { return instruments[index]; }

void Pattern::evaluate (int granularity, Environment* env) {
    unsigned int length = 0;
    for (unsigned int i = 0; i < instrument_lines.size (); ++i) {
        if (instrument_lines[i]->length () > length) length = instrument_lines[i]->length ();
        for (unsigned int j = 0; j < instrument_lines[i]->length (); ++j)
            if (instrument_lines[i]->get (j) > 0)
                env->add_beat (instruments[i], granularity, j, instrument_lines[i]->get (j));
    }
    if (!instrument_lines.empty ()) env->update_track_position (granularity, length);
}

Pattern::~Pattern () {
    for (Beat* b : instrument_lines) delete b;
}

