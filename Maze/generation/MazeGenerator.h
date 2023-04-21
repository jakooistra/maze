//
//  MazeGenerator.h
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#ifndef MazeGenerator_h
#define MazeGenerator_h

#include <memory>

#include "GeneratedMaze.h"
#include "Maze.h"
#include "MazeQuality.h"

class MazeGenerator {
private:
    std::string argument;
    std::string name;
    std::string description;
    MazeQuality quality;
    
public:
    MazeGenerator(std::string const &argument, std::string const &name, std::string const &description, MazeQuality quality)
    :   argument(argument)
    ,   name(name)
    ,   description(description)
    ,   quality(quality)
    {}
    
    virtual ~MazeGenerator() {}
    
    std::string const &getName() const { return name; }
    std::string const &getDescription() const { return description; }
    std::string const &getArgumentName() const { return argument; }
    MazeQuality getQuality() const { return quality; }
    
    GeneratedMaze generate(int width, int height, int seed) const {
        return {
            getArgumentName(),
            seed,
            generateInternal(width, height, seed),
        };
    }
    
protected:
    virtual std::unique_ptr<Maze> generateInternal(int width, int height, int seed) const = 0;
};

#endif /* MazeGenerator_h */
