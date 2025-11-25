/**
 * @file Cell.hpp
 * @author Michal Repcik (xrepcim00)
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
     * @brief Sets cell velocity
     * @param velocity Value to be set (-1=empty, >=0=car (velocity))
     */
    void setVelocity(int velocity);

    /**
     * @brief Getter for Cell velocity
     */
    int getVelocity() const { return velocity; }

    /**
     * @brief Check if cell has a car
     */
    bool hasCar() const { return velocity >= 0; }

private:
    int velocity;   /// Cell velocity (-1=empty, 0..vmax=car speed)
};

#endif // CELL_HPP