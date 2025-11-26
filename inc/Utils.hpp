/**
 * @file Utils.hpp
 * @authors Michal Repcik (xrepcim00)
 */
#ifndef UTILS_HPP 
#define UTILS_HPP

#include <array>

enum class Colormap{
    Turbo,
    Magma,
    Viridis
};

namespace Utils
{
    std::array<unsigned char, 3> idColormap(int id);

    std::array<unsigned char, 3> velocityColormap(int velocity, int vmax, Colormap cmap);
}

#endif // UTILS_HPP