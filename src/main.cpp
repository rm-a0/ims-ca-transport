/**
 * @file main.cpp
 * @authors Michal Repcik (xrepcim00), Adam Vesely (xvesela00)
 */
#include "Grid.hpp"
#include "Rules.hpp"
#include "ArgParser.hpp"
#include "Utils.hpp"
#include "Logger.hpp"
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
    if (parser.isPlotEnabled())
        std::filesystem::create_directories(parser.getPlotDir());
    
    Grid grid(parser.getWidth(), parser.getHeight());
    grid.initializeMap(parser.getDensity(), parser.getOptimize());
    grid.setupCrossroadLights(25, 0, 20);
    
    NSRules rules;
    Logger logger;
    grid.setLogger(&logger);
    
    for (int step = 0; step < parser.getSteps(); step++) {
        grid.update(rules, parser.getDensity(), parser.getVMax(), parser.getProb(), step);
        
        if (parser.isVizEnabled()) {
            std::ostringstream ss;
            ss << parser.getVizDir() << "/frame_" << std::setw(5) << std::setfill('0') << step << ".ppm";
            Utils::exportPPM(grid, ss.str(), 10, parser.getVMax()); 
        }

        if (step % 25 == 0 || step == parser.getSteps() - 1) {
            logger.finalizeData();
            logger.printSummaryTable();
        }

    }

    if (parser.isPlotEnabled()) {
        std::cout << "\nFinalizing and exporting data..." << std::endl;
        std::string plotSubDir = parser.getOptimize() ? "modified" : "baseline";
        std::string exportDir = parser.getPlotDir() + "/" + plotSubDir;
        std::filesystem::create_directories(exportDir);
        grid.logDirectionMetrics(parser.getSteps() - 1);
        logger.finalizeData();
        logger.exportAll(exportDir);
        std::cout << "Data export complete!" << std::endl;
        std::cout << "\nGenerated files in '" << exportDir << "':" << std::endl;
        std::cout << "  - timestep_metrics.csv" << std::endl;
        std::cout << "  - vehicle_trajectories.csv" << std::endl;
        std::cout << "  - spatial_heatmap.csv" << std::endl;
        std::cout << "  - direction_metrics.csv" << std::endl;
        std::cout << "  - summary_statistics.csv" << std::endl;
    } 
        
    return 0;
}