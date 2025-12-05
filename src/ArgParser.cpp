/**
 * @file ArgParser.cpp
 * @authors Michal Repcik (xrepcim00), Adam Vesely (xvesela00)
 */
#include "ArgParser.hpp"
#include <iostream>
#include <string>
#include <cstdlib>

ArgParser::ArgParser(size_t argc, char* argv[])
    : argc(argc), argv(argv) {}

bool parseInt(const char* s, int& out, const char* flagName) {
    try {
        out = std::stoi(s);
        return true;
    } catch (...) {
        std::cerr << "Error: Invalid integer for " << flagName << std::endl;
        return false;
    }
}

bool parseDouble(const char* s, double& out, const char* flagName) {
    try {
        out = std::atof(s);
        return true;
    } catch (...) {
        std::cerr << "Error: Invalid double for " << flagName << std::endl;
        return false;
    }
}

bool returnWithError(const char* msg) {
    std::cerr << "Error: " << msg << std::endl;
    return false;
}

bool ArgParser::parse() {
    for (size_t i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            displayHelp();
            return false;
        }
        else if (arg == "-v" || arg == "--viz") {
            vizFlag = true;
            if (i + 1 < argc && argv[i + 1][0] != '-')
                vizDir = argv[++i];
        }
        else if (arg == "-p" || arg == "--plot") {
            plotFlag = true;
            if (i + 1 < argc && argv[i + 1][0] != '-')
                plotDir = argv[++i];
        }
        else if (arg == "-s" || arg == "--steps") {
            if (i + 1 >= argc || argv[i + 1][0] == '-') 
                return returnWithError("Missing number for --steps.");
            if (!parseInt(argv[++i], steps, "--steps")) 
                return false;
        }
        else if (arg == "-W" || arg == "--width") {
            if (i + 1 >= argc || argv[i + 1][0] == '-') 
                return returnWithError("Missing number for --width.");
            if (!parseInt(argv[++i], width, "--width")) 
                return false;
        }
        else if (arg == "-H" || arg == "--height") {
            if (i + 1 >= argc || argv[i + 1][0] == '-') 
                return returnWithError("Missing number for --height.");
            if (!parseInt(argv[++i], height, "--height")) 
                return false;
        }
        else if (arg == "-M" || arg == "--maxspeed") {
            if (i + 1 >= argc || argv[i + 1][0] == '-') 
                return returnWithError("Missing number for --maxspeed.");
            if (!parseInt(argv[++i], vmax, "--maxspeed")) 
                return false;
            if (vmax < 0) return returnWithError("--maxspeed must be non-negative.");
        }
        else if (arg == "-P" || arg == "--prob") {
            if (i + 1 >= argc || argv[i + 1][0] == '-') 
                return returnWithError("Missing number for --prob.");
            if (!parseDouble(argv[++i], prob, "--prob")) 
                return false;
            if (prob < 0.0 || prob > 1.0) return returnWithError("--prob must be 0-1.");
        }
        else if (arg == "-D" || arg == "--density") {
            if (i + 1 >= argc || argv[i + 1][0] == '-') 
                return returnWithError("Missing number for --density.");
            if (!parseDouble(argv[++i], density, "--density")) 
                return false;
            if (density < 0.0 || density > 1.0) return returnWithError("--density must be 0-1.");
        }
        else if (arg == "-o" || "--optimize") {
            optimize = true;
            continue;
        }
        else {
            std::cerr << "Unknown option: " << arg << "\n\n";
            displayHelp();
            return false;
        }
    }
    return true;
}

void ArgParser::displayHelp() {
    std::cout
        << "Usage: " << argv[0] << " [options]\n\n"
        << "Options:\n"
        << "  -v, --viz [dir]           Enable PPM visualization.\n"
        << "                            dir = output directory (optional)\n"
        << "  -p, --plot [dir]          Enable plot data extraction\n"
        << "                            dir = output directory"
        << "  -s, --steps <n>           Number of CA steps/updates.\n"
        << "  -W, --width <n>           Road length (CA grid width).\n"
        << "  -H, --height <n>          Number of lanes (CA grid height, default 1).\n"
        << "  -M, --maxspeed <n>        Max car velocity (>=0, default 5).\n"
        << "  -P, --prob <f>            Braking probability (random braking) (0-1, default 0.3).\n"
        << "  -D, --density <f>         Initial car density (0-1, default 0.2).\n"
        << "  -O, --optimize            Adds an additional straight lane to east inbound and west outbound.\n"
        << "  -h, --help                Show this help message.\n";
}