#include "Grid.hpp"
#include "Rules.hpp"
#include "ArgParser.hpp"
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <ctime>

int main(int argc, char* argv[]) {
    srand(static_cast<unsigned>(time(nullptr)));  // Seed rand

    ArgParser parser(static_cast<size_t>(argc), argv);

    if (!parser.parse())
        return 1;

    if (parser.isVizEnabled())
        std::filesystem::create_directories(parser.getVizDir());

    Grid grid(parser.getWidth(), parser.getHeight());
    grid.initializeRandom(parser.getDensity(), parser.getVMax());

    NSRules rules;

    // For experiments: Track avg velocity over steps
    std::cout << "Step, AvgVelocity" << std::endl;

    for (int step = 0; step < parser.getSteps(); step++) {
        grid.update(rules, parser.getVMax(), parser.getProb());

        double avgVel = grid.averageVelocity();
        std::cout << step << ", " << avgVel << std::endl;

        if (parser.isVizEnabled()) {
            std::ostringstream ss;
            ss << parser.getVizDir() << "/frame_" 
               << std::setw(4) << std::setfill('0') << step << ".ppm";

            grid.exportPPM(ss.str(), 10, parser.getVMax());
            std::cout << "Saved frame: " << ss.str() << std::endl;
        }
    }

    return 0;
}