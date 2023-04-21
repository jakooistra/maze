//
//  FiftyFiftyGenerator.cpp
//  maze
//
//  Created by John Kooistra on 2023-04-16.
//

#include <random>

#include "MazeGenerator.h"
#include "Utility.h"

// Generates a maze by giving each wall segment a 50/50 chance to exist.
//
// Properties: highly variable, not guaranteed to be solvable, lots of
// unreachable areas, very quick to generate.
class FiftyFiftyGenerator : public MazeGenerator {
public:
    FiftyFiftyGenerator()
    :   MazeGenerator("ff",
                      "Fifty Fifty",
                      "Every wall has a 50% chance to exist.",
                      MazeQuality::Trivial)
    {
    }
    
    virtual ~FiftyFiftyGenerator() {}
    
protected:
    std::unique_ptr<Maze> generateInternal(int width, int height, int seed) const {
        auto maze = std::make_unique<Maze>(width, height);
        auto rng = std::bind(std::uniform_int_distribution<>(0, INT_MAX), std::mt19937(seed));
        
        setRandomStartAndFinish(maze.get(), width, height, rng);
        
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                bool leftWall = (rng() % 2) == 1;
                bool topWall = (rng() % 2) == 1;
                maze->setCell(x, y, Cell(leftWall, topWall));
            }
        }
        
        return maze;
    }
};

REGISTER_AND_STORE_GENERATOR(FiftyFiftyGenerator);
