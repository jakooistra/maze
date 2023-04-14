//
//  VerticalPathBreaksGenerator.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#include "VerticalPathBreaksGenerator.h"

std::unique_ptr<Maze> VerticalPathBreaksGenerator::generate(int width, int height, int seed) {
    auto maze = std::make_unique<Maze>(width, height);
    
    srand(seed);
    maze->setStart({0, rand() % height});
    maze->setFinish({width-1, rand() % height});
    while (maze->getStart() == maze->getFinish()) {
        maze->setFinish({width-1, rand() % height});
    }
    
    for (int x = 0; x < width; ++x) {
        int yHole = rand() % height;
        for (int y = 1; y < height; ++y) {
            maze->setCell(x, y, Cell(y != yHole, false));
        }
    }
    
    return maze;
}
