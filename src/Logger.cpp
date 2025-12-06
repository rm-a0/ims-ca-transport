/**
 * @file Logger.cpp
 * @authors Michal Repcik (xrepcim00)
 */
#include "Logger.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <iomanip>
#include <cmath>
#include <algorithm>

void Logger::logTimestep(const TimestepMetrics& metrics) {
    timestepData.push_back(metrics);
}

void Logger::logVehicleSpawn(int vehicleId, int step, Direction spawnDir, bool willTurn) {
    VehicleTrajectory traj;
    traj.vehicleId = vehicleId;
    traj.spawnStep = step;
    traj.exitStep = -1;
    traj.totalSteps = 0;
    traj.stepsAtZeroVelocity = 0;
    traj.totalDistance = 0;
    traj.avgVelocity = 0.0;
    traj.spawnDirection = spawnDir;
    traj.didTurn = willTurn;
    traj.maxVelocity = 0;
    
    vehicleData[vehicleId] = traj;
}

void Logger::logVehicleExit(int vehicleId, int step) {
    if (vehicleData.find(vehicleId) != vehicleData.end()) {
        vehicleData[vehicleId].exitStep = step;
        vehicleData[vehicleId].totalSteps = step - vehicleData[vehicleId].spawnStep;
    }
}

void Logger::logVehicleState(int vehicleId, int step, int x, int y, int velocity) {
    if (vehicleData.find(vehicleId) == vehicleData.end()) {
        return;
    }
    
    VehicleTrajectory& traj = vehicleData[vehicleId];
    
    // Update statistics
    if (velocity == 0) {
        traj.stepsAtZeroVelocity++;
    }
    
    traj.totalDistance += velocity;
    traj.maxVelocity = std::max(traj.maxVelocity, velocity);
    
    // Update average velocity (incremental)
    int currentSteps = step - traj.spawnStep + 1;
    traj.avgVelocity = ((traj.avgVelocity * (currentSteps - 1)) + velocity) / currentSteps;
}

void Logger::logSpatialData(int x, int y, int velocity) {
    auto key = std::make_pair(x, y);
    
    if (spatialData.find(key) == spatialData.end()) {
        SpatialData data;
        data.x = x;
        data.y = y;
        data.totalVelocity = 0;
        data.observations = 0;
        data.avgVelocity = 0.0;
        spatialData[key] = data;
    }
    
    spatialData[key].totalVelocity += velocity;
    spatialData[key].observations++;
}

void Logger::logDirectionMetrics(const DirectionMetrics& metrics) {
    directionData[metrics.dir] = metrics;
}

void Logger::finalizeData() {
    // Compute final averages for spatial data
    for (auto& [key, data] : spatialData) {
        if (data.observations > 0) {
            data.avgVelocity = static_cast<double>(data.totalVelocity) / data.observations;
        }
    }
    
    // Finalize vehicle trajectories
    for (auto& [id, traj] : vehicleData) {
        // If vehicle is still in system, use current step
        if (traj.exitStep == -1 && !timestepData.empty()) {
            traj.totalSteps = timestepData.back().step - traj.spawnStep;
        }
    }
}

void Logger::exportTimestepMetrics(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }
    
    // Header
    file << "step,totalCars,carsEntered,carsExited,avgVelocity,"
         << "avgVelNorth,avgVelSouth,avgVelEast,avgVelWest,"
         << "stoppedCars,maxQueueN,maxQueueS,maxQueueE,maxQueueW,"
         << "carsAtRedLight,avgTimeInSystem,avgWaitTime\n";
    
    // Data
    for (const auto& metrics : timestepData) {
        file << metrics.step << ","
             << metrics.totalCarsInSystem << ","
             << metrics.carsEntered << ","
             << metrics.carsExited << ","
             << std::fixed << std::setprecision(4) << metrics.avgVelocity << ","
             << metrics.avgVelocityNorth << ","
             << metrics.avgVelocitySouth << ","
             << metrics.avgVelocityEast << ","
             << metrics.avgVelocityWest << ","
             << metrics.carsAtZeroVelocity << ","
             << metrics.maxQueueNorth << ","
             << metrics.maxQueueSouth << ","
             << metrics.maxQueueEast << ","
             << metrics.maxQueueWest << ","
             << metrics.carsWaitingAtRedLight << ","
             << metrics.avgTimeInSystem << ","
             << metrics.avgWaitingTime << "\n";
    }
    
    file.close();
    std::cout << "Exported timestep metrics to: " << filename << std::endl;
}

void Logger::exportVehicleTrajectories(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }
    
    // Header
    file << "vehicleId,spawnStep,exitStep,totalSteps,stepsAtZero,"
         << "totalDistance,avgVelocity,spawnDirection,didTurn,maxVelocity\n";
    
    // Data
    for (const auto& [id, traj] : vehicleData) {
        file << traj.vehicleId << ","
             << traj.spawnStep << ","
             << traj.exitStep << ","
             << traj.totalSteps << ","
             << traj.stepsAtZeroVelocity << ","
             << traj.totalDistance << ","
             << std::fixed << std::setprecision(4) << traj.avgVelocity << ","
             << directionToString(traj.spawnDirection) << ","
             << (traj.didTurn ? 1 : 0) << ","
             << traj.maxVelocity << "\n";
    }
    
    file.close();
    std::cout << "Exported vehicle trajectories to: " << filename << std::endl;
}

