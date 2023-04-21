//
//  GeneratedMaze.h
//  Maze
//
//  Created by John Kooistra on 2023-04-16.
//

#ifndef GeneratedMaze_h
#define GeneratedMaze_h

#include <memory>

#include "Maze.h"
#include "MazeType.h"

struct GeneratedMaze {
    std::string typeArgument;
    int seed;
    std::shared_ptr<Maze const> maze;
};

#endif /* GeneratedMaze_h */
