//
//  main.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#include <chrono>
#include <iostream>
#include <set>

#include "lodepng.h"

#include "Assessment.h"
#include "GeneratorFactory.h"
#include "Maze.h"
#include "MazeToImage.h"
#include "Stats.h"
#include "ThreadUtility.h"
#include "Utility.h"

using namespace std::chrono;

static void writeMazeImageFile(std::string const &fileName, FullAssessment const &assessment, int wallWidth, int cellSize) {
    auto image = convertToImage(assessment.maze.maze.get(), wallWidth, cellSize,
                                assessment.analysis->shortestPath);
    std::vector<unsigned char> rgba;
    image->encodeRGBA(rgba);
    lodepng::encode(fileName, &rgba[0], image->width, image->height);
}

int main(int argc, const char * argv[]) {
    // TODO: use command line parameters to determine what to do
    // TODO: single maze analysis
    // TODO: output of maze collections
    // TODO: analysis of best mazes given heuristics
    // TODO: output with and without solution
    
    int wallWidth = 1;
    int cellSize = 9;
    
    int width = 16;
    int height = 16;
    int numMazesToGenerateForStats = 250;
    
    std::vector<MazeType> types = {
        MazeType::ChainBurst,
        MazeType::RemoveRandomWalls,
        MazeType::VerticalPathBreaks,
    };
    
    // TODO: is there a better way to express this? assessValue as a function has the same signature, can I not just pass it directly?
    std::function<FullAssessment(GeneratedMaze)> assess = [](GeneratedMaze maze){
        return assessValue(maze);
    };
    
    for (auto type : types) {
        auto generator = createGenerator(type);
        auto typeName = getMazeTypeName(generator->getType());
        std::cout << "Generating " << numMazesToGenerateForStats << " " << typeName << " mazes..." <<std::endl;
        
        std::function<GeneratedMaze(int)> generate = [width, height, &generator](int seed){
            return generator->generate(width, height, seed);
        };
        
        // Mazes in order from worst to best.
        std::set<FullAssessment> sortedMazes;
        
        auto startTime = high_resolution_clock::now();
        std::vector<int> seedsToGenerate = consecutiveNumbers(0, numMazesToGenerateForStats-1);
        std::vector<GeneratedMaze> mazes = threadedTransform(seedsToGenerate, generate);
        auto generateTime = high_resolution_clock::now();
        
        Stats stats;
        std::vector<FullAssessment> assessments = threadedTransform(mazes, assess);
        for (auto assessment : assessments) {
            sortedMazes.insert(assessment);
            stats.accumulate(assessment.analysis.get());
        }
        auto finishTime = high_resolution_clock::now();
        
        std::cout << "  Generation time: " << duration_cast<milliseconds>(generateTime - startTime).count() << "ms" << std::endl;
        std::cout << "  Assessment time: " << duration_cast<milliseconds>(finishTime - generateTime).count() << "ms" << std::endl;
        stats.print(typeName);
        
        if (!sortedMazes.empty()) {
            writeMazeImageFile(typeName + " (Best).png", *sortedMazes.rbegin(), wallWidth, cellSize);
            writeMazeImageFile(typeName + " (Worst).png", *sortedMazes.begin(), wallWidth, cellSize);
        }
    }
    
    return 0;
}
