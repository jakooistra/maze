//
//  MazeGenerator.cpp
//  maze
//
//  Created by John Kooistra on 2023-04-21.
//

#include <iostream>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "MazeGenerator.h"

struct MazeGeneratorRegistry {
    std::recursive_mutex mutex;
    std::map<std::string, std::shared_ptr<MazeGenerator const>> generatorByType;
    std::unordered_map<MazeQuality,std::set<std::string>> generatorTypeByQuality;
};

MazeGenerator::MazeGenerator(std::string const &type, std::string const &name, std::string const &description, MazeQuality quality)
:   type(type)
,   name(name)
,   description(description)
,   quality(quality)
{
}

GeneratedMaze MazeGenerator::generate(int width, int height, int seed) const {
    return {
        type,
        seed,
        generateInternal(width, height, seed),
    };
}

std::shared_ptr<MazeGenerator const> MazeGenerator::registerGenerator(std::shared_ptr<MazeGenerator const> generator) {
    auto registry = getRegistry();
    std::lock_guard guard(registry->mutex);
    
    auto type = generator->type;
    if (std::any_of(type.begin(), type.end(), [](auto ch) { return std::isspace(ch); })) {
        std::cerr << "ERROR: the type string must not contain spaces: '" << type << "', please fix the definnition for " << generator->name << std::endl;
        return nullptr;
    }
    
    //  Validate that the registry is unique. If it already exists, return the existing generator.
    auto generatorIter = registry->generatorByType.find(type);
    if (generatorIter != registry->generatorByType.end()) {
        std::cerr << "ERROR: the type " << type << " is being used by both " << generatorIter->second->name << " and " << generator->name << "." << std::endl;
        std::cerr << "NOTE: the generator named " << generatorIter->second->name << " will be used for the type " << type << "." << std::endl;
        return generatorIter->second;
    }
    
    registry->generatorByType[type] = generator;
    registry->generatorTypeByQuality[generator->quality].insert(type);
    return generator;
}

std::shared_ptr<MazeGenerator const> MazeGenerator::get(std::string const &type) {
    auto registry = getRegistry();
    std::lock_guard guard(registry->mutex);
    auto iter = registry->generatorByType.find(type);
    return iter == registry->generatorByType.end() ? nullptr : iter->second;
}

std::vector<std::shared_ptr<MazeGenerator const>> MazeGenerator::all() {
    auto registry = getRegistry();
    std::lock_guard guard(registry->mutex);
    std::vector<std::shared_ptr<MazeGenerator const>> mazes;
    for (auto tuple : registry->generatorByType) {
        mazes.push_back(tuple.second);
    }
    return mazes;
}

std::vector<std::shared_ptr<MazeGenerator const>> MazeGenerator::allOfQuality(MazeQuality quality) {
    auto registry = getRegistry();
    std::lock_guard guard(registry->mutex);
    std::vector<std::shared_ptr<MazeGenerator const>> mazes;
    for (auto type : registry->generatorTypeByQuality[quality]) {
        mazes.push_back(get(type));
    }
    return mazes;
}

MazeGeneratorRegistry *MazeGenerator::getRegistry() {
    static MazeGeneratorRegistry s_registry;
    return &s_registry;
}
