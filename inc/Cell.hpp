/**
 * @file Cell.hpp
 * @authors Michal Repcik (xrepcim00), Adam Vesely (xvesela00)
 */
#ifndef CELL_HPP
#define CELL_HPP 

#include <optional>

struct Car {
    int id;
    int velocity;
};

struct Road {
    enum Direction { LEFT, RIGHT, UP, DOWN } direction;
};

struct TrafficLight {
    enum State { RED, YELLOW, GREEN } state;

    int redDuration;
    int yellowDuration;
    int greenDuration;
    int timer;

    void update();
};

/**
 * @class Cell
 * @brief Represents a single Cell in CA grid for traffic (velocity: -1=empty, >=0=car speed)
 */
class Cell {
public:
    /** Constructor */
    Cell(int velocity = -1);

    void moveCarTo(Cell& dest);

    /** Alive setter/getter */
    void setAlive(bool val) { alive = val; }
    bool isAlive() const { return alive; }

    /** Car setters/getters */
    void setCar(const Car& c) { car = c; }
    void setCarVelocity(int v);
    void setCarId(int id);
    int getCarVelocity() const { return car ? car->velocity : -1; }
    int getCarId() const { return car ? car->id : -1; }
    bool hasCar() const { return car.has_value(); }
    void removeCar();

    /** Road setters/getters */
    void setRoad(const Road& r);
    void setRoadDirection(Road::Direction dir);
    bool hasRoad() const { return road.has_value(); }

    /** TrafficLight setters/getters */
    void setTrafficLight(const TrafficLight& t);
    bool hasTrafficLight() const { return tl.has_value(); }
    void updateTrafficLight();

private:
    std::optional<Car> car;
    std::optional<Road> road;
    std::optional<TrafficLight> tl;
    bool alive;
};

#endif // CELL_HPP
