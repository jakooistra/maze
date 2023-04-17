//
//  Solver.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#include <queue>

#include "Solver.h"

static XY invalid { -1, -1 };

struct SolverCell {
    XY shortestLink { -1, -1 };
    int distanceToStart { 0 };
    bool somethingPointsToThis { false };
    
    bool visited() const {
        return shortestLink != invalid;
    }
};

// Run a breadth-first search on the cells in the maze, marking all cells in
// startingLocations as distance 0. Returns a grid of cells where all visited
// cells have a shortestLink pointing backward to the nearest starting location
// and a minimized distance value tracking how far the search iterated to get
// there. Finally, each cell has somethingPointsToThis=true if there is another
// cell for which its shortestLink specifies the cell in question.
static std::vector<std::vector<SolverCell>> runBreadthFirstPathing(Maze const *maze, std::vector<XY> const &startingLocations) {
    std::vector<std::vector<SolverCell>> solverCells;
    solverCells.resize(maze->getWidth());
    for (int x = 0; x < maze->getWidth(); ++x) {
        solverCells[x].resize(maze->getHeight());
    }
    
    std::queue<XY> bfs;
    for (auto start : startingLocations) {
        solverCells[start.x][start.y].shortestLink = start;
        bfs.push(start);
    }
    
    while (!bfs.empty()) {
        XY currentPosition = bfs.front();
        bfs.pop();
        auto const &currentCell = solverCells[currentPosition.x][currentPosition.y];
        
        int nextDistanceToStart = currentCell.distanceToStart + 1;
        for (XY nextPosition : currentPosition.allAdjacent()) {
            if (maze->contains(nextPosition) && maze->canTraverse(currentPosition, nextPosition)) {
                auto &nextCell = solverCells[nextPosition.x][nextPosition.y];
                if (!nextCell.visited() || nextDistanceToStart < nextCell.distanceToStart) {
                    nextCell.distanceToStart = nextDistanceToStart;
                    nextCell.shortestLink = currentPosition;
                    bfs.push(nextPosition);
                }
            }
        }
    }
    
    // Mark linked cells.
    for (int x = 0; x < maze->getWidth(); ++x) {
        for (int y = 0; y < maze->getHeight(); ++y) {
            auto cell = solverCells[x][y];
            if (cell.visited()) {
                auto link = cell.shortestLink;
                solverCells[link.x][link.y].somethingPointsToThis = true;
            }
        }
    }
    
    return solverCells;
}

void solve(Maze const *maze, Analysis &analysis) {
    // reset the analysis
    analysis = Analysis();
    analysis.size = maze->getSize();
    
    auto solverCells = runBreadthFirstPathing(maze, { maze->getStart() });
    
    // Record the solution if it exists by tracing backwards from the finish.
    auto finish = maze->getFinish();
    auto finishCell = solverCells[finish.x][finish.y];
    if (finishCell.visited()) {
        XY path = finish;
        analysis.shortestPath.push_back(path);
        while (path != maze->getStart()) {
            path = solverCells[path.x][path.y].shortestLink;
            analysis.shortestPath.insert(analysis.shortestPath.begin(), path);
        }
    }
    
    // Check reachable areas.
    for (int x = 0; x < maze->getWidth(); ++x) {
        for (int y = 0; y < maze->getHeight(); ++y) {
            auto const &cell = solverCells[x][y];
            if (cell.visited()) {
                analysis.reachableCells++;
            } else {
                analysis.unreachableCells++;
            }
        }
    }
    
    analysis.singularPath = false;
    if (analysis.isSolvable()) {
        // Run breadth-first pathing on the maze again, with the start position
        // as every cell on the shortest path. This way, all dead ends will end
        // up having the "distance from start" metric equal to the distance
        // from the shortest path.
        solverCells = runBreadthFirstPathing(maze, analysis.shortestPath);
        
        // Find all branch endpoints by checking every cell.
        for (int x = 0; x < maze->getWidth(); ++x) {
            for (int y = 0; y < maze->getHeight(); ++y) {
                const XY position(x, y);
                if (position != finish) {
                    auto const &cell = solverCells[x][y];
                    if (!cell.somethingPointsToThis && cell.visited()) {
                        analysis.branches.push_back({position, cell.distanceToStart});
                    }
                }
            }
        }
        
        // Determine if the solution is singular.
        // Search iteratively from the shortest path. If any cell has multiple
        // adjacent cells with distance-to-shortest-path less than or equal to
        // the checked cell, the path is not singular. One way to check this is
        // if any one adjacent cell is not the shortestLink for that cell and
        // has a distance metric less than or equal to the checked cell.
        analysis.singularPath = true;
        std::queue<XY> checkPoints;
        for (auto check : analysis.shortestPath) {
            checkPoints.push(check);
        }
        while (analysis.singularPath && !checkPoints.empty()) {
            XY check = checkPoints.front();
            checkPoints.pop();
            auto const &checkCell = solverCells[check.x][check.y];
            
            for (auto adjacent : check.allAdjacent()) {
                if (maze->contains(adjacent) &&
                    checkCell.shortestLink != adjacent &&
                    maze->canTraverse(check, adjacent))
                {
                    auto const &adjacentCell = solverCells[adjacent.x][adjacent.y];
                    if (adjacentCell.distanceToStart <= checkCell.distanceToStart) {
                        if (checkCell.distanceToStart > 0) {
                            analysis.singularPath = false;
                            break;
                        }
                    } else {
                        checkPoints.push(adjacent);
                    }
                }
            }
        }
    }
    
    // Check local 2x2 connectivity for tight loops.
    for (int x = 1; x < maze->getWidth(); ++x) {
        for (int y = 1; y < maze->getHeight(); ++y) {
            if (!(*maze)[{x, y}].leftWall &&
                !(*maze)[{x, y}].topWall &&
                !(*maze)[{x-1, y}].topWall &&
                !(*maze)[{x, y-1}].leftWall)
            {
                analysis.numDegenerateCorners++;
            }
        }
    }
}
