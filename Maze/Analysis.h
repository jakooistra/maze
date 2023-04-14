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
    // Shortest solution, from start to finish inclusive.
    std::vector<XY> shortestPath;
    
    // True if there is only one way to solve the maze.
    bool singularPath;
    
    // The length and location of all branch dead ends.
    std::vector<BranchEndpoint> branches;
    
    // How many cells were reachable/unreachable in the analyzed maze.
    int reachableCells;
    int unreachableCells;
    
    // TODO: "donuts" - areas that have unnecessarily high connectivity
    
    // Returns true if there is a solution to the analyzed maze.
    bool isSolvable() const { return !shortestPath.empty(); }
    
    int totalCellCount() const { return reachableCells + unreachableCells; }
    
    void print() const;
};

#endif /* Analysis_h */
