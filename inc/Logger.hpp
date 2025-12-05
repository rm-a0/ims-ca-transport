#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <vector>
#include <string>

struct VehicleLogEntry {
    int vehicleId;
    int step;
    int x;
    int y;
    int velocity;
    bool waiting;
};

struct StepMetrics {
    int step;
    int carsExited;
    double avgSpeed;
    int carsWaiting;
};

class Logger {
public:
    Logger() = default;

    // Log individual vehicle info at a given step
    void logVehicle(int vehicleId, int step, int x, int y, int velocity, bool waiting);

    // Log aggregated metrics for a step
    void logStepMetrics(int step, int carsExited, double avgSpeed, int carsWaiting);

    // Export logs to CSV
    void exportVehicleLog(const std::string& filename) const;
    void exportStepMetrics(const std::string& filename) const;

private:
    std::vector<VehicleLogEntry> vehicleLog;
    std::vector<StepMetrics> stepMetrics;
};

#endif // LOGGER_HPP
