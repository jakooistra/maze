//
//  RemoveRandomWallsGenerator.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#include <random>

#include "RemoveRandomWallsGenerator.h"
#include "Utility.h"

std::unique_ptr<Maze> RemoveRandomWallsGenerator::generateInternal(int width, int height, int seed) {
    auto maze = std::make_unique<Maze>(width, height);
    auto rng = std::bind(std::uniform_int_distribution<>(0, INT_MAX), std::mt19937(seed));
    
    setRandomStartAndFinish(maze.get(), width, height, rng);
    
    int numWallsToRemove = (width * height * 3) / 2;
    for (int i = 0; i < numWallsToRemove; ++i) {
        XY pos { rng() % width, rng() % height };
        Cell cell = (*maze)[pos];
        if ((rng() % 2) == 1) {
            cell.topWall = false;
        } else {
            cell.leftWall = false;
        }
        maze->setCell(pos, cell);
    }
    
    return maze;
}
