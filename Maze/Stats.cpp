//
//  Stats.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#include <iostream>

#include "Stats.h"

void Stats::accumulate(Analysis const *analysis) {
    count++;
    
    if (analysis->isSolvable()) {
        numSolvable++;
        totalSolvablePathLength += analysis->shortestPath.size();
        if (analysis->singularPath) {
            numSingular++;
        }
    }
    
    totalReachable += analysis->reachableCells;
    totalUnreachable += analysis->unreachableCells;
    
    numDeadEnds += analysis->branches.size();
}

void Stats::print(std::string const &title) const {
    std::cout << title << ":" << std::endl;
    if (count == 0) {
        std::cout << "  No analyses accumulated." << std::endl;
        return;
    } else {
        std::cout << "  " << count << " mazes analyzed" << std::endl;
    }
    
    int percentSolvable = (100 * numSolvable + count / 2) / count;
    std::cout << "  " << percentSolvable << "% solvable" << std::endl;
    
    int percentSingular = (100 * numSingular + numSolvable / 2) / numSolvable;
    std::cout << "  " << percentSingular << "% of solvable mazes have singular solutions" << std::endl;
    
    int averageLength = (totalSolvablePathLength + numSolvable / 2) / numSolvable;
    std::cout << "  " << averageLength << " cells in average shortest solution" << std::endl;
    
    int totalCells = totalReachable + totalUnreachable;
    int percentReachable = (100 * totalReachable + totalCells / 2) / totalCells;
    std::cout << "  " << percentReachable << "% of cells can be reached" << std::endl;
    
    int averageDeadEnds = (numDeadEnds + count / 2) / count;
    std::cout << "  " << averageDeadEnds << " dead ends per maze on average" << std::endl;
}
