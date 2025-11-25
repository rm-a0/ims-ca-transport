/**
 * @file Grid.hpp
 * @authors Michal Repcik (xrepcim00), Adam Vesely (xvesela00)
 */
#ifndef GRID_HPP
#define GRID_HPP

#include "Cell.hpp"
#include "Rules.hpp"
#include <vector>
#include <fstream>
#include <string>

/**
 * @class Grid
 * @brief Represents a CA grid for traffic (1D road if height=1)
 */
class Grid {
public:
    /**
     * @brief Constructor for the Grid class
     * @param w Grid width (road length)
     * @param h Grid height (lanes, use 1 for single-lane)
     */
    Grid(int w, int h);

    /**
     * @brief Initializes grid with random cars at given density
     * @param density Fraction of cells with cars (0-1)
     * @param vmax Max velocity for initial cars
     */
    void initializeRandom(double density, int vmax);

    /**
     * @brief Updates the grid using specified rules (NS for traffic)
     * @param rules Rules to be applied
     * @param vmax Max velocity
     * @param p Braking probability
     */
    void update(const Rules& rules, int vmax, double p);

    /**
     * @brief Finds distance to next car ahead (toroidal)
     * @param x X coordinate
     * @param y Y coordinate (lane)
     * @return Distance (cells) to next car
     */
    int distanceToNextCar(int x, int y) const;

    /**
     * @brief Export grid into a PPM file (colors by velocity)
     * @param filename Name of the file
     * @param scale Pixel scale factor
     */
    void exportPPM(const std::string& filename, int scale, int vmax) const;

    /**
     * @brief Computes average velocity of all cars
     * @return Avg velocity (or 0 if no cars)
     */
    double averageVelocity() const;

    /**
     * @brief Gets cell at (x, y)
     * @param x X coordinate
     * @param y Y coordinate
     * @return Cell at set coordinate
     */
    Cell& getCell(int x, int y) { return cells[y][x]; }
    
private:
    int width;                              ///< Width of the grid (road length)
    int height;                             ///< Height of the grid (lanes)
    std::vector<std::vector<Cell>> cells;   ///< 2D vector containing cells
};

#endif // GRID_HPP