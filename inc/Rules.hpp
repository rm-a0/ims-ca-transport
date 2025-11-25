/**
 * @file Rules.hpp
 * @author Michal Repcik (xrepcim00), Adam Vesely (xvesela00)
 */

#ifndef RULES_HPP
#define RULES_HPP

/**
 * @class Rules
 * @brief Abstract base class defining rules for the CA
 */
class Rules {
public:
    virtual ~Rules() = default;

    /**
     * @brief Determines the next velocity of a cell (abstract, but for NS we override in subclass)
     */
    virtual int nextVelocity(int currentVel, int distToNext, int vmax, double p) const = 0;
};

/**
 * @class GameOfLifeRules
 * @brief Retained for reference, but not used in traffic model
 */
// class GameOfLifeRules : public Rules {
// public:
//     int nextVelocity(int currentVel, int distToNext, int vmax, double p) const override {
//         // Not used
//         return 0;
//     }
// };

/**
 * @class NSRules
 * @brief Nagel-Schreckenberg rules for traffic flow
 */
class NSRules : public Rules {
public:
    int nextVelocity(int currentVel, int distToNext, int vmax, double p) const override;
};

#endif // RULES_HPP