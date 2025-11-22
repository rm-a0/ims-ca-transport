/**
 * @file Rules.hpp
 * @author Michal Repcik (xrepcim00)
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
     * @brief Determines the next state of a cell based on current state and neighbors
     * @param currentState The current state of the cell
     * @param aliveNeighbors Number of alive neighboring cells
     * @return The next state of the cell
     */ 
    virtual bool nextState(bool currentState, int aliveNeighbors) const = 0;
};

/**
 * @class GameOfLifeRules
 * @brief Example implementation of Conway's Game of Life rules
 */
class GameOfLifeRules : public Rules {
public:
    bool nextState(bool currentAlive, int aliveNeighbors) const override;
};

#endif // RULES_HPP__
