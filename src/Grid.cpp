/**
 * @file Grid.cpp
 * @authors Michal Repcik (xrepcim00), Adam Vesely (xvesela00)
 */
#include "Grid.hpp"
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include "Utils.hpp"
#include <cmath>
#include <map>
#include <iostream>

Grid::Grid(int w, int h) : width(w), height(h) {
    cells.resize(height, std::vector<Cell>(width));
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            cells[y][x] = Cell(); // empty
}

void Grid::initializeMap(double density, bool opt) {
    if (opt) {
        numLanesEastIn = 4;
        numLanesWestOut = 3;
        normalize = true;
    }

    int centerX = width / 2;
    int centerY = height / 2;

    totalLaneCells = (numLanesWest + numLanesEast) * height + (numLanesNorth + numLanesSouth) * width;
    maxCars = static_cast<int>(totalLaneCells * density);

    // Number of cells per direction (northHeight will go to the southOut end of the junction)
    int northHeight = centerY + numLanesWestIn;
    int westWidth = centerX + numLanesSouthIn;
    int southHeight = centerY - numLanesEastIn;
    int eastWidth = centerX - numLanesNorthIn;


    // === MARK ROAD CELLS AS ALIVE AND SET SPAWN POINTS ===

    // VERTICAL ROADS (North-South)
    // North inbound: top side, cars going DOWN -> left of center (x < centerX)
    for (int lane = 0; lane < numLanesNorthIn; lane++) {
        int x = centerX - numLanesNorthIn + lane;
        if (x < 0 || x >= width) continue;
        // Set the edge road cell as spawn point for all cars coming from north
        cells[0][x].setSpawnPoint(true);
        for (int y = 0; y < northHeight; y++)
            cells[y][x].setAlive(true);
    }

    // North outbound: top side, cars going UP -> right of center (x > centerX)
    for (int lane = 0; lane < numLanesNorthOut; lane++) {
        int x = centerX + northLaneSpace + lane;
        if (x < 0 || x >= width) continue;
        for (int y = 0; y < northHeight; y++)
            cells[y][x].setAlive(true);
    }

    // South inbound: bottom side, cars going UP -> right of center (x > centerX)
    for (int lane = 0; lane < numLanesSouthIn; lane++) {
        int x = centerX + lane;
        if (x < 0 || x >= width) continue;
        // Set the edge road cell as spawn point for all cars coming from south
        cells[height - 1][x].setSpawnPoint(true);
        for (int y = height - 1; y > southHeight; y--)
            cells[y][x].setAlive(true);
    }

    // South outbound: bottom side, cars going DOWN -> left of center (x < centerX)
    for (int lane = 0; lane < numLanesSouthOut; lane++) {
        int x = centerX - numLanesSouthOut - southLaneSpace + lane;
        if (x < 0 || x >= width) continue;
        for (int y = height - 1; y > southHeight; y--)
            cells[y][x].setAlive(true);
    }

    // HORIZONTAL ROADS (West-East)
    // West inbound: left side, cars going RIGHT -> below center (y > centerY)
    for (int lane = 0; lane < numLanesWestIn; lane++) {
        int y = centerY + lane;
        if (y < 0 || y >= height) continue;
        // Set the edge road cell as spawn point for all cars coming from west
        cells[y][0].setSpawnPoint(true);
        for (int x = 0; x < westWidth; x++)
            cells[y][x].setAlive(true);
    }

    // West outbound: left side, cars going LEFT -> above center (y < centerY)
    for (int lane = 0; lane < numLanesWestOut; lane++) {
        int y = centerY - numLanesWestOut - westLaneSpace + lane;
        if (y < 0 || y >= height) continue;
        for (int x = 0; x < westWidth; x++)
            cells[y][x].setAlive(true);
    }

    // East inbound: right side, cars going LEFT -> above center (y < centerY)
    for (int lane = 0; lane < numLanesEastIn; lane++) {
        int y = centerY - numLanesEastIn - eastLaneSpace + lane;
        if (y < 0 || y >= width) continue;
        // Set the edge road cell as spawn point for all cars coming from east
        cells[y][width - 1].setSpawnPoint(true);
        for (int x = width - 1; x > eastWidth; x--)
            cells[y][x].setAlive(true);
    }

    // East outbound: right side, cars going RIGHT -> below center (y > centerY)
    for (int lane = 0; lane < numLanesEastOut; lane++) {
        int y = centerY + lane;
        if (y < 0 || y >= height) continue;
        for (int x = width - 1; x > eastWidth; x--)
            cells[y][x].setAlive(true);
    }

    // Turn blocks at the junction
    Turn t0;
    Turn t1;
    Turn t2;
    Turn t3;
    t0.direction = Direction::UP;
    t1.direction = Direction::LEFT;
    t2.direction = Direction::DOWN;
    t3.direction = Direction::RIGHT;

    // Turns for cars coming from NORTH
    int y_northRight0 = northHeight - westLaneSpace;
    int x_northRight0 = centerX - northLaneSpace;
    cells[y_northRight0][x_northRight0].setTurn(t3);

    // Turns for cars coming from SOUTH
    int y_southLeft0 = southHeight;
    int x_southLeft0 = centerX;
    int y_southLeft1 = southHeight - eastLaneSpace;
    int x_southLeft1 = centerX + southLaneSpace;
    cells[y_southLeft0][x_southLeft0].setTurn(t1);
    cells[y_southLeft1][x_southLeft1].setTurn(t1);

    // Turns for cars coming from EAST
    int y_eastDown0 = centerY - westLaneSpace;
    int x_eastDown0 = eastWidth + northLaneSpace;
    cells[y_eastDown0][x_eastDown0].setTurn(t2);
}

