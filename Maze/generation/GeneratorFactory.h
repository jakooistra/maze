//
//  GeneratorFactory.h
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#ifndef GeneratorFactory_h
#define GeneratorFactory_h

#include <vector>

#include "MazeGenerator.h"
#include "MazeType.h"

namespace GeneratorFactory {
    std::unique_ptr<MazeGenerator> create(MazeType type);

    MazeType defaultType();
    std::vector<MazeType> allTypes();
    std::vector<MazeType> trivialTypes();
    std::vector<MazeType> complexTypes();
}

#endif /* GeneratorFactory_h */
