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
    
public:
    MazeGenerator(MazeType _type) : type(_type) {}
    virtual ~MazeGenerator() {}
    
    MazeType getType() const { return type; }
    
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
