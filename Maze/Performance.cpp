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

namespace {
    struct PerformanceChart {
        std::string name;
        std::string suffix;
        std::function<double(GenerationRate)> value;
    };

    // Simplifies the cell count display.
    // For instance, 999 is close enough to 1000, 4002 is close enough to 4000.
    // Simply display 1000 or 4000 and not the precise number.
    static int simplifiedCellCount(int cellCount) {
        return ((cellCount + 50) / 100) * 100;
    }
}

GenerationRate GenerationRate::measure(std::string const &type, int width, int height, double minimumTimeSeconds) {
    GenerationRate rate;
    rate.cellCount = width * height;
    auto generator = MazeGenerator::get(type);
    if (generator) {
        bool finishedGenerating = false;
        std::mutex generationLock;
        generationLock.lock();
        std::thread generatorThread([generator, width, height, &rate, &finishedGenerating, &generationLock](){
            int count = 0;
            auto startTime = std::chrono::high_resolution_clock::now();
            while (!finishedGenerating || count < 3) {
                generator->generate(width, height, count++);
            }
            auto endTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> timeDelta = endTime - startTime;
            rate.seconds = timeDelta.count();
            rate.count = count;
            generationLock.unlock();
        });
        
        // Wait for the specified minimum number of seconds, then signal that generation
        // should finish and acquire the generation lock to wait for the thread to finish.
        std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000 * minimumTimeSeconds)));
        finishedGenerating = true;
        generationLock.lock();
        if (generatorThread.joinable()) {
            generatorThread.join();
        }
    }
    return rate;
}

Performance Performance::measure(std::string const &type) {
    Performance perf;
    perf.type = type;
    double timeScale = 0.05;
    perf.measuredRates.push_back(GenerationRate::measure(type, 32, 32, 1 * timeScale)); // ~1000
    perf.measuredRates.push_back(GenerationRate::measure(type, 45, 45, 2 * timeScale)); // ~2000
    perf.measuredRates.push_back(GenerationRate::measure(type, 64, 63, 4 * timeScale)); // ~4000
    perf.measuredRates.push_back(GenerationRate::measure(type, 90, 89, 8 * timeScale)); // ~8000
    return perf;
}

std::vector<Performance> Performance::measure(std::set<std::string> const &types) {
    std::cout << "Measuring performance..." << std::endl;
    std::vector<Performance> list;
    for (auto type : types) {
        auto generator = MazeGenerator::get(type);
        std::cout << "  Measuring " << generator->name << " performance..." << std::endl;
        list.push_back(measure(type));
    }
    return list;
}

void Performance::print(const std::set<std::string> &types) {
    auto list = measure(types);
    
    int rateWidth = 9;
    int nameWidth = 5;
    for (auto perf : list) {
        nameWidth = std::max(nameWidth, (int)MazeGenerator::get(perf.type)->name.length() + 1);
    }
    
    double minimumPerSecond = list.front().measuredRates.front().perSecond();
    int maximumCells = 0;
    for (auto perf : list) {
        for (auto rate : perf.measuredRates) {
            minimumPerSecond = std::min(minimumPerSecond, rate.perSecond());
            maximumCells = std::max(maximumCells, rate.cellCount);
        }
    }
    if (minimumPerSecond < 0.00001) {
        minimumPerSecond = 1.0;
    }
    double smallestPerCellCount = minimumPerSecond * (double)maximumCells;
    
    // TODO: hone in on what charts are useful.
    // TODO: have a variant of performance chart that shows performance normalized per maze generator type.
    PerformanceChart perSecond = { "Mazes generated per second:", "", [](auto rate){
        return rate.perSecond();
    } };
    PerformanceChart perCellCount = { "Relative rate per cell count:", "%", [smallestPerCellCount](auto rate){
        return 100.0 * (rate.perSecond() * (double)rate.cellCount) / smallestPerCellCount;
    } };
    PerformanceChart timePerCell = { "Time per cell (picoseconds):", "", [smallestPerCellCount](auto rate){
        return 1000000000.0 * (rate.seconds / (double)rate.count) / (double)rate.cellCount;
    } };
    std::vector<PerformanceChart> charts = {
        perSecond,
        perCellCount,
        timePerCell,
    };
    
    for (auto chart : charts) {
        std::cout << std::endl << chart.name << std::endl;
        std::cout << std::setw(nameWidth) << std::right << "cell count:";
        for (auto rate : list.front().measuredRates) {
            std::cout << std::setw(rateWidth) << std::right << simplifiedCellCount(rate.cellCount);
        }
        std::cout << std::endl;
        
        for (auto perf : list) {
            auto generator = MazeGenerator::get(perf.type);
            
            std::cout << std::setw(nameWidth) << std::left << generator->name;
            for (auto rate : perf.measuredRates) {
                std::cout << std::setw(rateWidth - (int)chart.suffix.length()) << std::right << (int)(0.5 + chart.value(rate)) << chart.suffix;
            }
            std::cout << std::endl;
        }
    }
}
