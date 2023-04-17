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

// TODO: some aspect of this solver is N^2 with respect to cell count, we should be able to do linear time analysis or at least NlogN.
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
    
    auto solverCells = runBreadthFirstPathing(maze, {maze->getStart()});
    
    // Record the solution if it exists.
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
            auto cell = solverCells[x][y];
            if (cell.visited()) {
                analysis.reachableCells++;
            } else {
                analysis.unreachableCells++;
            }
        }
    }
    
    // Determine if the solution is singular.
    analysis.singularPath = true;
    for (XY path : analysis.shortestPath) {
        auto cell = solverCells[path.x][path.y];
        for (auto adjacent : path.allAdjacent()) {
            if (maze->contains(adjacent) && maze->canTraverse(path, adjacent)) {
                auto adjacentCell = solverCells[adjacent.x][adjacent.y];
                if (adjacent != cell.shortestLink) {
                    if (adjacentCell.distanceToStart < cell.distanceToStart) {
                        analysis.singularPath = false;
                        break;
                    }
                    
                    // Traverse this branch. If a descent gradient is found, it will separately lead back to the start.
                    std::queue<XY> searchLocations;
                    searchLocations.push(adjacent);
                    while (!searchLocations.empty()) {
                        XY search = searchLocations.front();
                        auto searchCell = solverCells[search.x][search.y];
                        searchLocations.pop();
                        for (auto searchNext : search.allAdjacent()) {
                            if ((searchNext != path) &&
                                (searchNext != searchCell.shortestLink) &&
                                maze->canTraverse(search, searchNext))
                            {
                                auto searchNextCell = solverCells[searchNext.x][searchNext.y];
                                if (searchNextCell.distanceToStart > searchCell.distanceToStart) {
                                    searchLocations.push(searchNext);
                                } else {
                                    analysis.singularPath = false;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (!analysis.singularPath) {
            break;
        }
    }
    
    if (analysis.isSolvable()) {
        solverCells = runBreadthFirstPathing(maze, analysis.shortestPath);
        
        // Find all branch endpoints.
        for (int x = 0; x < maze->getWidth(); ++x) {
            for (int y = 0; y < maze->getHeight(); ++y) {
                const XY position(x, y);
                if (position != finish) {
                    auto cell = solverCells[x][y];
                    if (!cell.somethingPointsToThis && cell.visited()) {
                        analysis.branches.push_back({position, cell.distanceToStart});
                    }
                }
            }
        }
    }
    
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
