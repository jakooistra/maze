//
//  MazeArguments.h
//  maze
//
//  Created by John Kooistra on 2023-04-20.
//

#ifndef MazeArguments_h
#define MazeArguments_h

#include <optional>
#include <set>
#include <string>
#include <vector>

#include "MazeType.h"

struct MazeArguments {
    int width = 25;
    int height = 25;
    int wallWidth = 1;
    int cellSize = 9;
    int count = 1;
    bool showAnalysis = false;
    bool showPath = false; // TODO: add command for this visual output argument
    std::optional<int> specifiedSeed;
    std::optional<std::string> baseFileName;
    std::vector<MazeType> types;  // TODO: add a command to modify this list
    std::set<int> rankedOutput;
    std::set<int> percentileOutput;
    
    static std::optional<MazeArguments> parse(int argc, const char * argv[]);
};

#endif /* MazeArguments_h */