void Grid::setupCrossroadLights(int redDur, int yellowDur, int greenDur) {
    int centerX = width / 2;
    int centerY = height / 2;

    // Traffic lights for west inbound
    for (int lane = 0; lane < numLanesWestIn; lane++) {
        int x = centerX - numLanesNorthIn - northLaneSpace;
        int y = centerY + lane;
        if (x >= 0 && x < width && y >= 0 && y < height) {
            TrafficLight tl;
            tl.state = TrafficLight::GREEN;
            tl.redDuration = redDur;
            tl.yellowDuration = yellowDur;
            tl.greenDuration = greenDur;
            tl.timer = 0;
            cells[y][x].setTrafficLight(tl);
            // Create right turn lane
            if (lane == numLanesWestIn - 1) {
                createRightTurnLanes(x, y, Direction::LEFT, distFromTrafficLight);
            }
        }
    }

    // Traffic lights for east inbound
    for (int lane = 0; lane < numLanesEastIn; lane++) {
        int x = centerX + numLanesSouthIn;
        int y = centerY - numLanesEastIn - eastLaneSpace + lane;
        if (x >= 0 && x < width && y >= 0 && y < height) {
            TrafficLight tl;
            tl.state = TrafficLight::GREEN;
            tl.redDuration = redDur;
            tl.yellowDuration = yellowDur;
            tl.greenDuration = greenDur;
            tl.timer = 0;
            cells[y][x].setTrafficLight(tl);
            // Create right turn lane
            if (lane == 0) {
                createRightTurnLanes(x, y, Direction::RIGHT, distFromTrafficLight);
            }
        }
    }

    // Traffic lights for north inbound
    for (int lane = 0; lane < numLanesNorthIn; lane++) {
        int y = centerY - numLanesEastIn - eastLaneSpace - 1;
        int x = centerX - lane - northLaneSpace;
        if (x >= 0 && x < width && y >= 0 && y < height) {
            TrafficLight tl;
            tl.state = TrafficLight::RED;
            tl.redDuration = redDur;
            tl.yellowDuration = yellowDur;
            tl.greenDuration = greenDur;
            tl.timer = 0;
            cells[y][x].setTrafficLight(tl);
            // Create right turn lane
            if (lane == numLanesNorthIn - 1) {
                createRightTurnLanes(x, y, Direction::UP, distFromTrafficLight);
            }
        }
    }

    // Traffic lights south inbound
    for (int lane = 0; lane < numLanesSouthIn; lane++) {
        int y = centerY + numLanesWestIn;
        int x = centerX + numLanesSouthIn - lane - 1;
        if (x >= 0 && x < width && y >= 0 && y < height) {
            TrafficLight tl;
            tl.state = TrafficLight::RED;
            tl.redDuration = redDur;
            tl.yellowDuration = yellowDur;
            tl.greenDuration = greenDur;
            tl.timer = 0;
            cells[y][x].setTrafficLight(tl);
            // Create right turn lane
            if (lane == 0) {
                createRightTurnLanes(x, y, Direction::DOWN, distFromTrafficLight);
            }
        }
    }
}

