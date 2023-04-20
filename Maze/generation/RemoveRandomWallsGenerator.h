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
//
// Properties: highly variable, not guaranteed to be solvable, lots of
// unreachable areas, very quick to generate.
//
// Stats:
//  250 mazes analyzed, 16x16
//  36% solvable
//  0% of solvable mazes have singular solutions
//  15 average path length
//  6% of solvable mazes are trivial
//  63% of cells are unreachable
//  15 average degenerate corners per maze
//  16 dead ends per solvable maze on average
//    33-64: XXX
//    17-32: XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//     9-16: XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//      5-8: XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//      3-4: XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//        2: XXXXXXXXXXXXXXXXXXXX
//        1: XXXXXXXXXXXXXXXXXXXXX
class RemoveRandomWallsGenerator : public MazeGenerator {
public:
    RemoveRandomWallsGenerator() : MazeGenerator(MazeType::RemoveRandomWalls, "Random walls are iteratively removed.") {}
    virtual ~RemoveRandomWallsGenerator() {}
    
protected:
    virtual std::unique_ptr<Maze> generateInternal(int width, int height, int seed);
};

#endif /* RemoveRandomWallsGenerator_h */
