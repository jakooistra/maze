//
//  VerticalPathBreaksGenerator.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#include <random>

#include "VerticalPathBreaksGenerator.h"

std::unique_ptr<Maze> VerticalPathBreaksGenerator::generate(int width, int height, int seed) {
    auto maze = std::make_unique<Maze>(width, height);
    std::mt19937 rng(seed);
    
    maze->setStart({0, (int)(rng() % height)});
    maze->setFinish({width-1, (int)(rng() % height)});
    
    for (int x = 0; x < width; ++x) {
        int yHole = rng() % height;
        for (int y = 0; y < height; ++y) {
            maze->setCell(x, y, Cell(y != yHole, false));
        }
    }
    
    return maze;
}
