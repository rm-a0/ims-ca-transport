#include "Grid.hpp"

Grid::Grid(int w, int h) : width(w), height(h) {}

void Grid::update(const Rules& rules) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            bool cellState = cells[x][y].isAlive();
            int aliveNeighbors = countAliveNeighbors(x, y);
            cells[y][x].setAlive(rules.nextState(cellState, aliveNeighbors));
        }
    }
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
