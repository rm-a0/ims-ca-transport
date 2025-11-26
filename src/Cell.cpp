#include "Cell.hpp"
#include <iostream>

Cell::Cell(int vel) : velocity(vel), carId(-1) {}

void Cell::setVelocity(int vel) {
    velocity = vel;
}

void Cell::setCarId(int id) {
    carId = id;
}
