//
//  RemoveRandomWallsGenerator.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#include <random>

#include "RemoveRandomWallsGenerator.h"
#include "Utility.h"

std::unique_ptr<Maze> RemoveRandomWallsGenerator::generate(int width, int height, int seed) {
    auto maze = std::make_unique<Maze>(width, height);
    std::mt19937 rng(seed);
    
    setRandomStartAndFinish(maze.get(), width, height, rng);
    
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            bool leftWall = (rng() % 2) == 1;
            bool topWall = (rng() % 2) == 1;
            maze->setCell(x, y, Cell(leftWall, topWall));
        }
    }
    
    for (int i = 0; i < (width * height * 10) / 10; ++i) {
        maze->setCell(rng() % width,
                      rng() % height,
                      Cell((rng() % 2) == 1,
                           (rng() % 2) == 1));
    }
    
    return maze;
}
