//
//  GeneratorFactory.h
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#ifndef GeneratorFactory_h
#define GeneratorFactory_h

#include <string>
#include <vector>

#include "MazeGenerator.h"

namespace GeneratorFactory {
    std::shared_ptr<MazeGenerator const> get(std::string const &type);

    std::vector<std::shared_ptr<MazeGenerator const>> all();
    std::vector<std::shared_ptr<MazeGenerator const>> allOfQuality(MazeQuality quality);

    inline std::vector<std::shared_ptr<MazeGenerator const>> complexTypes() { return allOfQuality(MazeQuality::Complex); }
    inline std::vector<std::shared_ptr<MazeGenerator const>> trivialTypes() { return allOfQuality(MazeQuality::Trivial); }
}

#endif /* GeneratorFactory_h */
