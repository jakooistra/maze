//
//  generatorfactory.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#include "generatorfactory.h"
#include "RemoveRandomWallsGenerator.h"

std::unique_ptr<MazeGenerator> createGenerator(MazeType type) {
    switch (type) {
        case MazeType::RemoveRandomWalls:
            return std::make_unique<RemoveRandomWallsGenerator>();
    }
    return nullptr;
}
