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
     * @brief Initializes grid for left-to-right spawning
     * @param vmax Max velocity
     * @param spawnProbability Probability of spawning at leftmost column
     */
    void initializeLeftToRight(int vmax, double spawnProbability);
    /**
     * @brief Adds a traffic light at specified position
     * @param x X coordinate
     * @param y Y coordinate
     * @param redDur Red light duration in steps
     * @param yellowDur Yellow light duration in steps
     * @param greenDur Green light duration in steps
     */
    void addTrafficLight(int x, int y, int redDur, int yellowDur, int greenDur);
    /**
     * @brief Updates all traffic lights in the grid
     */
    void updateTrafficLights();
    /**
     * @brief Spawns a car at the leftmost column if cell is empty
     * @param y Lane (row) to spawn car in
     * @param velocity Initial velocity of the car
     * @return true if car was spawned, false if cell was occupied
     */
    bool spawnCarAtLeft(int y, int velocity);
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
    const Cell& getCell(int x, int y) const { return cells[y][x]; }
    
    /**
     * @brief Gets the width of the grid
     * @return Width
     */
    int getWidth() const { return width; }
    
    /**
     * @brief Gets the height of the grid
     * @return Height
     */
    int getHeight() const { return height; }
    
private:
    int width;                              ///< Width of the grid (road length)
    int height;                             ///< Height of the grid (lanes)
    std::vector<std::vector<Cell>> cells;   ///< 2D vector containing cells
    int nextCarId = 0;                      ///< ID of the next car
};
#endif // GRID_HPP