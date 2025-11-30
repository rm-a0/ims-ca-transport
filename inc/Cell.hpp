/**
 * @file Cell.hpp
 * @authors Michal Repcik (xrepcim00), Adam Vesely (xvesela00)
 */
#ifndef CELL_HPP
#define CELL_HPP 

#include <optional>

enum Direction { 
    LEFT, 
    RIGHT, 
    UP, 
    DOWN
};

struct Car {
    int id;
    int velocity;
    Direction direction;
};

struct Turn {
    Direction direction;
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
    Cell() : car(std::nullopt), turn(std::nullopt), tl(std::nullopt), alive(false) {}

    void moveCarTo(Cell& dest);

    /** Alive setter/getter */
    void setAlive(bool val) { alive = val; }
    bool isAlive() const { return alive; }

    int getEffectiveVelocity() const;

    /** Car setters/getters */
    void setCar(const Car& c) { car = c; }
    void setCarVelocity(int v);
    void setCarId(int id);
    void setCarDirection(Direction dir);
    Direction getCarDirection() const { return car? car-> direction : Direction::RIGHT; }
    int getCarVelocity() const { return car ? car->velocity : -1; }
    int getCarId() const { return car ? car->id : -1; }
    bool hasCar() const { return car.has_value(); }
    void removeCar();

    /** Road setters/getters */
    void setTurn(const Turn& t);
    void setTurnDirection(Direction dir);
    const std::optional<Turn>& getTurn() const { return turn; }
    Direction getTurnDirection() const { return turn ? turn->direction : Direction::RIGHT; }
    bool hasTurn() const { return turn.has_value(); }

    /** TrafficLight setters/getters */
    void setTrafficLight(const TrafficLight& t);
    bool hasTrafficLight() const { return tl.has_value(); }
    const std::optional<TrafficLight>& getTrafficLight() const { return tl; }
    TrafficLight::State getTrafficLightState() const { return tl ? tl->state : TrafficLight::GREEN; }
    void updateTrafficLight();

private:
    std::optional<Car> car;
    std::optional<Turn> turn;
    std::optional<TrafficLight> tl;
    bool alive;
};

#endif // CELL_HPP
