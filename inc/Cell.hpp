/**
 * @file Cell.hpp
 * @authors Michal Repcik (xrepcim00), Adam Vesely (xvesela00)
 */
#ifndef CELL_HPP
#define CELL_HPP 

/**
 * @class Cell
 * @brief Represents a single Cell in CA grid for traffic (velocity: -1=empty, >=0=car speed)
 */
class Cell {
public:
    /**
     * @brief Constructor for the Cell class
     * @param velocity Initial cell velocity (-1 for empty)
     */
    Cell(int velocity = -1);

    /**
     * @brief Setters
     */
    void setVelocity(int velocity);
    void setCarId(int id);

    /**
     * @brief Getters
     */
    int getVelocity() const { return velocity; }
    bool hasCar() const { return velocity >= 0; }
    int getCarId() const {return carId; }

private:
    int velocity;       ///< Cell velocity (-1=empty, 0..vmax=car speed)
    int carId = -1;     ///< ID of a car (for colormapping)
};

#endif // CELL_HPP