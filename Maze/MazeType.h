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
};

inline std::string getMazeTypeName(MazeType type) {
    switch (type) {
        case MazeType::RemoveRandomWalls:
            return "Remove Random Walls";
    }
    return "Unknown";
}

#endif /* MazeType_h */
