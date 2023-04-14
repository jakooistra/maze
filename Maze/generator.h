//
//  generator.h
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#ifndef generator_h
#define generator_h

#include <memory>

#include "maze.h"

std::unique_ptr<Maze> generate(int width, int height, int seed);

#endif /* generator_h */
