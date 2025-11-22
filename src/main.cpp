#include "Grid.hpp"
#include "Rules.hpp"
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <iostream>

int main() {
    std::filesystem::create_directories("viz");

    Grid grid(10, 10);
    grid.getCell(1, 0).setAlive(true);
    grid.getCell(2, 1).setAlive(true);
    grid.getCell(0, 2).setAlive(true);
    grid.getCell(1, 2).setAlive(true);
    grid.getCell(2, 2).setAlive(true);

    GameOfLifeRules rules;

    for (int step = 0; step < 100; ++step) {
        grid.update(rules);
        std::ostringstream ss;
        ss << "viz/frame_" << std::setw(3) << std::setfill('0') << step << ".ppm";

        grid.exportPPM(ss.str(), 50);

        std::cout << "Saved frame: " << ss.str() << std::endl;
    } 

    return 0;
}