/**
 * @file Logger.hpp
 * @authors Michal Repcik (xrepcim00)
 * @brief Comprehensive data collection for traffic simulation analysis
 */
#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include "Cell.hpp"

/**
 * @brief Per-timestep aggregate metrics for the entire intersection
 */
struct TimestepMetrics {
    int step;
    
    // Throughput metrics
    int totalCarsInSystem;      // Total cars currently on grid
    int carsEntered;            // Cumulative cars that entered (spawned)
    int carsExited;             // Cumulative cars that left the grid
    
    // Speed metrics
    double avgVelocity;         // Average velocity across all cars
    double avgVelocityNorth;    // Average on northbound approach
    double avgVelocitySouth;    // Average on southbound approach
    double avgVelocityEast;     // Average on eastbound approach
    double avgVelocityWest;     // Average on westbound approach
    
    // Queue/congestion metrics
    int carsAtZeroVelocity;     // Number of stopped cars
    int maxQueueNorth;          // Longest consecutive stopped vehicles
    int maxQueueSouth;
    int maxQueueEast;
    int maxQueueWest;
    
    // Traffic light metrics
    int carsWaitingAtRedLight;  // Cars stopped at red lights
    
    // Delay metrics
    double avgTimeInSystem;     // Average time cars spent in simulation
    double avgWaitingTime;      // Average time spent at v=0
};

/**
 * @brief Per-vehicle trajectory data for detailed analysis
 */
struct VehicleTrajectory {
    int vehicleId;
    int spawnStep;              // When car entered
    int exitStep;               // When car left (-1 if still in system)
    int totalSteps;             // Time in system
    int stepsAtZeroVelocity;    // Time spent stopped
    int totalDistance;          // Cells traveled
    double avgVelocity;         // Average velocity during journey
    Direction spawnDirection;   // Which approach (EAST, WEST, NORTH, SOUTH)
    bool didTurn;               // Whether car turned at intersection
    int maxVelocity;            // Peak velocity achieved
};

/**
 * @brief Spatial heatmap data for congestion visualization
 */
struct SpatialData {
    int x, y;
    long long totalVelocity;    // Sum of all velocities at this cell
    int observations;           // Number of times a car was here
    double avgVelocity;         // Computed: totalVelocity / observations
};

/**
 * @brief Direction-specific flow metrics
 */
struct DirectionMetrics {
    Direction dir;
    int totalVehicles;          // Total vehicles on this approach
    int vehiclesExited;         // Vehicles that successfully crossed
    double avgSpeed;            // Average speed on this approach
    int maxQueue;               // Maximum queue length observed
    double avgWaitTime;         // Average waiting time for vehicles
    double throughputRate;      // Vehicles per minute
};

/**
 * @class Logger
 * @brief Collects and exports comprehensive traffic simulation data
 */
class Logger {
public:
    Logger() = default;

    // === Data Collection Methods ===
    
    /**
     * @brief Record metrics for current timestep
     */
    void logTimestep(const TimestepMetrics& metrics);
    
    /**
     * @brief Record vehicle entering the system
     */
    void logVehicleSpawn(int vehicleId, int step, Direction spawnDir, bool willTurn);
    
    /**
     * @brief Record vehicle exiting the system
     */
    void logVehicleExit(int vehicleId, int step);
    
    /**
     * @brief Update vehicle statistics during simulation
     */
    void logVehicleState(int vehicleId, int step, int x, int y, int velocity);
    
    /**
     * @brief Record spatial data for heatmap generation
     */
    void logSpatialData(int x, int y, int velocity);
    
    /**
     * @brief Record direction-specific metrics
     */
    void logDirectionMetrics(const DirectionMetrics& metrics);

    // === Data Export Methods ===
    
    /**
     * @brief Export timestep metrics to CSV
     * Columns: step, totalCars, carsEntered, carsExited, avgVelocity, 
     *          avgVelNorth, avgVelSouth, avgVelEast, avgVelWest,
     *          stoppedCars, maxQueueN, maxQueueS, maxQueueE, maxQueueW,
     *          carsAtRedLight, avgTimeInSystem, avgWaitTime
     */
    void exportTimestepMetrics(const std::string& filename) const;
    
    /**
     * @brief Export vehicle trajectory data to CSV
     * Columns: vehicleId, spawnStep, exitStep, totalSteps, stepsAtZero,
     *          totalDistance, avgVelocity, spawnDirection, didTurn, maxVelocity
     */
    void exportVehicleTrajectories(const std::string& filename) const;
    
    /**
     * @brief Export spatial heatmap data to CSV
     * Columns: x, y, totalVelocity, observations, avgVelocity
     */
    void exportSpatialHeatmap(const std::string& filename) const;
    
    /**
     * @brief Export direction-specific metrics to CSV
     * Columns: direction, totalVehicles, vehiclesExited, avgSpeed,
     *          maxQueue, avgWaitTime, throughputRate
     */
    void exportDirectionMetrics(const std::string& filename) const;
    
    /**
     * @brief Export summary statistics to CSV
     * Single row with key performance indicators (KPIs)
     */
    void exportSummaryStatistics(const std::string& filename) const;
    
    /**
     * @brief Export all data files to specified directory
     */
    void exportAll(const std::string& directory) const;

    // === Helper Methods ===
    
    /**
     * @brief Finalize vehicle trajectories (compute averages, etc.)
     * Call this after simulation completes
     */
    void finalizeData();
    
    /**
     * @brief Clear all collected data
     */
    void reset();

    /**
     * @brief Print table in stdout
     */
    void printSummaryTable() const;

    // Data storage
    std::vector<TimestepMetrics> timestepData;
    std::unordered_map<int, VehicleTrajectory> vehicleData;
    std::map<std::pair<int,int>, SpatialData> spatialData;
    std::map<Direction, DirectionMetrics> directionData;
    
    std::string directionToString(Direction dir) const;
private:
};

#endif // LOGGER_HPP