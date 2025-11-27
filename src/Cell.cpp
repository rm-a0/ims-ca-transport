/**
 * @file Cell.cpp
 * @authors Michal Repcik (xrepcim00), Adam Vesely (xvesela00)
 */

#include "Cell.hpp"

void Cell::setCarDirection(Direction dir) {
    if (car.has_value())
        car->direction = dir;
}

void Cell::setTurnDirection(Direction dir) {
    if (turn.has_value())
        turn->direction = dir;
}

void Cell::setCarVelocity(int v) {
    if (car.has_value())
        car->velocity = v;
    else if (v >= 0)
        car = Car{-1, v};
}

void Cell::setCarId(int id) {
    if (car.has_value())
        car->id = id;
}

void Cell::removeCar() {
    car.reset();
}

void Cell::setTurn(const Turn& t) {
    turn = t;
}

void Cell::setTrafficLight(const TrafficLight& t) {
    tl = t;
}

void Cell::updateTrafficLight() {
    if (tl.has_value())
        tl->update();
}

void TrafficLight::update() {
    timer++;
    switch (state) {
        case RED:
            if (timer >= redDuration) {
                state = GREEN;
                timer = 0;
            }
            break;
        case GREEN:
            if (timer >= greenDuration) {
                state = YELLOW;
                timer = 0;
            }
            break;
        case YELLOW:
            if (timer >= yellowDuration) {
                state = RED;
                timer = 0;
            }
            break;
    }
}

int Cell::getEffectiveVelocity() const {
    if (hasCar()) 
        return getCarVelocity();
    if (hasTrafficLight()) 
        return (getTrafficLightState() == TrafficLight::RED) ? 0 : -1;
    return -1;
}

void Cell::moveCarTo(Cell& dest) {
    if (!car.has_value()) return;
    dest.setCar(*car);
    removeCar();
}
