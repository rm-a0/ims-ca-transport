/**
 * @file Cell.hpp
 * @author Michal Repcik (xrepcim00)
 */
#ifndef CELL_HPP
#define CELL_HPP 

/**
 * @class Cell
 * @brief Represents a single Cell in CA grid
 */
class Cell {
public:
    /**
     * @brief Constructor for the Cell class
     * @param state Initial cell state
     */
    Cell(bool state = false);

    /**
     * @brief Sets cell state
     * @param state Value to be set
     */
    void setAlive(bool state);

    /**
     * @brief Toggle the Cell state
     */
    void toggle();

    /**
     * @brief Getter for Cell state
     */
    bool isAlive() const { return alive; }

private:
    bool alive;     ///< Cell state
};

#endif // CELL_HPP__