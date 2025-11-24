#include "Grid.hpp"
#include "Rules.hpp"
#include "ArgParser.hpp"
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <iostream>

int main(int argc, char* argv[]) {
    ArgParser parser(argc, argv);

    if (!parser.parse())
        return 1;

    if (parser.isVizEnabled())
        std::filesystem::create_directories(parser.getVizDir());

    Grid grid(parser.getWidth(), parser.getHeight());

    grid.getCell(1, 0).setAlive(true);
    grid.getCell(2, 1).setAlive(true);
    grid.getCell(0, 2).setAlive(true);
    grid.getCell(1, 2).setAlive(true);
    grid.getCell(2, 2).setAlive(true);

    GameOfLifeRules rules;

    for (int step = 0; step < parser.getSteps(); ++step) {
        grid.update(rules);

        if (parser.isVizEnabled()) {
            std::ostringstream ss;
            ss << parser.getVizDir() << "/frame_" 
               << std::setw(3) << std::setfill('0') << step << ".ppm";

            grid.exportPPM(ss.str(), 50);
            std::cout << "Saved frame: " << ss.str() << std::endl;
        }
    }

    return 0;
}
