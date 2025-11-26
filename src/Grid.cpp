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

void Grid::update(const Rules& rules, int vmax, double p) {
    std::vector<std::vector<Cell>> next(height, std::vector<Cell>(width, Cell(-1)));

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
        if (cells[y][nx].hasCar())
            return dist;
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
            
            int id  = cells[cellY][cellX].getCarId();
            unsigned char r, g, b;
            auto [rr, gg, bb] = Utils::idColormap(id);  
            r = rr; g = gg; b = bb;

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
