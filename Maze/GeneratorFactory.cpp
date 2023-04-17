//
//  GeneratorFactory.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#include "ChainBurstGenerator.h"
#include "FiftyFiftyGenerator.h"
#include "GeneratorFactory.h"
#include "RemoveRandomWallsGenerator.h"
#include "VerticalPathBreaksGenerator.h"

std::unique_ptr<MazeGenerator> GeneratorFactory::create(MazeType type) {
    switch (type) {
        case MazeType::ChainBurst:
            return std::make_unique<ChainBurstGenerator>();
        case MazeType::FiftyFifty:
            return std::make_unique<FiftyFiftyGenerator>();
        case MazeType::RemoveRandomWalls:
            return std::make_unique<RemoveRandomWallsGenerator>();
        case MazeType::VerticalPathBreaks:
            return std::make_unique<VerticalPathBreaksGenerator>();
    }
    return nullptr;
}

std::vector<MazeType> GeneratorFactory::allTypes() {
    return {
        MazeType::ChainBurst,
        MazeType::FiftyFifty,
        MazeType::RemoveRandomWalls,
        MazeType::VerticalPathBreaks,
    };
}

std::vector<MazeType> GeneratorFactory::trivialTypes() {
    return {
        MazeType::FiftyFifty,
        MazeType::RemoveRandomWalls,
        MazeType::VerticalPathBreaks,
    };
}

std::vector<MazeType> GeneratorFactory::complexTypes() {
    return {
        MazeType::ChainBurst,
    };
}
