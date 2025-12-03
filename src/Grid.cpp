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

void Grid::initializeMap(double density) {
    int centerX = width / 2;
    int centerY = height / 2;

    int totalLanes = numLanesNorth + numLanesWest + numLanesSouth + numLanesEast;
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
                double r = rand() / double(RAND_MAX);
                if (currentCars < maxCars && r <= spawnProb)
                    next[y][x].spawnCar(vmax, willTurnProb, nextCarId++, getInitialDirection(x, y));
                    currentCars++;
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
                newX = (x + newVel * dx + width) % width;
            }
            if (dy != 0) {
                newY = (y + newVel * dy + height) % height;
            }
           
            moves.push_back({x, y, newX, newY, newVel, cells[y][x].getCarId(), dir});
        }
    }
   
    // Second pass: Apply moves, checking for conflicts
    for (const auto& move : moves) {
        cells[move.oldY][move.oldX].moveCarTo(next[move.newY][move.newX]);
        if (next[move.newY][move.newX].hasTurn())
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
        cx = (cx + dx + width) % width;
        cy = (cy + dy + height) % height;
      
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

        if (cells[cy][cx].hasTurn()) {
            return dist + 1;
        }
      
        dist++;
    }
    return loop_size;
}

void Grid::exportPPM(const std::string& filename, int scale, int vmax) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file) return;
   
    file << "P6\n" << width * scale << " " << height * scale << "\n255\n";
   
    for (int py = 0; py < height * scale; py++) {
        for (int px = 0; px < width * scale; px++) {
            int cellX = px / scale;
            int cellY = py / scale;
           
            unsigned char r, g, b;
           
            // Check for traffic light first
            if (cells[cellY][cellX].hasTrafficLight()) {
                switch (cells[cellY][cellX].getTrafficLightState()) {
                    case TrafficLight::RED:
                        r = 255; g = 0; b = 0;
                        break;
                    case TrafficLight::YELLOW:
                        r = 255; g = 255; b = 0;
                        break;
                    case TrafficLight::GREEN:
                        r = 0; g = 255; b = 0;
                        break;
                }
            }
            // Check for car
            else if (cells[cellY][cellX].hasCar()) {
                int id = cells[cellY][cellX].getCarId();
                int vel = cells[cellY][cellX].getCarVelocity();
                auto [rr, gg, bb] = Utils::velocityColormap(vel, vmax, Colormap::Turbo);
                r = rr; g = gg; b = bb;
            }
            // Check for turn
            else if (cells[cellY][cellX].hasTurn()) {
                r = 255; g = 255; b = 255;
            }
            // Check for spawn point
            else if (cells[cellY][cellX].isSpawnPoint()) {
                r = 0; g = 0; b = 255;
            }
            // Check for alive cell (road)
            else if (cells[cellY][cellX].isAlive()) {
                r = 0; g = 0; b = 0;
            }
            // Empty cell
            else {
                r = 50; g = 50; b = 50; // Dark gray for road
            }
           
            file.put(r);
            file.put(g);
            file.put(b);
        }
    }
}

void Grid::exportSmoothPPM(const std::string& filename, int scale, int vmax,
                           const Grid& nextGrid, float t) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file) return;
   
    file << "P6\n" << width * scale << " " << height * scale << "\n255\n";
   
    // Pre-compute interpolated car positions in a lookup grid
    std::vector<std::vector<int>> carAtCell(height, std::vector<int>(width, -1));
    std::vector<std::vector<int>> velAtCell(height, std::vector<int>(width, 0));
   
    // Build map of car IDs to positions
    std::map<int, std::pair<int, int>> currentPos;
    std::map<int, std::pair<int, int>> nextPos;
    std::map<int, int> carVelocities;
   
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (cells[y][x].hasCar()) {
                int id = cells[y][x].getCarId();
                currentPos[id] = {x, y};
                carVelocities[id] = cells[y][x].getCarVelocity();
            }
            if (nextGrid.cells[y][x].hasCar()) {
                int id = nextGrid.cells[y][x].getCarId();
                nextPos[id] = {x, y};
            }
        }
    }
   
    // Place interpolated cars in grid
    for (const auto& [id, currXY] : currentPos) {
        if (nextPos.find(id) == nextPos.end()) continue;
       
        int x1 = currXY.first;
        int y1 = currXY.second;
        int x2 = nextPos[id].first;
        int y2 = nextPos[id].second;
       
        // Handle wrapping
        int dx = x2 - x1;
        int dy = y2 - y1;
       
        if (abs(dx) > width / 2) {
            if (dx > 0) x2 -= width;
            else x2 += width;
        }
        if (abs(dy) > height / 2) {
            if (dy > 0) y2 -= height;
            else y2 += height;
        }
       
        // Interpolate position
        float interpX = x1 + t * (x2 - x1);
        float interpY = y1 + t * (y2 - y1);
       
        // Wrap back to grid
        int finalX = ((int)round(interpX) % width + width) % width;
        int finalY = ((int)round(interpY) % height + height) % height;
       
        carAtCell[finalY][finalX] = id;
        velAtCell[finalY][finalX] = carVelocities[id];
    }
   
    for (int py = 0; py < height * scale; py++) {
        for (int px = 0; px < width * scale; px++) {
            int cellX = px / scale;
            int cellY = py / scale;
           
            unsigned char r, g, b;
           
            // Check for traffic light first
            if (cells[cellY][cellX].hasTrafficLight()) {
                switch (cells[cellY][cellX].getTrafficLightState()) {
                    case TrafficLight::RED:
                        r = 255; g = 0; b = 0;
                        break;
                    case TrafficLight::YELLOW:
                        r = 255; g = 255; b = 0;
                        break;
                    case TrafficLight::GREEN:
                        r = 0; g = 255; b = 0;
                        break;
                }
            }
            // Check for car (fast lookup!)
            else if (carAtCell[cellY][cellX] != -1) {
                int vel = velAtCell[cellY][cellX];
                auto [rr, gg, bb] = Utils::velocityColormap(vel, vmax, Colormap::Turbo);
                r = rr; g = gg; b = bb;
            }
            // Empty cell
            else {
                r = 50; g = 50; b = 50;
            }
           
            file.put(r);
            file.put(g);
            file.put(b);
        }
    }
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