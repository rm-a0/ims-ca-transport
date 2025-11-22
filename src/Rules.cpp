#include "Rules.hpp"

bool GameOfLifeRules::nextState(bool currentAlive, int aliveNeighbors) const {
    if (currentAlive) {
        return aliveNeighbors == 2 || aliveNeighbors == 3;
    } else {
        return aliveNeighbors == 3;
    }
}