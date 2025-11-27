#include "Grid.hpp"
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include "Utils.hpp"

Grid::Grid(int w, int h) : width(w), height(h) {
    cells.resize(height, std::vector<Cell>(width));
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            cells[y][x] = Cell(); // empty
}

void Grid::spawnCars(int velocity) {
    // Upper half: leftmost side, direction right
    for (int y = 0; y < height / 2; y++) {
        int spawnX = 0;
        if (cells[y][spawnX].hasCar())
            continue;
       
        cells[y][spawnX].setCarVelocity(velocity);
        cells[y][spawnX].setCarDirection(Direction::RIGHT);
        cells[y][spawnX].setCarId(nextCarId++);
    }
   
    // Lower half: rightmost side, direction left
    for (int y = height / 2; y < height; y++) {
        int spawnX = width - 1;
        if (cells[y][spawnX].hasCar())
            continue;
       
        cells[y][spawnX].setCarVelocity(velocity);
        cells[y][spawnX].setCarDirection(Direction::LEFT);
        cells[y][spawnX].setCarId(nextCarId++);
    }
}

void Grid::addTrafficLight(int x, int y, int redDur, int yellowDur, int greenDur) {
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

void Grid::updateTrafficLights() {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            cells[y][x].updateTrafficLight();
        }
    }
}

void Grid::update(const Rules& rules, int vmax, double p) {
    std::vector<std::vector<Cell>> next(height, std::vector<Cell>(width, Cell()));
    
    // Copy traffic lights to next state
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (cells[y][x].hasTrafficLight()) {
                next[y][x].setTrafficLight(*cells[y][x].getTrafficLight());
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
            
            // Apply Nagel-Schreidemacher rules
            int newVel = rules.nextVelocity(currentVel, dist, vmax, p);
            
            // Clamp velocity to available space
            if (dist > 0) {
                newVel = std::min(newVel, dist - 1);
            }
            newVel = std::max(0, newVel);
        
            // Calculate new position
            int newX = x;
            int newY = y;
            
            if (dx != 0) {
                newX = (x + newVel * dx % width + width) % width;
            }
            if (dy != 0) {
                newY = (y + newVel * dy % height + height) % height;
            }
            
            moves.push_back({x, y, newX, newY, newVel, cells[y][x].getCarId(), dir});
        }
    }
    
    // Second pass: Apply moves, checking for conflicts
    for (const auto& move : moves) {
        // Check if destination is already occupied in next grid
        if (next[move.newY][move.newX].hasCar()) {
            // Collision detected! Keep car at current position with velocity 0
            cells[move.oldY][move.oldX].moveCarTo(next[move.oldY][move.oldX]);
            next[move.oldY][move.oldX].setCarVelocity(0);
        } else {
            // Safe to move
            cells[move.oldY][move.oldX].moveCarTo(next[move.newY][move.newX]);
            next[move.newY][move.newX].setCarVelocity(move.newVel);
        }
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