//
//  GeneratorFactory.h
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#ifndef GeneratorFactory_h
#define GeneratorFactory_h

#include "MazeGenerator.h"
#include "MazeType.h"

std::unique_ptr<MazeGenerator> createGenerator(MazeType type);

#endif /* GeneratorFactory_h */
