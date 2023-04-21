//
//  FiftyFiftyGenerator.cpp
//  maze
//
//  Created by John Kooistra on 2023-04-16.
//

#include <random>

#include "FiftyFiftyGenerator.h"
#include "Utility.h"

std::unique_ptr<Maze> FiftyFiftyGenerator::generateInternal(int width, int height, int seed) const {
    auto maze = std::make_unique<Maze>(width, height);
    auto rng = std::bind(std::uniform_int_distribution<>(0, INT_MAX), std::mt19937(seed));
    
    setRandomStartAndFinish(maze.get(), width, height, rng);
    
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            bool leftWall = (rng() % 2) == 1;
            bool topWall = (rng() % 2) == 1;
            maze->setCell(x, y, Cell(leftWall, topWall));
        }
    }
    
    return maze;
}

REGISTER_AND_STORE_GENERATOR(FiftyFiftyGenerator);
