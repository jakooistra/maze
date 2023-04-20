//
//  MazeTypeParsing.h
//  Maze
//
//  Created by John Kooistra on 2023-04-20.
//

#ifndef MazeTypeParsing_h
#define MazeTypeParsing_h

#include <optional>

#include "MazeType.h"

inline std::string getMazeArgumentName(MazeType type) {
    switch (type) {
        case MazeType::ChainBurst:
            return "cb";
        case MazeType::FiftyFifty:
            return "ff";
        case MazeType::RemoveRandomWalls:
            return "rr";
        case MazeType::VerticalPathBreaks:
            return "vp";
    }
    return "Unknown";
}

inline std::optional<MazeType> getMazeTypeFromArgument(std::string const &argument) {
    for (auto type : allMazeTypes()) {
        if (argument == getMazeArgumentName(type)) {
            return type;
        }
    }
    return std::nullopt;
}

#endif /* MazeTypeParsing_h */
