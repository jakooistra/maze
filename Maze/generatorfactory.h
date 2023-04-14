//
//  generatorfactory.h
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#ifndef generatorfactory_h
#define generatorfactory_h

#include "MazeGenerator.h"
#include "MazeType.h"

std::unique_ptr<MazeGenerator> createGenerator(MazeType type);

#endif /* generatorfactory_h */
