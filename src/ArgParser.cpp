#include "ArgParser.hpp"
#include <iostream>
#include <string>

ArgParser::ArgParser(size_t argc, char* argv[])
    : argc(argc), argv(argv) {}

bool parseInt(const char* s, int& out, const char* flagName) {
    try {
        out = std::stoi(s);
        return true;
    } catch (...) {
        std::cerr << "Error: Invalid value for " << flagName << std::endl;
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
        << "  -s, --steps <n>           Number of CA steps/updates.\n"
        << "  -W, --width <n>           Width of the CA grid.\n"
        << "  -H, --height <n>          Height of the CA grid.\n"
        << "  -h, --help                Show this help message.\n";
}
