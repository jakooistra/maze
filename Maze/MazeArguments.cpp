//
//  MazeArguments.cpp
//  maze
//
//  Created by John Kooistra on 2023-04-20.
//

#include <algorithm>
#include <iostream>
#include <filesystem>
#include <map>
#include <memory>
#include <sstream>
#include <vector>

#include "CommandParser.h"
#include "MazeArguments.h"
#include "MazeGenerator.h"

std::optional<MazeArguments> MazeArguments::parse(int argc, const char * argv[]) {
    MazeArguments args;
    
    std::filesystem::path programPath(argv[0]);
    std::string programName = programPath.filename();
    
    std::map<std::string, std::vector<std::shared_ptr<MazeGenerator const>>> mazeTypeLists;
    mazeTypeLists["all"] = MazeGenerator::all();
    mazeTypeLists["complex"] = MazeGenerator::allOfQuality(MazeQuality::Complex);
    mazeTypeLists["trivial"] = MazeGenerator::allOfQuality(MazeQuality::Trivial);
    
    // Build all command line parameter definitions.
    auto parser = std::make_shared<CommandParser>();
    std::optional<CommandDefinitionBuilder> cmdBuilderType = (*parser)
        .add("t", "Add a maze type to generate.")
        .stringArgument("type")
        .setMultiple();
    for (auto generator : MazeGenerator::all()) {
        std::stringstream message;
        message
            << "'" << generator->type << "' "
            << generator->name << " - " << generator->description;
        cmdBuilderType->addMessage(message.str());
    }
    for (auto tuple : mazeTypeLists) {
        std::stringstream message;
        message << "'" << tuple.first << "' - (";
        for (auto generator : tuple.second) {
            if (generator != tuple.second.front()) {
                message << ", ";
            }
            message << generator->type;
        }
        message << ")";
        cmdBuilderType->addMessage(message.str());
    }
    auto cmdType = cmdBuilderType->build();
    auto cmdWidth = (*parser)
        .add("w", "The width of the maze.")
        .intArgument("width", args.width)
        .build();
    auto cmdHeight = (*parser)
        .add("h", "The height of the maze.")
        .intArgument("height", args.height)
        .build();
    auto cmdCount = (*parser)
        .add("n", "The number of each maze type to generate.")
        .intArgument("count", args.count)
        .build();
    auto cmdSeed = (*parser) // TODO: allow specification of multiple seeds?
        .add("s", "The seed used to generate the maze.")
        .intArgument("seed", args.specifiedSeed)
        .addMessage("Only valid when generating a single maze")
        .setUncommon()
        .build();
    auto cmdAnalyze = (*parser)
        .add("a", "Analyze the generated mazes and output statistics.")
        .build();
    auto cmdShowPath = (*parser)
        .add("p", "Show the shortest path solution in image output.")
        .setUncommon()
        .build();
    auto cmdWallWidth = (*parser)
        .add("ww", "The width of maze walls in pixels.")
        .intArgument("width", args.wallWidth)
        .setUncommon()
        .build();
    auto cmdCellSize = (*parser)
        .add("cs", "The size of maze cells in pixels.")
        .intArgument("size", args.cellSize)
        .setUncommon()
        .build();
    auto cmdBaseFileName = (*parser)
        .add("f", "Specifies the base file name to use when outputting maze images.")
        .stringArgument("file")
        .setUncommon()
        .addMessage("Defaults to using the maze type name.")
        .addMessage("Only valid when a single maze type is specified.")
        .build();
    auto cmdOutputRanked = (*parser)
        .add("o", "Creates an image of the nth best maze of the ones generated (1-n).")
        .intArgument("rank")
        .setOptional()
        .setMultiple()
        .addMessage("If 'rank' is unspecified, outputs the best maze (1).")
        .build();
    auto cmdOutputPercentile = (*parser)
        .add("op", "Creates an image of the best maze in the given percentile (1-100).")
        .intArgument("percentile")
        .setUncommon()
        .setMultiple()
        .build();
    auto cmdOnlyUsage = (*parser)
        .add("usage", "Ignores other commands, and outputs detailed usage notes.")
        .setUncommon()
        .build();
    auto cmdPerformance = (*parser)
        .add("perf", "Measures maze generation speed for all specified types.")
        .setUncommon()
        .build();
    auto cmdCollate = (*parser)
        .add("collate", "When outputting maze images, output all types in one wide image.")
        .setUncommon()
        .build();
    
    // Print usage and exit early if the input is invalid.
    auto parserResult = parser->parse(argc, argv);
    if (!parserResult.valid) {
        parser->printUsage(programName);
        return std::nullopt;
    }
    
    bool onlyPrintUsage = false;
    for (auto command : parserResult.commands) {
        if (command.name == cmdType.name) {
            auto generator = MazeGenerator::get(command.value->string);
            auto list = mazeTypeLists.find(command.value->string);
            if (generator) {
                args.types.insert(generator->type);
            } else if (list != mazeTypeLists.end()) {
                for (auto generator : list->second) {
                    args.types.insert(generator->type);
                }
            } else {
                std::cout << "ERROR: '" << command.value->str() << "' is not a valid maze type." << std::endl;
                parser->printUsage(programName);
                return std::nullopt;
            }
        } else if (command.name == cmdWidth.name) {
            args.width = std::max(1, command.value->integer);
        } else if (command.name == cmdHeight.name) {
            args.height = std::max(1, command.value->integer);
        } else if (command.name == cmdCount.name) {
            args.count = command.value->integer;
        } else if (command.name == cmdShowPath.name) {
            args.showPath = true;
        } else if (command.name == cmdWallWidth.name) {
            args.wallWidth = std::max(1, command.value->integer);
        } else if (command.name == cmdCellSize.name) {
            args.cellSize = std::max(1, command.value->integer);
        } else if (command.name == cmdBaseFileName.name) {
            args.baseFileName = command.value->string;
        } else if (command.name == cmdSeed.name) {
            args.specifiedSeed = command.value->integer;
        } else if (command.name == cmdAnalyze.name) {
            args.showAnalysis = true;
        } else if (command.name == cmdOutputRanked.name) {
            args.rankedOutput.insert(command.has_value() ? command.value->integer : 1);
        } else if (command.name == cmdOutputPercentile.name) {
            args.percentileOutput.insert(command.has_value() ? command.value->integer : 1);
        } else if (command.name == cmdOnlyUsage.name) {
            onlyPrintUsage = true;
        } else if (command.name == cmdPerformance.name) {
            args.measurePerformance = true;
        } else if (command.name == cmdCollate.name) {
            args.collateAllMazeTypeImages = true;
        }
    }
    
    if (onlyPrintUsage) {
        parser->printUsage(programName, true);
        return std::nullopt;
    }
    
    // Massage valid data and prune invalid specified data.
    // Include percentile output in ranked output.
    for (auto percentile : args.percentileOutput) {
        if (percentile == 1) {
            args.rankedOutput.insert(1);
        } else {
            args.rankedOutput.insert((percentile * args.count + 99) / 100);
        }
    }
    // If a base file name was specified, but no output rank is specified,
    // ensure the best maze will have its image created.
    if (args.baseFileName.has_value() && args.rankedOutput.empty()) {
        args.rankedOutput.insert(1);
    }
    // Remove invalid ranked output specifications.
    while (!args.rankedOutput.empty() && *args.rankedOutput.begin() < 1) {
        args.rankedOutput.erase(args.rankedOutput.begin());
        args.rankedOutput.insert(1);
    }
    while (!args.rankedOutput.empty() && *args.rankedOutput.rbegin() > args.count) {
        args.rankedOutput.erase(*args.rankedOutput.rbegin());
        args.rankedOutput.insert(args.count - 1);
    }
    
    // Validate combinations of commands.
    std::stringstream invalidMessage;
    if (args.types.empty()) {
        invalidMessage << "expected at least one maze type to be specified.";
    } else if (args.baseFileName.has_value() && args.types.size() > 1) {
        invalidMessage << "-f option is not valid when specifying multiple (" << args.types.size() << ") maze types.";
    } else if (args.specifiedSeed.has_value() && args.count > 1) {
        invalidMessage << "-s option is not valid when maze count (" << args.count << ") is more than 1.";
    }
    auto invalidMessageString = invalidMessage.str();
    if (!invalidMessageString.empty()) {
        std::cout << "ERROR: " << invalidMessageString << std::endl;
        parser->printUsage(programName);
        return std::nullopt;
    }
    
    return args;
}

void MazeArguments::printWarnings() {
    if (rankedOutput.empty() && !measurePerformance && !showAnalysis) {
        std::cout << "No output specified." << std::endl;
        std::cout << "  Use -o or -op to specify which maze(s) to output." << std::endl;
        if (types.size() <= 1) {
            std::cout << "  Use -f to specify a file name only the best maze is desired." << std::endl;
        }
    }
    if (collateAllMazeTypeImages && types.size() == 1) {
        std::cout << "Collate was specified but only one generator type was specified." << std::endl;
    }
}