void Logger::exportSpatialHeatmap(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }
    
    // Header
    file << "x,y,totalVelocity,observations,avgVelocity\n";
    
    // Data
    for (const auto& [key, data] : spatialData) {
        file << data.x << ","
             << data.y << ","
             << data.totalVelocity << ","
             << data.observations << ","
             << std::fixed << std::setprecision(4) << data.avgVelocity << "\n";
    }
    
    file.close();
    std::cout << "Exported spatial heatmap to: " << filename << std::endl;
}

void Logger::exportDirectionMetrics(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }
    
    // Header
    file << "direction,totalVehicles,vehiclesExited,avgSpeed,"
         << "maxQueue,avgWaitTime,throughputRate\n";
    
    // Data
    for (const auto& [dir, metrics] : directionData) {
        file << directionToString(dir) << ","
             << metrics.totalVehicles << ","
             << metrics.vehiclesExited << ","
             << std::fixed << std::setprecision(4) << metrics.avgSpeed << ","
             << metrics.maxQueue << ","
             << metrics.avgWaitTime << ","
             << metrics.throughputRate << "\n";
    }
    
    file.close();
    std::cout << "Exported direction metrics to: " << filename << std::endl;
}

void Logger::exportSummaryStatistics(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }
    
    if (timestepData.empty()) {
        std::cerr << "Warning: No data to export" << std::endl;
        return;
    }
    
    // Compute summary statistics
    int totalSteps = timestepData.size();
    int totalCarsSpawned = timestepData.back().carsEntered;
    int totalCarsExited = timestepData.back().carsExited;
    
    // Average metrics over all timesteps
    double avgVelocityOverall = 0.0;
    double avgStoppedCars = 0.0;
    int maxQueueOverall = 0;
    
    for (const auto& m : timestepData) {
        avgVelocityOverall += m.avgVelocity;
        avgStoppedCars += m.carsAtZeroVelocity;
        maxQueueOverall = std::max({maxQueueOverall, m.maxQueueNorth, 
                                    m.maxQueueSouth, m.maxQueueEast, m.maxQueueWest});
    }
    avgVelocityOverall /= totalSteps;
    avgStoppedCars /= totalSteps;
    
    // Vehicle-based statistics
    double avgTimeInSystem = 0.0;
    double avgWaitingTime = 0.0;
    int completedVehicles = 0;
    
    for (const auto& [id, traj] : vehicleData) {
        if (traj.exitStep > 0) {  // Only count vehicles that exited
            avgTimeInSystem += traj.totalSteps;
            avgWaitingTime += traj.stepsAtZeroVelocity;
            completedVehicles++;
        }
    }
    
    if (completedVehicles > 0) {
        avgTimeInSystem /= completedVehicles;
        avgWaitingTime /= completedVehicles;
    }
    
    // Throughput (vehicles per minute)
    double throughput = (totalCarsExited * 60.0) / totalSteps;
    
    // Header
    file << "metric,value\n";
    
    // Export
    file << "totalSteps," << totalSteps << "\n"
         << "totalCarsSpawned," << totalCarsSpawned << "\n"
         << "totalCarsExited," << totalCarsExited << "\n"
         << "completionRate," << std::fixed << std::setprecision(4) 
         << (totalCarsSpawned > 0 ? (double)totalCarsExited/totalCarsSpawned : 0.0) << "\n"
         << "avgVelocity," << avgVelocityOverall << "\n"
         << "avgStoppedCars," << avgStoppedCars << "\n"
         << "maxQueueLength," << maxQueueOverall << "\n"
         << "avgTimeInSystem," << avgTimeInSystem << "\n"
         << "avgWaitingTime," << avgWaitingTime << "\n"
         << "throughputPerMinute," << throughput << "\n";
    
    file.close();
    std::cout << "Exported summary statistics to: " << filename << std::endl;
}

void Logger::exportAll(const std::string& directory) const {
    // Create directory if it doesn't exist
    std::filesystem::create_directories(directory);
    
    // Export all data files
    exportTimestepMetrics(directory + "/timestep_metrics.csv");
    exportVehicleTrajectories(directory + "/vehicle_trajectories.csv");
    exportSpatialHeatmap(directory + "/spatial_heatmap.csv");
    exportDirectionMetrics(directory + "/direction_metrics.csv");
    exportSummaryStatistics(directory + "/summary_statistics.csv");
}

void Logger::reset() {
    timestepData.clear();
    vehicleData.clear();
    spatialData.clear();
    directionData.clear();
}

std::string Logger::directionToString(Direction dir) const {
    switch (dir) {
        case Direction::LEFT:  return "WEST";
        case Direction::RIGHT: return "EAST";
        case Direction::UP:    return "NORTH";
        case Direction::DOWN:  return "SOUTH";
        default:               return "UNKNOWN";
    }
}