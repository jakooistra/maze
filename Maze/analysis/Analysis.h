//
//  Analysis.h
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#ifndef Analysis_h
#define Analysis_h

#include <vector>

#include "XY.h"

struct BranchEndpoint {
    XY position;
    int length;
};

struct Analysis {
    XY size {0, 0};
    
    // Shortest solution, from start to finish inclusive.
    std::vector<XY> shortestPath;
    
    // True if there is only one way to solve the maze.
    bool singularPath { false };
    
    // The length and location of all branch dead ends.
    std::vector<BranchEndpoint> branches;
    
    // How many cells were reachable/unreachable in the analyzed maze.
    int reachableCells { 0 };
    int unreachableCells { 0 };
    
    // Unnecessarily high connectivity, measured as fully-connected corners.
    // If there is a path of length 4 that leads back to a cell, the corner
    // in the middle of that path counts as a degenerate corner.
    int numDegenerateCorners { 0 };
    
    // Returns true if there is a solution to the analyzed maze.
    bool isSolvable() const { return !shortestPath.empty(); }
    
    // Returns true if there is a solution to the maze, and it is trivial.
    // This means the end is either in the same location as the start or is adjacent.
    bool isDegenerate() const { return isSolvable() && shortestPath.size() <= 2; }
    
    int totalCellCount() const { return reachableCells + unreachableCells; }
    
    void print() const;
};

#endif /* Analysis_h */
