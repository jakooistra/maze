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
#include "Maze.h"
#include "MazeArguments.h"
#include "MazeGenerator.h"
#include "MazeToImage.h"
#include "Performance.h"
#include "Stats.h"
#include "ThreadUtility.h"
#include "Utility.h"

static void writeMazeImageFile(std::string const &fileName, Image const *image) {
    std::vector<unsigned char> rgba;
    image->encodeRGBA(rgba);
    auto error = lodepng::encode(fileName, &rgba[0], image->width, image->height);
    if (error != 0) {
        std::cout << "ERROR: writing \"" << fileName << "\"" << std::endl;
        std::cout << "ERROR: " << lodepng_error_text(error) << std::endl;
    }
}

int main(int argc, const char * argv[]) {
    std::optional<MazeArguments> args = MazeArguments::parse(argc, argv);
    if (!args) {
        return 0;
    }
    
    // If processing takes a long time, it's good to output warnings before generating mazes.
    args->printWarnings();
    
    if (args->rankedOutput.size() > 0 || args->showAnalysis || !args->specifiedSeeds.empty()) {
        if (args->count > 1) {
            std::cout << "Running on " << ThreadPool::shared().getThreadCount() << " threads." << std::endl;
        }
        
        if (args->types.size() > 1) {
            std::cout << "Generating " << args->types.size() << " maze types x" << args->count << "..." <<std::endl;
        }
        int typeCount = 0;
        std::map<int, std::map<std::string, std::vector<std::shared_ptr<Image>>>> rankedNamedImages;
        for (auto type : args->types) {
            auto generator = MazeGenerator::get(type);
            auto generatorTypeName = generator->name;
            typeCount++;
            if (args->types.size() > 1) {
                std::cout << "  (" << typeCount << "/" << args->types.size() << ") ";
            }
            if (args->specifiedSeeds.size() == 1) {
                std::cout << "Generating " << generatorTypeName << " " << args->width << "x" << args->height << " from seed " << *args->specifiedSeeds.begin() << "..." << std::endl;
            } else if (!args->specifiedSeeds.empty()) {
                std::cout << "Generating x" << args->specifiedSeeds.size() << " seeds of " << generatorTypeName << " " << args->width << "x" << args->height << "..." << std::endl;
            } else {
                std::cout << "Generating x" << args->count << " " << generatorTypeName << " " << args->width << "x" << args->height << "..." << std::endl;
            }
            
            std::function<GeneratedMaze(int)> generate = [args, generator](int seed){
                return generator->generate(args->width, args->height, seed);
            };
            
            // Mazes in order from worst to best.
            std::set<FullAssessment> sortedMazes;
            
            std::vector<int> seedsToGenerate = consecutiveNumbers(0, args->count-1);
            seedsToGenerate.reserve(args->specifiedSeeds.size() + seedsToGenerate.size());
            for (auto seed : args->specifiedSeeds) {
                seedsToGenerate.push_back(seed);
            }
            std::vector<GeneratedMaze> mazes = threadedTransform(seedsToGenerate, generate, "Generate");
            
            if (args->showAnalysis) {
                std::cout << "Analyzing x" << args->count << " " << generatorTypeName << "..." << std::endl;
            }
            Stats stats;
            std::function<FullAssessment(GeneratedMaze const &)> assessmentFunction = valueOfMaze;
            std::vector<FullAssessment> assessments = threadedTransform(mazes, assessmentFunction, "Analyze");
            for (auto assessment : assessments) {
                sortedMazes.insert(assessment);
                stats.accumulate(assessment.analysis.get(), assessment.maze.seed);
            }
            if (args->showAnalysis) {
                stats.print(generatorTypeName + " statistics");
            }
            
            if (!args->rankedOutput.empty() || !args->specifiedSeeds.empty()) {
                std::vector<XY> const emptyPath;
                std::stringstream fileNamePrefixStream;
                if (args->collateAllMazeTypeImages && !args->baseFileName.has_value()) {
                    fileNamePrefixStream << "All";
                } else {
                    if (args->baseFileName.has_value()) {
                        fileNamePrefixStream << *args->baseFileName;
                    } else {
                        fileNamePrefixStream << generatorTypeName;
                    }
                    fileNamePrefixStream << " " << args->width << "x" << args->height;
                }
                std::string fileNamePrefix = fileNamePrefixStream.str();
                int rank = 0;
                for (auto iter = sortedMazes.rbegin(); iter != sortedMazes.rend(); ++iter) {
                    rank++;
                    bool isSpecifiedSeed = args->specifiedSeeds.contains(iter->maze.seed);
                    if (args->rankedOutput.contains(rank) || isSpecifiedSeed) {
                        std::stringstream fileName;
                        fileName << fileNamePrefix;
                        if (isSpecifiedSeed) {
                            fileName << " (seed " << iter->maze.seed << ")";
                        } else if (!args->baseFileName.has_value() || args->rankedOutput.size() > 1) {
                            fileName << " (" << rank << " of " << args->count << ")";
                        }
                        fileName << ".png";
                        
                        auto path = args->showPath ? iter->analysis->shortestPath : emptyPath;
                        auto image = convertToImage(iter->maze.maze.get(), args->sizes, path);
                        rankedNamedImages[rank][fileName.str()].push_back(std::move(image));
                    }
                }
            }
        }
        
        for (auto rankTuple : rankedNamedImages) {
            for (auto nameTuple : rankTuple.second) {
                auto const &images = nameTuple.second;
                if (images.size() == 1) {
                    writeMazeImageFile(nameTuple.first, images.front().get());
                } else {
                    Image collatedImage;
                    for (auto image : images) {
                        collatedImage.width += image->width;
                        collatedImage.height = std::max(collatedImage.height, image->height);
                    }
                    collatedImage.width -= ((int)images.size() - 1) * args->sizes.border;
                    // TODO: border color white and maybe a border around all mazes for clarity
                    RGBA borderColor = RGBA::gray(255);
                    collatedImage.pixels.resize(collatedImage.width * collatedImage.height, borderColor);
                    int x = 0;
                    for (auto image : images) {
                        collatedImage.blit(image.get(), x, 0);
                        x += image->width - args->sizes.border;
                    }
                    writeMazeImageFile(nameTuple.first, &collatedImage);
                }
            }
        }
    }
    
    if (args->measurePerformance && args->types.size() > 0) {
        Performance::print(args->types);
    }
    
    return 0;
}
