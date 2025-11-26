#include "Grid.hpp"
#include <cstdlib>
#include <algorithm>
#include <numeric>
#include <array>
#include <cmath>
#include <fstream>

inline std::array<unsigned char, 3> idColormap(int id) {
    if (id < 0) return {0, 0, 0};

    // Knuth multiplicative hash
    unsigned int h = (id * 2654435761u) % 360;

    double s = 0.85;
    double v = 0.90;

    double C = v * s;
    double hh = h / 60.0;   // sector index
    double X = C * (1 - std::fabs(std::fmod(hh, 2.0) - 1.0));
    double m = v - C;

    double r, g, b;

    if      (hh < 1) { r = C; g = X; b = 0; }
    else if (hh < 2) { r = X; g = C; b = 0; }
    else if (hh < 3) { r = 0; g = C; b = X; }
    else if (hh < 4) { r = 0; g = X; b = C; }
    else if (hh < 5) { r = X; g = 0; b = C; }
    else             { r = C; g = 0; b = X; }

    return {
        (unsigned char)((r + m) * 255),
        (unsigned char)((g + m) * 255),
        (unsigned char)((b + m) * 255)
    };
}

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
                cells[y][x].setVelocity(rand() % (vmax + 1));
                cells[y][x].setCarId(nextCarId++);  // ID assigned ONCE
            } else {
                cells[y][x].setVelocity(-1); // empty
            }
        }
    }
}

void Grid::update(const Rules& rules, int vmax, double p) {
    std::vector<std::vector<Cell>> next(height, std::vector<Cell>(width, Cell(-1)));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            int currentVel = cells[y][x].getVelocity();
            if (currentVel < 0) continue; // empty

            int dist = distanceToNextCar(x, y);
            int newVel = rules.nextVelocity(currentVel, dist, vmax, p);

            int newX = (x + newVel) % width;

            next[y][newX].setVelocity(newVel);
            next[y][newX].setCarId(cells[y][x].getCarId());
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
            if (id < 0) {
                r = g = b = 0;
            } else {
                auto [rr, gg, bb] = idColormap(id);  
                r = rr; g = gg; b = bb;
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
            int v = cells[y][x].getVelocity();
            if (v >= 0) {
                totalVel += v;
                carCount++;
            }
        }
    }
    return carCount > 0 ? (double)totalVel / carCount : 0.0;
}
