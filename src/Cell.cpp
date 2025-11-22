#include "Cell.hpp"
#include <iostream>

Cell::Cell(bool state) : alive(state) {}

void Cell::setAlive(bool state) {
    alive = state;
}

void Cell::toggle() {
    alive = !alive;
}
