#include "Grid.hpp"
#include <cstdlib>
#include <fstream>
#include "Utils.hpp"

Grid::Grid(int w, int h) : width(w), height(h) {
    cells.resize(height, std::vector<Cell>(width));
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            cells[y][x] = Cell(-1); // empty
}

void Grid::initializeRandom(double density, int vmax) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if ((double)rand() / RAND_MAX < density) {
                cells[y][x].setCarVelocity(rand() % (vmax + 1));
                cells[y][x].setCarId(nextCarId++);
            } else {
                cells[y][x].removeCar();
            }
        }
    }
}

void Grid::initializeLeftToRight(int vmax, double spawnProbability) {
    // Clear the grid first
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            cells[y][x].removeCar();
        }
    }
    
    // Spawn cars from the leftmost column with some probability
    for (int y = 0; y < height; y++) {
        if ((double)rand() / RAND_MAX < spawnProbability) {
            cells[y][0].setCarVelocity(rand() % (vmax + 1));
            cells[y][0].setCarId(nextCarId++);
        }
    }
}

void Grid::addTrafficLight(int x, int y, int redDur, int yellowDur, int greenDur) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        TrafficLight tl;
        tl.state = TrafficLight::GREEN; // Start with green
        tl.redDuration = redDur;
        tl.yellowDuration = yellowDur;
        tl.greenDuration = greenDur;
        tl.timer = 0;
        cells[y][x].setTrafficLight(tl);
    }
}

void Grid::updateTrafficLights() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            cells[y][x].updateTrafficLight();
        }
    }
}

bool Grid::spawnCarAtLeft(int y, int velocity) {
    if (y < 0 || y >= height)
        return false;
    
    if (cells[y][0].hasCar())
        return false; // Cell already occupied
    
    cells[y][0].setCarVelocity(velocity);
    cells[y][0].setCarId(nextCarId++);
    return true;
}

void Grid::update(const Rules& rules, int vmax, double p) {
    std::vector<std::vector<Cell>> next(height, std::vector<Cell>(width, Cell(-1)));
    
    // Copy traffic lights to next state
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (cells[y][x].hasTrafficLight()) {
                next[y][x].setTrafficLight(*cells[y][x].tl);
            }
        }
    }
    
    // Update cars
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (!cells[y][x].hasCar()) continue;
            
            int currentVel = cells[y][x].getCarVelocity();
            int dist = distanceToNextCar(x, y);
            int newVel = rules.nextVelocity(currentVel, dist, vmax, p);
            int newX = (x + newVel) % width;
            
            cells[y][x].moveCarTo(next[y][newX]);
            next[y][newX].setCarVelocity(newVel);
        }
    }
    
    cells = std::move(next);
}

int Grid::distanceToNextCar(int x, int y) const {
    int dist = 1;
    while (dist <= width) {
        int nx = (x + dist) % width;
        
        // Check for traffic light
        if (cells[y][nx].hasTrafficLight()) {
            if (cells[y][nx].getTrafficLightState() == TrafficLight::RED) {
                return dist; // Treat red light as obstacle
            }
        }
        
        // Check for car
        if (cells[y][nx].hasCar()) {
            return dist;
        }
        
        dist++;
    }
    return width;
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
                auto [rr, gg, bb] = Utils::idColormap(id);  
                r = rr; g = gg; b = bb;
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