void Grid::update(const Rules& rules, double density, int vmax, double p) {
    std::vector<std::vector<Cell>> next(height, std::vector<Cell>(width, Cell()));
    // Copy traffic lights and turns to next state
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (cells[y][x].hasTrafficLight()) {
                cells[y][x].updateTrafficLight();
                next[y][x].setTrafficLight(*cells[y][x].getTrafficLight());
            }
            if (cells[y][x].hasTurn()) {
                next[y][x].setTurn(*cells[y][x].getTurn());
            }
            if (cells[y][x].isSpawnPoint()) {
                double willTurnProbability = calculateWillTurnProbability(x, y);
                double r = rand() / double(RAND_MAX);
                if (currentCars < maxCars && r <= spawnProb) {
                    next[y][x].spawnCar(vmax, willTurnProbability, nextCarId++, getInitialDirection(x, y));
                    currentCars++;
                }
                next[y][x].setSpawnPoint(true);
            }
            if (cells[y][x].isAlive()) {
                next[y][x].setAlive(true);
            }
        }
    }
   
    // First pass: Calculate desired positions and velocities for all cars
    struct CarMove {
        int oldX, oldY;
        int newX, newY;
        int newVel;
        int carId;
        Direction dir;
        bool willTurn;
    };
    std::vector<CarMove> moves;
   
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (!cells[y][x].hasCar()) continue;
            Direction dir = cells[y][x].getCarDirection();
            int dx = 0, dy = 0;
            if (dir == Direction::RIGHT) { dx = 1; dy = 0; }
            else if (dir == Direction::LEFT) { dx = -1; dy = 0; }
            else if (dir == Direction::UP) { dx = 0; dy = -1; }
            else if (dir == Direction::DOWN) { dx = 0; dy = 1; }
       
            int currentVel = cells[y][x].getCarVelocity();
            int dist = distanceToNextCar(x, y);
           
            // Apply Nagel-Schreckenberg rules
            int newVel = rules.nextVelocity(currentVel, dist, vmax, p);
       
            // Calculate new position with wrapping
            int newX = x;
            int newY = y;
           
            if (dx != 0) {
                newX = x + newVel * dx;
            }
            if (dy != 0) {
                newY = y + newVel * dy;
            }

            // Check if the car is leaving the grid (if so then remove it)
            if (newX >= width || newX < 0 || newY >= height || newY < 0) {
                cells[y][x].removeCar();
                currentCars--;
                continue;
            }
           
            moves.push_back({x, y, newX, newY, newVel, cells[y][x].getCarId(), dir, cells[y][x].getCarWillTurn()});
        }
    }
   
    // Second pass: Apply moves, checking for conflicts
    for (const auto& move : moves) {
        bool turn = cells[move.oldY][move.oldX].getCarWillTurn();
        cells[move.oldY][move.oldX].moveCarTo(next[move.newY][move.newX]);
        if (next[move.newY][move.newX].hasTurn() && turn)
            next[move.newY][move.newX].setCarDirection(next[move.newY][move.newX].getTurnDirection());
        next[move.newY][move.newX].setCarVelocity(move.newVel);
    }
   
    cells = std::move(next);
}

