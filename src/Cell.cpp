#include "Cell.hpp"

Cell::Cell(bool state) : alive(state = false) {}

void Cell::setAlive(bool state) {
    alive = state;
}

void Cell::toggle() {
    alive = !alive;
}
