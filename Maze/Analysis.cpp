//
//  Analysis.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#include <iostream>

#include "Analysis.h"

void Analysis::print() const {
    std::cout << "Analysis:" << std::endl;
    if (isSolvable()) {
        std::cout << "  Shortest path is " << shortestPath.size() << " long." << std::endl;
        std::cout << "  Solution is " << (singularPath ? "singular." : "not singular.") << std::endl;
    } else {
        std::cout << "  Maze is not solvable." << std::endl;
    }
    
    std::cout << "  Dead ends: " << branches.size() << std::endl;
    
    int percentReachable = (100 * reachableCells) / totalCellCount();
    std::cout << "  " << percentReachable << "% of cells can be reached." << std::endl;
}
