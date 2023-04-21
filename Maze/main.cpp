//
//  main.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#include <iostream>
#include <set>
#include <sstream>

#include "lodepng.h"

#include "Assessment.h"
#include "GeneratorFactory.h"
#include "Maze.h"
#include "MazeArguments.h"
#include "MazeToImage.h"
#include "Stats.h"
#include "ThreadUtility.h"
#include "Utility.h"

static void writeMazeImageFile(std::string const &fileName, Maze const *maze, std::vector<XY> const &path, int wallWidth, int cellSize) {
    auto image = convertToImage(maze, wallWidth, cellSize, path);
    std::vector<unsigned char> rgba;
    image->encodeRGBA(rgba);
    auto error = lodepng::encode(fileName, &rgba[0], image->width, image->height);
    if (error != 0) {
        std::cout << "ERROR: writing \"" << fileName << "\"" << std::endl;
        std::cout << "ERROR: " << lodepng_error_text(error) << std::endl;
    }
}

int main(int argc, const char * argv[]) {
    // TODO: output of maze collections (grid? best to worst? multiple maze types?)
    std::optional<MazeArguments> args = MazeArguments::parse(argc, argv);
    if (!args) {
        return 0;
    }
    
    // If processing takes a long time, it's good to output warnings before generating mazes.
    args->printWarnings();
    
    if (args->count > 1) {
        std::cout << "Running on " << ThreadPool::shared().getThreadCount() << " threads." << std::endl;
    }
    
    if (args->generators.size() > 1) {
        std::cout << "Generating " << args->generators.size() << " maze types x" << args->count << "..." <<std::endl;
    }
    int typeCount = 0;
    for (auto generator : args->generators) {
        auto typeName = generator->getName();
        typeCount++;
        if (args->generators.size() > 1) {
            std::cout << "  (" << typeCount << "/" << args->generators.size() << ") ";
        }
        std::cout << "Generating x" << args->count << " " << typeName << " " << args->width << "x" << args->height << "..." << std::endl;
        
        std::function<GeneratedMaze(int)> generate = [args, generator](int seed){
            return generator->generate(args->width, args->height, seed);
        };
        
        // Mazes in order from worst to best.
        std::set<FullAssessment> sortedMazes;
        
        std::vector<int> seedsToGenerate;
        if (args->specifiedSeed.has_value() && args->count <= 1) {
            seedsToGenerate.push_back(args->specifiedSeed.value());
        } else {
            seedsToGenerate = consecutiveNumbers(0, args->count-1);
        }
        std::vector<GeneratedMaze> mazes = threadedTransform(seedsToGenerate, generate, "Generate");
        
        if (args->showAnalysis) {
            std::cout << "Analyzing x" << args->count << " " << typeName << "..." << std::endl;
        }
        Stats stats;
        std::function<FullAssessment(GeneratedMaze const &)> assessmentFunction = valueOfMaze;
        std::vector<FullAssessment> assessments = threadedTransform(mazes, assessmentFunction, "Analyze");
        for (auto assessment : assessments) {
            sortedMazes.insert(assessment);
            stats.accumulate(assessment.analysis.get(), assessment.maze.seed);
        }
        if (args->showAnalysis) {
            stats.print(typeName + " statistics");
        }
        
        if (!args->rankedOutput.empty()) {
            std::vector<XY> const emptyPath;
            std::string fileNamePrefix = args->baseFileName.has_value() ? *args->baseFileName : typeName;
            int rank = 0;
            for (auto iter = sortedMazes.rbegin(); iter != sortedMazes.rend(); ++iter) {
                rank++;
                if (args->rankedOutput.contains(rank)) {
                    std::stringstream fileName;
                    fileName << fileNamePrefix;
                    if (!args->baseFileName.has_value() || args->rankedOutput.size() > 1) {
                        fileName << " (" << rank << " of " << args->count << ")";
                    }
                    fileName << ".png";
                    writeMazeImageFile(fileName.str(),
                                       iter->maze.maze.get(),
                                       args->showPath ? iter->analysis->shortestPath : emptyPath,
                                       args->wallWidth, args->cellSize);
                }
            }
        }
    }
    
    return 0;
}
