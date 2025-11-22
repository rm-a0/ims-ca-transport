#include "Grid.hpp"

Grid::Grid(int w, int h) : width(w), height(h) {
    cells.resize(height, std::vector<Cell>(width));

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            cells[y][x] = Cell(false);
        }
    }
}

void Grid::update(const Rules& rules) {
    std::vector<std::vector<Cell>> next = cells;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            bool cellState = cells[y][x].isAlive();
            int aliveNeighbors = countAliveNeighbors(x, y);

            next[y][x].setAlive(rules.nextState(cellState, aliveNeighbors));
        }
    }

    cells = next;
}

int Grid::countAliveNeighbors(int x, int y) const {
    int count = 0;
    for (int i = x - 1; i <= x + 1; ++i) {
        for (int j = y - 1; j <= y + 1; ++j) {
            if (i == x && j == y) continue;

            int nx = (i + width) % width;
            int ny = (j + height) % height;

            if (cells[ny][nx].isAlive()) {
                ++count;
            }
        }
    }
    return count;
}

void Grid::exportPPM(const std::string& filename, int scale) const {
    std::ofstream file(filename, std::ios::binary);
    if (!file) return;

    file << "P6\n" << width * scale << " " << height * scale << "\n255\n";

    for (int y = 0; y < height * scale; ++y) {
        for (int x = 0; x < width * scale; ++x) {

            int cellX = x / scale;
            int cellY = y / scale;

            bool alive = cells[cellY][cellX].isAlive();

            unsigned char r = alive ? 0   : 255;
            unsigned char g = alive ? 255 : 0;
            unsigned char b = 0;

            file.put(r);
            file.put(g);
            file.put(b);
        }
    }
}
