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
    // Lower case short string with no spaces identifying this generator type. Used on the command line.
    std::string type;
    
    // The human-readable name of this generator.
    std::string name;
    
    // A short description of what this generator does, used on the command line.
    std::string description;
    
    // A rough quality type of this generator: trivial, complex, or any other defined value.
    MazeQuality quality;
    
public:
    MazeGenerator(std::string const &type, std::string const &name, std::string const &description, MazeQuality quality)
    :   type(type)
    ,   name(name)
    ,   description(description)
    ,   quality(quality)
    {}
    
    virtual ~MazeGenerator() {}
    
    std::string const &getType() const { return type; }
    std::string const &getName() const { return name; }
    std::string const &getDescription() const { return description; }
    MazeQuality getQuality() const { return quality; }
    
    GeneratedMaze generate(int width, int height, int seed) const {
        return {
            getType(),
            seed,
            generateInternal(width, height, seed),
        };
    }
    
protected:
    virtual std::unique_ptr<Maze> generateInternal(int width, int height, int seed) const = 0;
};

#endif /* MazeGenerator_h */
