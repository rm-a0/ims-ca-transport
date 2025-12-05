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

    void setupCrossroadLights(int redDur, int yellowDur, int greenDur);
    void initializeMap(double density, bool opt);
    /**
     * @brief Updates the grid using specified rules (NS for traffic)
     * @param rules Rules to be applied
     * @param vmax Max velocity
     * @param p Braking probability
     */
    void update(const Rules& rules, double density, int vmax, double p);
    /**
     * @brief Finds distance to next car ahead (toroidal)
     * @param x X coordinate
     * @param y Y coordinate (lane)
     * @return Distance (cells) to next car
     */
    int distanceToNextCar(int x, int y) const;
    /**
     * @brief Computes average velocity of all cars
     * @return Avg velocity (or 0 if no cars)
     */
    double averageVelocity() const;
    /**
     * @brief Gets cell at (y, x)
     * @param y Y coordinate
     * @param x X coordinate
     * @return Cell at set coordinate
     */
    Cell& getCell(int y, int x) { return cells[y][x]; }
    const Cell& getCell(int y, int x) const { return cells[y][x]; }
    
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

    /**
     * @brief Sets the maximum number of cars in the simulation
     * @param mC Maximum number of cars
     */
    void setMaxCars(int mC) { maxCars = mC; }
    /**
     * @brief Gets the maximum number of cars in the simulation
     * @return Maximum number of cars
     */
    int getMaxCars() const { return maxCars; }

    /**
     * @brief Gets the next unique car ID and increments the internal counter
     * @return Next car ID
     */
    int getNextCarId() { return nextCarId++; }

    /**
     * @brief Determines if a car spawned at (x, y) will turn at the next turn block
     * @param x X coordinate
     * @param y Y coordinate
     * @return 0.0 if on a straight only lane, 1.0 if on a turn only lane, default willTurnProb otherwise
     */
    double calculateWillTurnProbability(int x, int y);

    /**
     * @brief Determines the initial direction of a car spawned at (x, y)
     * @param x X coordinate
     * @param y Y coordinate
     * @return Direction enum value representing initial direction
     */
    Direction getInitialDirection(int x, int y);

    /**
     * @brief Creates right turn lanes at the junction
     * @param x X coordinate of the turn block
     * @param y Y coordinate of the turn block
     * @param fromDir Direction from which the car is coming
     * @param distFromTrafficLight Distance from the traffic light to the turn block
     */
    void createRightTurnLanes(int x, int y, Direction fromDir, int distFromTrafficLight);
private:
    int width;                              ///< Width of the grid
    int height;                             ///< Height of the grid
    std::vector<std::vector<Cell>> cells;   ///< 2D vector containing cells
    int nextCarId = 0;                      ///< ID of the next car

    // Traffic light durations (yellow is calculated from green -> 90% green / 10% yellow)
    int northInRedDuration = 0;             ///< Red light duration for north inbound
    int northInGreenDuration = 100;         ///< Green light duration for north inbound
    int southInRedDuration = 100;           ///< Red light duration for south inbound
    int southInGreenDuration = 0;           ///< Green light duration for south inbound
    int westInRedDuration = 100;            ///< Red light duration for west inbound
    int westInGreenDuration = 0;            ///< Green light duration for west inbound
    int eastInStraightRedDuration = 100;    ///< Red light duration for east inbound (straight)
    int eastInStraightGreenDuration = 0;    ///< Green light duration for east inbound (straight)
    int eastInTurnRedDuration = 100;        ///< Red light duration for east inbound (left turn)
    int eastInTurnGreenDuration = 0;        ///< Green light duration for east inbound (left turn)

    int numLanesNorthIn = 3;  ///< From NORTH to SOUTH (towards junction)
    int numLanesNorthOut = 2; ///< From SOUTH to NORTH (away from junction)
    int numLanesWestIn = 2;   ///< From WEST to EAST (towards junction)
    int numLanesWestOut = 2;  ///< From EAST to WEST (away from junction)
    int numLanesSouthIn = 3;  ///< From SOUTH to NORTH (towards junction)
    int numLanesSouthOut = 2; ///< From NORTH to SOUTH (away from junction)
    int numLanesEastIn = 3;   ///< From EAST to WEST (towards junction)
    int numLanesEastOut = 2;  ///< From WEST to EAST (away from junction)

    // Lane spacing from center (between inbound and outbound)
    int northLaneSpace = 1;
    int westLaneSpace = 2;
    int southLaneSpace = 1;
    int eastLaneSpace = 1;

    // Total number of lanes for each direction
    int numLanesNorth = numLanesNorthIn + numLanesNorthOut;
    int numLanesWest = numLanesWestIn + numLanesWestOut;
    int numLanesSouth = numLanesSouthIn + numLanesSouthOut;
    int numLanesEast = numLanesEastIn + numLanesEastOut;

    int totalLaneCells;   ///< Total number of cells that are part of lanes
    int maxCars;          ///< Maximum number of cars in the simulation
    int currentCars = 0;  ///< Current number of cars in the simulation

    int distFromTrafficLight = 10; ///< Distance from traffic light to turn block for right lane turns

    double spawnProb = 0.2; ///< Probability of spawning a car at spawn point per update

    double willTurnProb = 0.4; ///< Probability that a car will turn at the next turn block

    bool normalize = false; ///< If --optimize is set then this becomes 1 to shift all affected areas
};

#endif // GRID_HPP