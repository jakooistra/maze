//
//  MazeArguments.cpp
//  maze
//
//  Created by John Kooistra on 2023-04-20.
//

#include <iostream>
#include <filesystem>
#include <memory>
#include <sstream>

#include "CommandParser.h"
#include "MazeArguments.h"

std::optional<MazeArguments> MazeArguments::parse(int argc, const char * argv[]) {
    MazeArguments args;
    
    std::filesystem::path programPath(argv[0]);
    std::string programName = programPath.filename();
    
    // Build the command line parser object.
    auto parser = std::make_shared<CommandParser>();
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
    auto cmdSeed = (*parser)
        .add("s", "The seed used to generate the maze.")
        .intArgument("seed", args.specifiedSeed)
        .addMessage("Only valid when generating a single maze")
        .build();
    auto cmdAnalyze = (*parser)
        .add("a", "Analyze the generated mazes and output statistics.")
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
        .stringArgument("rank")
        .setOptional()
        .setUncommon()
        .addMessage("If 'rank' is unspecified, outputs the best maze (1).")
        .build();
    auto cmdOutputPercentile = (*parser)
        .add("op", "Creates an image of the best maze in the given percentile (1-100).")
        .intArgument("percentile")
        .setUncommon()
        .build();
    
    // Print usage and exit early if the input is invalid.
    auto parserResult = parser->parse(argc, argv);
    if (!parserResult.valid) {
        parser->printUsage(programName);
        return std::nullopt;
    }
    
    for (auto command : parserResult.commands) {
        if (command.name == cmdWidth.name) {
            args.width = std::max(1, command.value->integer);
        } else if (command.name == cmdHeight.name) {
            args.height = std::max(1, command.value->integer);
        } else if (command.name == cmdCount.name) {
            args.count = command.value->integer;
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
        }
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
