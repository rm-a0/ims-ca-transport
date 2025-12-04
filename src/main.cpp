#include "Grid.hpp"
#include "Rules.hpp"
#include "ArgParser.hpp"
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <ctime>
#include <cmath>
#include <map>

int main(int argc, char* argv[]) {
    srand(static_cast<unsigned>(time(nullptr)));
    ArgParser parser(static_cast<size_t>(argc), argv);
    if (!parser.parse())
        return 1;
    if (parser.isVizEnabled())
        std::filesystem::create_directories(parser.getVizDir());
    
    Grid grid(parser.getWidth(), parser.getHeight());
    grid.setupCrossroadLights(25, 0, 20);
    grid.initializeCarsWithDensity(parser.getDensity(), parser.getVMax());
    
    NSRules rules;
    
    std::cout << "Step, AvgVelocity" << std::endl;
    
    for (int step = 0; step < parser.getSteps(); step++) {
        grid.update(rules, parser.getVMax(), parser.getProb());
        
        double avgVel = grid.averageVelocity();
        std::cout << step << ", " << avgVel << std::endl;

        if (parser.isVizEnabled()) {
            std::ostringstream ss;
            ss << parser.getVizDir() << "/frame_" << std::setw(5) << std::setfill('0') << step << ".ppm";
            grid.exportPPM(ss.str(), 8, parser.getVMax());
        }
    }
    
    return 0;
}