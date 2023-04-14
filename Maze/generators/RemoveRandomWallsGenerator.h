//
//  RemoveRandomWallsGenerator.h
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#ifndef RemoveRandomWallsGenerator_h
#define RemoveRandomWallsGenerator_h

#include "MazeGenerator.h"

// Generates a maze by randomly selecting walls to remove.
class RemoveRandomWallsGenerator : public MazeGenerator {
public:
    RemoveRandomWallsGenerator() : MazeGenerator(MazeType::RemoveRandomWalls) {}
    virtual ~RemoveRandomWallsGenerator() {}
    
    virtual std::unique_ptr<Maze> generate(int width, int height, int seed);
};

#endif /* RemoveRandomWallsGenerator_h */
