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
#include "MazeType.h"

class MazeGenerator {
private:
    MazeType type;
    std::string description;
    
public:
    MazeGenerator(MazeType _type, std::string const &_description) : type(_type), description(_description) {}
    virtual ~MazeGenerator() {}
    
    MazeType getType() const { return type; }
    std::string getDescription() const { return description; }
    
    GeneratedMaze generate(int width, int height, int seed) {
        return {
            getType(),
            seed,
            generateInternal(width, height, seed),
        };
    }
    
protected:
    virtual std::unique_ptr<Maze> generateInternal(int width, int height, int seed) = 0;
};

#endif /* MazeGenerator_h */
