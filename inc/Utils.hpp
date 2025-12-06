/**
 * @file Utils.hpp
 * @authors Michal Repcik (xrepcim00) Adam Vesely (xvesela00)
 */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <array>
#include "Grid.hpp"
#include "Cell.hpp"

/**
 * @enum Colormap
 * @brief Predefined color maps for visualizing velocities.
 */
enum class Colormap{
    Turbo,   ///< High-contrast, perceptually uniform colormap
    Magma,   ///< Dark-to-light perceptually uniform colormap
    Viridis  ///< Classic perceptually uniform colormap
};

namespace Utils
{
    /**
     * @brief Maps a cell ID to an RGB color.
     * @param id Integer ID of the cell.
     * @return Array of 3 unsigned chars representing RGB values.
     */
    std::array<unsigned char, 3> idColormap(int id);

    /**
     * @brief Maps a cell's velocity to an RGB color using a specified colormap.
     * @param velocity Current velocity of the cell.
     * @param vmax Maximum possible velocity in the simulation.
     * @param cmap Colormap to use for mapping.
     * @return Array of 3 unsigned chars representing RGB values.
     */
    std::array<unsigned char, 3> velocityColormap(int velocity, int vmax, Colormap cmap);

    /**
     * @brief Exports the grid as a PPM image.
     * @param grid Grid object to export.
     * @param filename Output filename for the PPM image.
     * @param scale Factor to scale up the image resolution.
     * @param vmax Maximum velocity for color mapping.
     */
    void exportPPM(const Grid& grid,
                   const std::string& filename,
                   int scale,
                   int vmax);

    /** 
     * @brief Exports a smoothly interpolated PPM image between two grid states.
     * @param currentGrid Current state of the grid.
     * @param nextGrid Next state of the grid.
     * @param filename Output filename for the PPM image.
     * @param scale Factor to scale up the image resolution.
     * @param vmax Maximum velocity for color mapping.
     * @param t Interpolation factor [0.0, 1.0] between current and next grid.
     */
    void exportSmoothPPM(const Grid& currentGrid,
                         const Grid& nextGrid,
                         const std::string& filename,
                         int scale,
                         int vmax,
                         float t);
}

#endif // UTILS_HPP
