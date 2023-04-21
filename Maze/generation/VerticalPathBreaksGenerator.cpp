//
//  VerticalPathBreaksGenerator.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#include <random>

#include "MazeGenerator.h"

// Generates a maze by creating vertical hallways, and randomly removing walls between them.
//
// Properties: kind of boring, guaranteed to be solvable and singular, long
// paths (averaging 40% of all available cells), bounded but obvious dead ends.
class VerticalPathBreaksGenerator : public MazeGenerator {
public:
    VerticalPathBreaksGenerator()
    :   MazeGenerator("vp",
                      "Vertical Path Breaks",
                      "Full height paths with one random opening between each.",
                      MazeQuality::Trivial)
    {
    }
    
    virtual ~VerticalPathBreaksGenerator() {}
    
protected:
    std::unique_ptr<Maze> generateInternal(int width, int height, int seed) const {
        auto maze = std::make_unique<Maze>(width, height);
        auto rng = std::bind(std::uniform_int_distribution<>(0, INT_MAX), std::mt19937(seed));
        
        maze->setStart({0, (int)(rng() % height)});
        maze->setFinish({width-1, (int)(rng() % height)});
        
        for (int x = 0; x < width; ++x) {
            int yHole = rng() % height;
            for (int y = 0; y < height; ++y) {
                maze->setCell(x, y, Cell(y != yHole, false));
            }
        }
        
        return maze;
    }
};

REGISTER_AND_STORE_GENERATOR(VerticalPathBreaksGenerator);
