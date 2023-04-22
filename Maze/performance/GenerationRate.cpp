//
//  GenerationRate.cpp
//  maze
//
//  Created by John Kooistra on 2023-04-21.
//

#include <chrono>
#include <mutex>
#include <thread>

#include "GenerationRate.h"
#include "MazeGenerator.h"

double GenerationRate::mazesPerSecond() const {
    return seconds >= 0 ? (double)count / seconds : 0.0;
}

double GenerationRate::cellsPerSecond() const {
    return seconds >= 0 ? (double)(cellCount * count) / seconds : 0.0;
}

int GenerationRate::simplifiedCellCount() {
    return ((cellCount + 50) / 100) * 100;
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
