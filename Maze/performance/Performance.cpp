//
//  Performance.cpp
//  maze
//
//  Created by John Kooistra on 2023-04-21.
//

#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>

#include "MazeGenerator.h"
#include "Performance.h"

struct PerformanceChart {
    std::string name;
    std::string suffix;
    std::function<double(GenerationRate)> value;
};

static std::vector<XY> const s_performanceTestSizes = {
    { 32, 32 }, // ~1000
    { 45, 45 }, // ~2000
    { 64, 63 }, // ~4000
    { 90, 89 }, // ~8000
};
static int s_maximumTestCells = s_performanceTestSizes.back().area();

double Performance::mostCellsPerSecond() const {
    double most = 0.0;
    for (auto rate : measuredRates) {
        most = std::max(most, rate.cellsPerSecond());
    }
    return most;
}

Performance Performance::measure(std::string const &type, double seconds) {
    Performance perf;
    perf.type = type;
    
    int totalCells = 0;
    for (auto size : s_performanceTestSizes) {
        totalCells += size.area();
    }
    for (auto size : s_performanceTestSizes) {
        double time = seconds * (double)size.area() / (double)totalCells;
        perf.measuredRates.push_back(GenerationRate::measure(type, size.x, size.y, time));
    }
    
    return perf;
}

std::vector<Performance> Performance::measure(std::set<std::string> const &types, double secondsPerType) {
    std::cout << "Measuring performance..." << std::endl;
    std::vector<Performance> list;
    for (auto type : types) {
        auto generator = MazeGenerator::get(type);
        std::cout << "  Measuring " << generator->name << " performance..." << std::endl;
        list.push_back(measure(type, secondsPerType));
    }
    return list;
}

void Performance::print(const std::set<std::string> &types, double secondsPerType) {
    auto list = measure(types, secondsPerType);
    
    int rateWidth = 9;
    int nameWidth = 5;
    for (auto perf : list) {
        nameWidth = std::max(nameWidth, (int)MazeGenerator::get(perf.type)->name.length() + 1);
    }
    
    PerformanceChart perSecond = { "Mazes generated per second:", "", [](auto rate){
        return rate.mazesPerSecond();
    } };
    double largestCellsPerSecond = list.front().mostCellsPerSecond();
    PerformanceChart cellNormalizedRate = { "Normalized rate per cell count:", "%", [&largestCellsPerSecond](auto rate){
        return 100.0 * rate.cellsPerSecond() / largestCellsPerSecond;
    } };
    std::vector<PerformanceChart> charts = {
        perSecond,
        cellNormalizedRate,
    };
    
    for (auto chart : charts) {
        std::cout << std::endl << chart.name << std::endl;
        std::cout << std::setw(nameWidth) << std::right << "cell count:";
        for (auto rate : list.front().measuredRates) {
            std::cout << std::setw(rateWidth) << std::right << rate.simplifiedCellCount();
        }
        std::cout << std::endl;
        
        for (auto perf : list) {
            largestCellsPerSecond = perf.mostCellsPerSecond();
            auto generator = MazeGenerator::get(perf.type);
            
            std::cout << std::setw(nameWidth) << std::left << generator->name;
            for (auto rate : perf.measuredRates) {
                std::cout << std::setw(rateWidth - (int)chart.suffix.length()) << std::right;
                double value = chart.value(rate);
                if (value < 9.5) {
                    std::cout << std::fixed << std::setprecision(1) <<  0.05 + value;
                } else {
                    std::cout << (int)(0.5 + value);
                }
                std::cout << chart.suffix;
            }
            std::cout << std::endl;
        }
    }
}
