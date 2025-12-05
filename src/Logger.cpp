#include "Logger.hpp"
#include <fstream>

void Logger::logVehicle(int vehicleId, int step, int x, int y, int velocity, bool waiting) {
    vehicleLog.push_back({vehicleId, step, x, y, velocity, waiting});
}

void Logger::logStepMetrics(int step, int carsExited, double avgSpeed, int carsWaiting) {
    stepMetrics.push_back({step, carsExited, avgSpeed, carsWaiting});
}

void Logger::exportVehicleLog(const std::string& filename) const {
    std::ofstream file(filename);
    file << "vehicle_id,step,x,y,velocity,waiting\n";
    for (const auto& e : vehicleLog) {
        file << e.vehicleId << "," << e.step << ","
             << e.x << "," << e.y << ","
             << e.velocity << "," << e.waiting << "\n";
    }
}

void Logger::exportStepMetrics(const std::string& filename) const {
    std::ofstream file(filename);
    file << "step,carsExited,avgSpeed,carsWaiting\n";
    for (const auto& e : stepMetrics) {
        file << e.step << "," << e.carsExited << ","
             << e.avgSpeed << "," << e.carsWaiting << "\n";
    }
}
