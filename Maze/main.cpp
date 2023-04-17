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
#include "CommandParser.h"
#include "GeneratorFactory.h"
#include "Maze.h"
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
    
    // Build the command line parser object.
    CommandParser parser;
    CommandDefinition cmdWidth = parser.add("w", CommandArgument::Int, "width", "The width of the maze.");
    CommandDefinition cmdHeight = parser.add("h", CommandArgument::Int, "height", "The height of the maze.");
    CommandDefinition cmdCount = parser.add("n", CommandArgument::Int, "count", "The number of each maze type to generate.");
    CommandDefinition cmdSeed = parser.add("s", CommandArgument::Int, "seed", "The seed used to generate the maze. Only valid when generating a single maze.");
    CommandDefinition cmdAnalyze = parser.add("a", "Analyze the generated mazes and output statistics.");
    CommandDefinition cmdWallWidth = parser.add("ww", CommandArgument::Int, "wall-width", "The width of maze walls in pixels.");
    CommandDefinition cmdCellSize = parser.add("cs", CommandArgument::Int, "cell-size", "The size of maze cells in pixels.");
    CommandDefinition cmdBaseFileName = parser.add("f", CommandArgument::String, "base-file-name", "Specifies the base file name to use when outputting maze images. Defaults to using the maze type name. Only valid when a single maze type is specified.");
    CommandDefinition cmdOutputRanked = parser.add("o", CommandArgument::OptionalInt, "output-rank", "Creates an image of the nth best maze of the ones generated (1-n). Defaults to best maze (1).");
    CommandDefinition cmdOutputPercentile = parser.add("op", CommandArgument::Int, "output-percentile", "Creates an image of the best maze in the given percentile (1-100).");
    
    // Print usage and exit early if the input is invalid.
    auto parserResult = parser.parse(argc, argv);
    if (!parserResult.valid) {
        parser.printUsage();
        return 0;
    }
    
    int width = 25;
    int height = 25;
    int wallWidth = 1;
    int cellSize = 9;
    int count = 1;
    bool showAnalysis = false;
    bool showPath = false; // TODO: add command for this visual output argument
    std::optional<int> specifiedSeed;
    std::optional<std::string> baseFileName;
    std::vector<MazeType> types = { GeneratorFactory::defaultType() };  // TODO: add a command to modify this list
    std::set<int> rankedOutput;
    std::set<int> percentileOutput;
    for (auto command : parserResult.commands) {
        if (command.name == cmdWidth.name) {
            width = std::max(1, command.intValue);
        } else if (command.name == cmdHeight.name) {
            height = std::max(1, command.intValue);
        } else if (command.name == cmdCount.name) {
            count = command.intValue;
        } else if (command.name == cmdWallWidth.name) {
            wallWidth = std::max(1, command.intValue);
        } else if (command.name == cmdCellSize.name) {
            cellSize = std::max(1, command.intValue);
        } else if (command.name == cmdBaseFileName.name) {
            baseFileName = command.stringValue;
        } else if (command.name == cmdSeed.name) {
            specifiedSeed = command.intValue;
        } else if (command.name == cmdAnalyze.name) {
            showAnalysis = true;
        } else if (command.name == cmdOutputRanked.name) {
            rankedOutput.insert(command.hasValue ? command.intValue : 1);
        } else if (command.name == cmdOutputPercentile.name) {
            percentileOutput.insert(command.hasValue ? command.intValue : 1);
        }
    }
    
    // Massage valid data and prune invalid specified data.
    // Include percentile output in ranked output.
    for (auto percentile : percentileOutput) {
        if (percentile == 1) {
            rankedOutput.insert(1);
        } else {
            rankedOutput.insert((percentile * count + 99) / 100);
        }
    }
    // If a base file name was specified, but no output rank is specified,
    // ensure the best maze will have its image created.
    if (baseFileName.has_value() && rankedOutput.empty()) {
        rankedOutput.insert(1);
    }
    
    // Validate combinations of commands.
    if (baseFileName.has_value() && types.size() > 1) {
        std::cout << "-f option is not valid when specifying multiple (" << types.size() << ") maze types." << std::endl;
        parser.printUsage();
        return 0;
    } else if (specifiedSeed.has_value() && count > 1) {
        std::cout << "-s option is not valid when maze count (" << count << ") is more than 1." << std::endl;
        parser.printUsage();
        return 0;
    }
    
    if (count > 1) {
        std::cout << "Running on " << ThreadPool::shared().getThreadCount() << " threads." << std::endl;
    }
    
    // TODO: is there a better way to express this? assessValue as a function has the same signature, can I not just pass it directly?
    std::function<FullAssessment(GeneratedMaze)> assess = [](GeneratedMaze maze){
        return assessValue(maze);
    };
    
    if (types.size() > 1) {
        std::cout << "Generating " << types.size() << " maze types x" << count << "..." <<std::endl;
    }
    int typeCount = 0;
    for (auto type : types) {
        auto generator = GeneratorFactory::create(type);
        auto typeName = getMazeTypeName(generator->getType());
        typeCount++;
        if (types.size() > 1) {
            std::cout << "  (" << typeCount << "/" << types.size() << ") ";
        }
        std::cout << "Generating x" << count << " " << typeName << "..." << std::endl;
        
        std::function<GeneratedMaze(int)> generate = [width, height, &generator](int seed){
            return generator->generate(width, height, seed);
        };
        
        // Mazes in order from worst to best.
        std::set<FullAssessment> sortedMazes;
        
        std::vector<int> seedsToGenerate;
        if (specifiedSeed.has_value() && count <= 1) {
            seedsToGenerate.push_back(specifiedSeed.value());
        } else {
            seedsToGenerate = consecutiveNumbers(0, count-1);
        }
        std::vector<GeneratedMaze> mazes = threadedTransform(seedsToGenerate, generate);
        
        Stats stats;
        std::vector<FullAssessment> assessments = threadedTransform(mazes, assess);
        for (auto assessment : assessments) {
            sortedMazes.insert(assessment);
            stats.accumulate(assessment.analysis.get(), assessment.maze.seed);
        }
        if (showAnalysis) {
            stats.print(typeName + " statistics");
        }
        
        if (!rankedOutput.empty()) {
            int rank = 1;
            std::vector<XY> const emptyPath;
            std::string fileNamePrefix = baseFileName.has_value() ? *baseFileName : typeName;
            for (auto iter = sortedMazes.rbegin(); iter != sortedMazes.rend(); ++iter) {
                if (rankedOutput.contains(rank)) {
                    std::stringstream fileName;
                    fileName << fileNamePrefix;
                    if (!baseFileName.has_value() || rankedOutput.size() > 1) {
                        fileName << " (" << rank << " of " << count << ")";
                    }
                    fileName << ".png";
                    writeMazeImageFile(fileName.str(),
                                       iter->maze.maze.get(),
                                       showPath ? iter->analysis->shortestPath : emptyPath,
                                       wallWidth, cellSize);
                }
                rank++;
            }
        }
    }
    
    return 0;
}