int Grid::distanceToNextCar(int x, int y) const {
    if (!cells[y][x].hasCar()) return 0; // Should not happen
  
    Direction dir = cells[y][x].getCarDirection();
    int dx = 0, dy = 0;
    int loop_size = 0;
    if (dir == Direction::RIGHT || dir == Direction::LEFT) {
        loop_size = width;
        dx = (dir == Direction::RIGHT) ? 1 : -1;
    } else if (dir == Direction::UP || dir == Direction::DOWN) {
        loop_size = height;
        dy = (dir == Direction::DOWN) ? 1 : -1;
    }
  
    int dist = 1;
    int cx = x;
    int cy = y;
  
    while (dist < loop_size) {
        cx = cx + dx;
        cy = cy + dy;

        if (cx >= width || cx < 0 || cy >= height || cy < 0) {
            // Out of bounds
            return dx != 0 ? width : height;
        }
      
        // Check for red traffic light
        if (cells[cy][cx].hasTrafficLight()) {
            if (cells[cy][cx].getTrafficLightState() == TrafficLight::RED) {
                return dist;
            }
        }
      
        // Check for another car
        if (cells[cy][cx].hasCar()) {
            return dist;
        }

        if (cells[cy][cx].hasTurn() && cells[y][x].getCarWillTurn()) {
            return dist + 1;
        }
      
        dist++;
    }
    return loop_size;
}

double Grid::averageVelocity() const {
    int totalVel = 0;
    int carCount = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (cells[y][x].hasCar()) {
                totalVel += cells[y][x].getCarVelocity();
                carCount++;
            }
        }
    }
    return carCount > 0 ? (double)totalVel / carCount : 0.0;
}

Direction Grid::getInitialDirection(int x, int y) {
    int centerX = width / 2;
    int centerY = height / 2;

    // Determine direction based on position relative to center
    // I. quadrant
    if (x > centerX && y < centerY) {
        return Direction::LEFT; // East inbound
    }
    // II. quadrant
    else if (x < centerX && y < centerY) {
        return Direction::DOWN; // North inbound
    }
    // III. quadrant
    else if (x < centerX && y >= centerY) {
        return Direction::RIGHT; // West inbound
    }
    // IV. quadrant
    else if (x >= centerX && y > centerY) {
        return Direction::UP; // South inbound
    }
}

double Grid::calculateWillTurnProbability(int x, int y) {
    int centerX = width / 2;
    int centerY = height / 2;
    // NORTH INBOUND straight only lane (48, 0)
    if (x == (centerX - northLaneSpace - 1) && y == 0) {
        return 0.0;
    }
    // NORTH INBOUND turn only lane (49, 0)
    if (x == (centerX - northLaneSpace) && y == 0) {
        return 1.0;
    }
    // EAST INBOUND straight only lane (99, 46) [only with --optimize]
    if (normalize && x == (width - 1) && y == (centerY - eastLaneSpace - 3)) {
        return 0.0;
    }
    // EAST INBOUND straight only lane (99, 47)
    if (x == (width - 1) && y == (centerY - eastLaneSpace - 2)) {
        return 0.0;
    }
    // EAST INBOUND turn only lane (99, 48)
    if (x == (width - 1) && y == (centerY - eastLaneSpace - 1)) {
        return 1.0;
    }
    // WEST INBOUND straight only lane (0, 50)
    if (x == 0 && y == centerY) {
        return 0.0;
    }
    // SOUTH INBOUND turn only lane (50, 99)
    if (x == centerX && y == (height - 1)) {
        return 1.0;
    }
    return willTurnProb;
}

