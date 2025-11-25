#include "Rules.hpp"
#include <cstdlib>
#include <algorithm>

int NSRules::nextVelocity(int currentVel, int distToNext, int vmax, double p) const {
    if (currentVel < 0) return -1;  // Empty stays empty here (movement handled in Grid)

    // 1. Acceleration
    int v = std::min(currentVel + 1, vmax);

    // 2. Deceleration
    v = std::min(v, distToNext - 1);

    // 3. Randomization
    if ((static_cast<double>(rand()) / RAND_MAX) < p) {
        v = std::max(v - 1, 0);
    }

    return v;
}