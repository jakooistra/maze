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
    
    for (int i = 0; i < (width * height * 6) / 10; ++i) {
        (*maze)[{rand() % width, rand() % height}].topWall = false;
        (*maze)[{rand() % width, rand() % height}].leftWall = false;
    }
    
    return maze;
}
