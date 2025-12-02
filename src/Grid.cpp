#include "Grid.hpp"
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include "Utils.hpp"
#include <cmath>
#include <map>

Grid::Grid(int w, int h) : width(w), height(h) {
    cells.resize(height, std::vector<Cell>(width));
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            cells[y][x] = Cell(); // empty
}

void Grid::initializeCarsWithDensity(double density, int maxVelocity) {
    int numLanes = 3;
    int laneSpace = 1;
    int centerX = width / 2;
    int centerY = height / 2;
   
    // Calculate total lane cells (12 lanes: 6 vertical + 6 horizontal)
    int totalLaneCells = 6 * height + 6 * width;
    int targetCars = static_cast<int>(totalLaneCells * density);
   
    int carsSpawned = 0;

    // set all lane cells as alive (road)
    for (int lane = 0; lane < numLanes; lane++) {
        // Vertical lanes
        for (int y = 0; y < height; y++) {
            int x1 = centerX - numLanes - laneSpace + lane;
            int x2 = centerX + lane;
            if (x1 >= 0 && x1 < width)
                cells[y][x1].setAlive(true);
            if (x2 >= 0 && x2 < width)
                cells[y][x2].setAlive(true);
        }
        // Horizontal lanes
        for (int x = 0; x < width; x++) {
            int y1 = centerY - numLanes - laneSpace + lane;
            int y2 = centerY + lane;
            if (y1 >= 0 && y1 < height)
                cells[y1][x].setAlive(true);
            if (y2 >= 0 && y2 < height)
                cells[y2][x].setAlive(true);
        }
    }
   
    // Try to spawn cars randomly in the lanes until we reach target density
    while (carsSpawned < targetCars) {
        // Randomly choose a lane (0-11: 0-5 vertical, 6-11 horizontal)
        int lane = rand() % 12;
        int x, y;
        Direction dir;
       
        if (lane < 3) {
            // Vertical lanes going DOWN (left side of center)
            x = centerX - numLanes - laneSpace + lane;
            y = rand() % height;
            dir = Direction::DOWN;
        } else if (lane < 6) {
            // Vertical lanes going UP (right side of center)
            x = centerX + (lane - 3);
            y = rand() % height;
            dir = Direction::UP;
        } else if (lane < 9) {
            // Horizontal lanes going RIGHT (top side of center)
            x = rand() % width;
            y = centerY - numLanes - laneSpace + (lane - 6);
            dir = Direction::RIGHT;
        } else {
            // Horizontal lanes going LEFT (bottom side of center)
            x = rand() % width;
            y = centerY + (lane - 9);
            dir = Direction::LEFT;
        }
       
        // Check if cell is valid and empty
        if (x >= 0 && x < width && y >= 0 && y < height &&
            !cells[y][x].hasCar() && !cells[y][x].hasTrafficLight()) {
            int vel = rand() % (maxVelocity + 1);
            cells[y][x].setCarVelocity(vel);
            cells[y][x].setCarDirection(dir);
            cells[y][x].setCarId(nextCarId++);
            carsSpawned++;
        }
        
        Turn t;
        t.direction = Direction::UP;
        cells[50][50].setTurn(t);
    }
}

void Grid::setupCrossroadLights(int redDur, int yellowDur, int greenDur) {
    int numLanes = 3;
    int laneSpace = 1;
    int centerX = width / 2;
    int centerY = height / 2;
   
    // Traffic lights for vertical lanes (going DOWN) - place just before intersection
    for (int lane = 0; lane < numLanes; lane++) {
        int x = centerX - numLanes - laneSpace + lane;
        int y = centerY - numLanes - laneSpace - 1; // Adjusted for space
       
        if (x >= 0 && x < width && y >= 0 && y < height) {
            TrafficLight tl;
            tl.state = TrafficLight::GREEN; // Vertical starts GREEN
            tl.redDuration = redDur;
            tl.yellowDuration = yellowDur;
            tl.greenDuration = greenDur;
            tl.timer = 0;
            cells[y][x].setTrafficLight(tl);
        }
    }
   
    // Traffic lights for vertical lanes (going UP) - place just before intersection
    for (int lane = 0; lane < numLanes; lane++) {
        int x = centerX + lane;
        int y = centerY + numLanes; // Unchanged, as space is above
       
        if (x >= 0 && x < width && y >= 0 && y < height) {
            TrafficLight tl;
            tl.state = TrafficLight::GREEN; // Vertical starts GREEN
            tl.redDuration = redDur;
            tl.yellowDuration = yellowDur;
            tl.greenDuration = greenDur;
            tl.timer = 0;
            cells[y][x].setTrafficLight(tl);
        }
    }

    // Traffic lights for horizontal lanes (going RIGHT) - place just before intersection
    for (int lane = 0; lane < numLanes; lane++) {
        int y = centerY - numLanes - laneSpace + lane;
        int x = centerX - numLanes - laneSpace - 1; // Adjusted for space
       
        if (x >= 0 && x < width && y >= 0 && y < height) {
            TrafficLight tl;
            tl.state = TrafficLight::RED; // Horizontal starts RED
            tl.redDuration = redDur;
            tl.yellowDuration = yellowDur;
            tl.greenDuration = greenDur;
            tl.timer = 0;
            cells[y][x].setTrafficLight(tl);
        }
    }
   
    // Traffic lights for horizontal lanes (going LEFT) - place just before intersection
    for (int lane = 0; lane < numLanes; lane++) {
        int y = centerY + lane;
        int x = centerX + numLanes;
       
        if (x >= 0 && x < width && y >= 0 && y < height) {
            TrafficLight tl;
            tl.state = TrafficLight::RED; // Horizontal starts RED
            tl.redDuration = redDur;
            tl.yellowDuration = yellowDur;
            tl.greenDuration = greenDur;
            tl.timer = 0;
            cells[y][x].setTrafficLight(tl);
        }
    }
}

void Grid::update(const Rules& rules, int vmax, double p) {
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