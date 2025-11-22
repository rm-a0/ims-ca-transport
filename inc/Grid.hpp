/**
 * @file Grid.hpp
 * @author Michal Repcik (xrepcim00)
 */
#ifndef GRID_HPP
#define GRID_HPP

#include "Cell.hpp"
#include "Rules.hpp"
#include <vector>
#include <fstream>

/**
 * @class Grid
 * @brief Represents a CA grid
 */
class Grid {
public:
    /**
     * @brief Constructor for the Grid class
     * @param w Grid width
     * @param h Grid height
     */
    Grid(int w, int h);

    /**
     * @brief Updates the grid using specified rules
     * @param rules Rules to be applied to each cell
     */
    void update(const Rules& rules);

    /**
     * @brief Counts alive neighbors of cell at (x, y)
     * @param x X coordinate
     * @param y Y coordinate
     * @return Number of alive cells
     */
    int countAliveNeighbors(int x, int y) const;

    /**
     * @brief Export grid into a PPM file
     * @param filename Name of the file
     * @param scale Pixel scale factor
     */
    void exportPPM(const std::string& filename, int scale) const;

    /**
     * @brief Gets cell at (x, y)
     * @param x X coordinate
     * @param y Y coordinate
     * @return Cell at set coordinate
     */
    Cell& getCell(int x, int y) { return cells[x][y]; }
    
private:
    int width;                              ///< Width of the grid
    int height;                             ///< Height of the grid
    std::vector<std::vector<Cell>> cells;   ///< 2D vector containing cells
};

#endif // GRID_HPP_