//
//  generator.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#include "generator.h"

std::unique_ptr<Maze> generate(int width, int height, int seed) {
    auto maze = std::make_unique<Maze>(width, height);
    
    (*maze)[{0, -1}].topWall = false;
    (*maze)[{0, 0}].topWall = false;
    (*maze)[{0, 1}].topWall = false;
    (*maze)[{2, 2}].leftWall = false;
    (*maze)[{1, 1}].leftWall = false;
    (*maze)[{1, 2}].topWall = false;
    
    (*maze)[{2, 2}].topWall = false;
    (*maze)[{2, 1}].topWall = false;
    (*maze)[{2, 0}].leftWall = false;
    
    srand(seed);
    for (int i = 0; i < width * height / 2; ++i) {
        (*maze)[{rand() % width, rand() % height}].topWall = false;
        (*maze)[{rand() % width, rand() % height}].leftWall = false;
    }
    maze->setStart({rand() % width, rand() % height});
    maze->setFinish({rand() % width, rand() % height});
    
    return maze;
}
