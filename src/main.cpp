#include "drums.h"
#include "environment.h"
#include "parser.tab.hh"

#include <cstring>
#include <iostream>
#include <string>

void usage (const std::string& program) {
    std::cout << program << " - Command line drum machine" << std::endl
              << std::endl
              << "Usage: " << std::endl
              << "\t" << program << " [options] -i <beat_file>" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "\t--help\t\t\t- Display this help and exit" << std::endl
              << "\t-i <beat_file>\t\t- File to read for beat data" << std::endl
              << "\t-o <out file>\t\t- File to output WAV to" << std::endl
              << "\t-s <sample rate>\t- Sets sample rate of output" << std::endl
              << "\t-t <bpm>\t\t- sets default tempo of output" << std::endl
              << "\t--24bit\t\t\t- Output 24-bit WAV (default is 16)" << std::endl;
}

int main (int argc, char** argv) {
    std::string program = argv[0];
    argv                = &argv[1];
    --argc;
    std::string infile      = "";
    bool        inset       = false;
    int         sample_rate = 44100;
    int         tempo       = 80;
    std::string outfile     = "output.wav";
    bool        b24         = false;
    while (argc) {
        if (!strcmp (argv[0], "--help")) {
            usage (program);
            return 0;
        } else if (!strcmp (argv[0], "-i")) {
            if (argc < 2) {
                std::cerr << "No input file specified." << std::endl;
                usage (program);
                return 1;
            }
            infile = argv[1];
            inset  = true;
            argv   = &argv[2];
            argc -= 2;
        } else if (!strcmp (argv[0], "-o")) {
            if (argc < 2) {
                std::cerr << "No output file specified." << std::endl;
                usage (program);
                return 1;
            }
            outfile = argv[1];
            argv    = &argv[2];
            argc -= 2;
        } else if (!strcmp (argv[0], "-s")) {
            if (argc < 2) {
                std::cerr << "No sample rate specified." << std::endl;
                usage (program);
                return 1;
            }
            try {
                sample_rate = std::stoi (argv[1]);
            } catch (const std::exception& e) {
                std::cerr << "Sample rate must be integer." << std::endl;
                usage (program);
                return 1;
            }
            argv = &argv[2];
            argc -= 2;
        } else if (!strcmp (argv[0], "-t")) {
            if (argc < 2) {
                std::cerr << "No tempo specified." << std::endl;
                usage (program);
                return 1;
            }
            try {
                tempo = std::stoi (argv[1]);
            } catch (const std::exception& e) {
                std::cerr << "Tempo must be integer." << std::endl;
                usage (program);
                return 1;
            }
            argv = &argv[2];
            argc -= 2;
        } else if (!strcmp (argv[0], "--24bit")) {
            b24  = true;
            argv = &argv[1];
            --argc;
        }
    }
    if (!inset) {
        std::cerr << "No input file supplied." << std::endl;
        usage (program);
        return 1;
    }
    Environment env (sample_rate, tempo, b24);
    parse (infile.c_str (), &env);
    env.process_samples ();
    try {
        env.get_module ("Song")->evaluate (&env);
        env.export_track (outfile);
    } catch (const std::exception& e) { std::cerr << e.what () << std::endl; }
}
