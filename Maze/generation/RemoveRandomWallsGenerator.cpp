//
//  RemoveRandomWallsGenerator.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#include <random>

#include "MazeGenerator.h"
#include "Utility.h"

// Generates a maze by randomly selecting walls to remove.
//
// Properties: highly variable, not guaranteed to be solvable, lots of
// unreachable areas, very quick to generate.
class RemoveRandomWallsGenerator : public MazeGenerator {
public:
    RemoveRandomWallsGenerator()
    :   MazeGenerator("rr",
                      "Remove Random Walls",
                      "Random walls are iteratively removed.",
                      MazeQuality::Trivial)
    {
    }
    
    virtual ~RemoveRandomWallsGenerator() {}
    
protected:
    std::unique_ptr<Maze> generateInternal(int width, int height, int seed) const {
        auto maze = std::make_unique<Maze>(width, height);
        auto rng = std::bind(std::uniform_int_distribution<>(0, INT_MAX), std::mt19937(seed));
        
        setRandomStartAndFinish(maze.get(), width, height, rng);
        
        int numWallsToRemove = (width * height * 3) / 2;
        for (int i = 0; i < numWallsToRemove; ++i) {
            XY pos { rng() % width, rng() % height };
            Cell cell = (*maze)[pos];
            if ((rng() % 2) == 1) {
                cell.topWall = false;
            } else {
                cell.leftWall = false;
            }
            maze->setCell(pos, cell);
        }
        
        return maze;
    }
};

REGISTER_AND_STORE_GENERATOR(RemoveRandomWallsGenerator);
