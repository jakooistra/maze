//
//  GeneratorFactory.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#include "GeneratorFactory.h"
#include "RemoveRandomWallsGenerator.h"
#include "VerticalPathBreaksGenerator.h"

std::unique_ptr<MazeGenerator> createGenerator(MazeType type) {
    switch (type) {
        case MazeType::RemoveRandomWalls:
            return std::make_unique<RemoveRandomWallsGenerator>();
        case MazeType::VerticalPathBreaks:
            return std::make_unique<VerticalPathBreaksGenerator>();
    }
    return nullptr;
}
