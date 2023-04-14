//
//  RemoveRandomWallsGenerator.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#include "RemoveRandomWallsGenerator.h"

std::unique_ptr<Maze> RemoveRandomWallsGenerator::generate(int width, int height, int seed) {
    auto maze = std::make_unique<Maze>(width, height);
    
    srand(seed);
    maze->setStart({rand() % width, rand() % height});
    maze->setFinish({rand() % width, rand() % height});
    while (maze->getStart() == maze->getFinish()) {
        maze->setFinish({rand() % width, rand() % height});
    }
    
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            bool leftWall = (rand() % 2) == 1;
            bool topWall = (rand() % 2) == 1;
            maze->setCell(x, y, Cell(leftWall, topWall));
        }
    }
    
    for (int i = 0; i < (width * height * 10) / 10; ++i) {
        maze->setCell(rand() % width,
                      rand() % height,
                      Cell((rand() % 2) == 1,
                           (rand() % 2) == 1));
    }
    
    return maze;
}
