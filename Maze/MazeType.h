//
//  MazeType.h
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#ifndef MazeType_h
#define MazeType_h

#include <string>

enum class MazeType {
    RemoveRandomWalls,
    VerticalPathBreaks,
};

inline std::string getMazeTypeName(MazeType type) {
    switch (type) {
        case MazeType::RemoveRandomWalls:
            return "Remove Random Walls";
        case MazeType::VerticalPathBreaks:
            return "Vertical Path Breaks";
    }
    return "Unknown";
}

#endif /* MazeType_h */
