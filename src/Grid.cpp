#include "Grid.hpp"
#include <cstdlib>
#include <algorithm>
#include <numeric>

Grid::Grid(int w, int h) : width(w), height(h) {
    cells.resize(height, std::vector<Cell>(width));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            cells[y][x] = Cell(-1);  // Empty
        }
    }
}

void Grid::initializeRandom(double density, int vmax) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (static_cast<double>(rand()) / RAND_MAX < density) {
                // Random initial velocity 0 to vmax
                cells[y][x].setVelocity(rand() % (vmax + 1));
            } else {
                cells[y][x].setVelocity(-1);
            }
        }
    }
}

void Grid::update(const Rules& rules, int vmax, double p) {
    std::vector<std::vector<Cell>> next(height, std::vector<Cell>(width, Cell(-1)));

    for (int y = 0; y < height; y++) {  // For each lane (independent for now)
        for (int x = 0; x < width; x++) {
            int currentVel = cells[y][x].getVelocity();
            if (currentVel < 0) continue;  // Empty

            int dist = distanceToNextCar(x, y);
            int newVel = rules.nextVelocity(currentVel, dist, vmax, p);

            // 4. Movement: Move forward newVel cells (toroidal)
            int newX = (x + newVel) % width; // Wrap around (toroidal)
            next[y][newX].setVelocity(newVel);
        }
    }

    cells = next;
}

int Grid::distanceToNextCar(int x, int y) const {
    int dist = 1;
    while (dist <= width) {  // Max dist = width (empty road)
        int nx = (x + dist) % width;
        if (cells[y][nx].hasCar()) {
            return dist;
        }
        dist++;
    }
    return width;  // No car ahead (full loop)
}

void Grid::exportPPM(const std::string& filename, int scale, int vmax) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file) return;

    file << "P6\n" << width * scale << " " << height * scale << "\n255\n";

    for (int py = 0; py < height * scale; py++) {
        for (int px = 0; px < width * scale; px++) {
            int cellX = px / scale;
            int cellY = py / scale;

            int vel = cells[cellY][cellX].getVelocity();
            unsigned char r, g, b;
            if (vel < 0) {  // Empty: white
                r = g = b = 255;
            } else {  // Car: color by velocity (blue=slow, green=mid, red=fast)
                int hue = static_cast<int>(255.0 * vel / (vmax == 0 ? 1 : vmax));
                r = hue >= 128 ? hue : 0;
                g = hue >= 128 ? (255 - hue) * 2 : hue * 2;
                b = hue < 128 ? (127 - hue) * 2 : 0;
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
    return carCount > 0 ? static_cast<double>(totalVel) / carCount : 0.0;
}