/**
 * @file Utils.hpp
 * @authors Michal Repcik (xrepcim00)
 */
#ifndef UTILS_HPP 
#define UTILS_HPP

#include <array>
#include "Grid.hpp"
#include "Cell.hpp"

enum class Colormap{
    Turbo,
    Magma,
    Viridis
};

namespace Utils
{
    std::array<unsigned char, 3> idColormap(int id);

    std::array<unsigned char, 3> velocityColormap(int velocity, int vmax, Colormap cmap);

    void exportPPM(const Grid& grid,
                   const std::string& filename,
                   int scale,
                   int vmax);

    void exportSmoothPPM(const Grid& currentGrid,
                         const Grid& nextGrid,
                         const std::string& filename,
                         int scale,
                         int vmax,
                         float t);
}

#endif // UTILS_HPP