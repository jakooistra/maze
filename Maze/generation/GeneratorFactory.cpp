//
//  GeneratorFactory.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#include <unordered_map>
#include <vector>

#include "ChainBurstGenerator.h"
#include "FiftyFiftyGenerator.h"
#include "GeneratorFactory.h"
#include "RemoveRandomWallsGenerator.h"
#include "VerticalPathBreaksGenerator.h"

std::shared_ptr<MazeGenerator const> GeneratorFactory::get(std::string const &type) {
    static std::unordered_map<std::string,std::shared_ptr<MazeGenerator const>> s_cachedTypeMap;
    if (s_cachedTypeMap.empty()) {
        for (auto generator : all()) {
            s_cachedTypeMap[generator->getType()] = generator;
        }
    }
    return s_cachedTypeMap[type];
}

std::vector<std::shared_ptr<MazeGenerator const>> GeneratorFactory::all() {
    // TODO: make a better way to collect these, if it can be made automatic (static? compile time?) that would be great.
    static std::vector<std::shared_ptr<MazeGenerator const>> s_allGenerators = {
        std::make_shared<ChainBurstGenerator>(),
        std::make_shared<FiftyFiftyGenerator>(),
        std::make_shared<RemoveRandomWallsGenerator>(),
        std::make_shared<VerticalPathBreaksGenerator>(),
    };
    return s_allGenerators;
}

std::vector<std::shared_ptr<MazeGenerator const>> GeneratorFactory::allOfQuality(MazeQuality quality) {
    static std::unordered_map<MazeQuality,std::vector<std::shared_ptr<MazeGenerator const>>> s_cachedQualityList;
    auto &list = s_cachedQualityList[quality];
    if (list.empty()) {
        for (auto generator : all()) {
            if (generator->getQuality() == quality) {
                list.push_back(generator);
            }
        }
    }
    return list;
}