void Grid::createRightTurnLanes(int x, int y, Direction fromDir, int distFromTrafficLight) {
    Turn t0;
    Turn t1;
    Turn t2;
    Turn t3;
    t0.direction = Direction::LEFT;
    t1.direction = Direction::DOWN;
    t2.direction = Direction::RIGHT;
    t3.direction = Direction::UP;
    // Create right turn lane for north inbound
    if (fromDir == Direction::UP) {
        int newX = x - distFromTrafficLight;
        int newY = y - distFromTrafficLight;
        if (newX < 0 || newY < 0) return;
        // 1st turn block
        cells[newY][x].setTurn(t0);

        // Create seperate lane for right turn (horizontal part)
        for (int i = 0; i <= distFromTrafficLight; i++) {
            if (i == distFromTrafficLight) {
                cells[newY][x - i].setTurn(t1);
                continue;
            }
            cells[newY][x - i].setAlive(true);
        }
        // Create vertical part of the right turn lane
        for (int i = 0; i <= distFromTrafficLight + 1; i++) {
            if (i == distFromTrafficLight + 1) {
                cells[newY + i][newX].setTurn(t0);
                continue;
            }
            cells[newY + i][newX].setAlive(true);
        }
    }
    // Create right turn lane for south inbound
    else if (fromDir == Direction::DOWN) {
        int newX = x + distFromTrafficLight;
        int newY = y + distFromTrafficLight;
        if (newX < 0 || newY < 0) return;
        // 1st turn block
        cells[newY][x].setTurn(t2);

        // Create seperate lane for right turn (horizontal part)
        for (int i = 0; i <= distFromTrafficLight; i++) {
            if (i == distFromTrafficLight) {
                cells[newY][x + i].setTurn(t3);
                continue;
            }
            cells[newY][x + i].setAlive(true);
        }
        // Create vertical part of the right turn lane
        for (int i = 0; i <= distFromTrafficLight + 1; i++) {
            if (i == distFromTrafficLight + 1) {
                cells[newY - i][newX].setTurn(t2);
                continue;
            }
            cells[newY - i][newX].setAlive(true);
        }
    }
    // Create right turn lane for west inbound
    else if (fromDir == Direction::LEFT) {
        int newX = x - distFromTrafficLight;
        int newY = y + distFromTrafficLight;
        if (newX < 0 || newY < 0) return;
        // 1st turn block
        cells[y][newX].setTurn(t1);

        // Create seperate lane for right turn (vertical part)
        for (int i = 0; i <= distFromTrafficLight; i++) {
            if (i == distFromTrafficLight) {
                cells[y + i][newX].setTurn(t2);
                continue;
            }
            cells[y + i][newX].setAlive(true);
        }
        // Create horizontal part of the right turn lane
        for (int i = 0; i <= distFromTrafficLight + 1; i++) {
            if (i == distFromTrafficLight + 1) {
                cells[newY][newX + i].setTurn(t1);
                continue;
            }
            cells[newY][newX + i].setAlive(true);
        }
    }
    // Create right turn lane for east inbound
    else if (fromDir == Direction::RIGHT) {
        int newX = x + distFromTrafficLight;
        int newY = y - distFromTrafficLight;
        if (newX < 0 || newY < 0) return;
        // 1st turn block
        cells[y][newX].setTurn(t3);

        // Create seperate lane for right turn (vertical part)
        for (int i = 0; i <= distFromTrafficLight; i++) {
            if (i == distFromTrafficLight) {
                cells[y - i][newX].setTurn(t0);
                continue;
            }
            cells[y - i][newX].setAlive(true);
        }
        // Create horizontal part of the right turn lane
        for (int i = 0; i <= distFromTrafficLight + 1; i++) {
            if (i == distFromTrafficLight + 1) {
                cells[newY][newX - i].setTurn(t3);
                continue;
            }
            cells[newY][newX - i].setAlive(true);
        }
    }
}