#include "Grid.hpp"
#include "Rules.hpp"
#include "ArgParser.hpp"
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <ctime>

int main(int argc, char* argv[]) {
    srand(static_cast<unsigned>(time(nullptr)));
    ArgParser parser(static_cast<size_t>(argc), argv);
    if (!parser.parse())
        return 1;
    if (parser.isVizEnabled())
        std::filesystem::create_directories(parser.getVizDir());
    
    Grid grid(parser.getWidth(), parser.getHeight());
    
    int middleX = parser.getWidth() / 2;
    std::cout << "Adding traffic lights at x=" << middleX << std::endl;
    for (int y = 0; y < parser.getHeight(); y++)
        grid.addTrafficLight(middleX, y, 30, 5, 30);
    
    NSRules rules;
    
    double spawnProbability = parser.getDensity();
    
    std::cout << "Step, AvgVelocity" << std::endl;
    
    for (int step = 0; step < parser.getSteps(); step++) {
        if ((double)rand() / RAND_MAX < spawnProbability) {
            int initialVel = rand() % (parser.getVMax() + 1);
            grid.spawnCars(initialVel);
        }
        
        grid.updateTrafficLights();
        grid.update(rules, parser.getVMax(), parser.getProb());
        
        double avgVel = grid.averageVelocity();
        std::cout << step << ", " << avgVel << std::endl;
        
        if (parser.isVizEnabled()) {
            std::ostringstream ss;
            ss << parser.getVizDir() << "/frame_" 
               << std::setw(5) << std::setfill('0') << step << ".ppm";
            grid.exportPPM(ss.str(), 10, parser.getVMax());
        }
    }
    
    return 0;
}