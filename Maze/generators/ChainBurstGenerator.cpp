//
//  ChainBurstGenerator.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#include <deque>
#include <set>
#include <queue>
#include <optional>

#include "ChainBurstGenerator.h"
#include "Utility.h"

struct ChainBurstCell {
    // The index of the chain burst, iterating up as more bursts are calculated.
    // Values of 0 or less can be used as temporary markers and indicate that
    // the cell has not been visited / populated during generation.
    int index { 0 };
    
    // The traversal distance from the start position.
    int distanceFromStart { 0 };
    
    // The previous cell in the chain leading back to the start position.
    XY link { 0, 0 };
    
    void visit(XY previousLocation, ChainBurstCell const &previousCell) {
        if (previousCell.index > 0) {
            index = previousCell.index;
            link = previousLocation;
            distanceFromStart = previousCell.distanceFromStart + 1;
        }
    }
    
    // Returns true if this cell is a reachable part of the generated maze.
    bool visited() const {
        return index > 0;
    }
};

static std::function<bool(XY,XY)> furthestFrom(XY point) {
    return [point](XY a, XY b){
        return (point - a).lengthSquared() > (point - b).lengthSquared();
    };
};

// Returns the location expanded to if it exists, or nullopt otherwise.
static std::optional<XY> expandFrom(XY checkPoint, Maze *maze, std::vector<std::vector<ChainBurstCell>> &cells) {
    auto const &checkCell = cells[checkPoint.x][checkPoint.y];
    auto finish = maze->getFinish();
    
    // Check all adjacent spots for expansion.
    auto checkNext = checkPoint.allAdjacent();
    // Prioritize going further from the finish point.
    std::sort(checkNext.begin(), checkNext.end(), furthestFrom(finish));
    // If we're sufficiently far away, add some randomness.
    int manhattanDistanceFromFinish = (checkNext.back() - finish).manhattanDistance();
    if (manhattanDistanceFromFinish > 4 && rand() % 3 == 0) {
        std::swap(checkNext[0], checkNext[1 + rand() % 3]);
    }
    if (manhattanDistanceFromFinish > 7 && rand() % 3 == 0) {
        std::swap(checkNext[1], checkNext[2 + rand() % 2]);
    }
    // Determine if it's possible to expand into adjacent locations,
    // and pick only one direction to extend the path.
    for (auto next : checkNext) {
        if (maze->contains(next) && !cells[next.x][next.y].visited()) {
            cells[next.x][next.y].visit(checkPoint, checkCell);
            maze->makeTraversable(checkPoint, next);
            return next;
        }
    }
    
    return std::nullopt;
}

static std::vector<XY> burst(Maze *maze, std::vector<std::vector<ChainBurstCell>> &cells, XY start) {
    auto finish = maze->getFinish();
    
    // If this pathway is constrained, expand it in the one way it can be
    // expanded until it can burst into multiple paths.
    bool canBurst = false;
    while (!canBurst) {
        int freeAdjacentCells = 0;
        for (auto adjacent : start.allAdjacent()) {
            if (maze->contains(adjacent) && !cells[adjacent.x][adjacent.y].visited()) {
                freeAdjacentCells++;
            }
        }
        
        canBurst = freeAdjacentCells >= 2;
        if (!canBurst) {
            if (freeAdjacentCells == 0) {
                // Exit early, no expansion is possible.
                return {};
            } else {
                auto next = expandFrom(start, maze, cells);
                start = next.value();
            }
        }
    }
    
    // Burst into multiple paths.
    std::vector<XY> checkExpansion;
    auto const &startCell = cells[start.x][start.y];
    for (auto next : start.allAdjacent()) {
        if (maze->contains(next) && !cells[next.x][next.y].visited()) {
            maze->makeTraversable(start, next);
            checkExpansion.push_back(next);
            cells[next.x][next.y].visit(start, startCell);
        }
    }
    
    // Extend each burst path as far as it can go, but not too far.
    // Extended paths generally stray away from the finish point but there is
    // some randomness to that selection.
    for (int explore = 0; explore < 7; ++explore) {
        // Run through all expansion points from the previous iteration.
        // Limit the number of expansion points if we're getting close to the finish point.
        // If close to the finish point, bias expansion to the paths furthest away.
        std::sort(checkExpansion.begin(), checkExpansion.end(), furthestFrom(finish));
        while (checkExpansion.size() > 1 && (checkExpansion.back() - finish).manhattanDistance() < checkExpansion.size() * 2) {
            checkExpansion.pop_back();
        }
        
        // Expand each check point by one cell.
        std::vector<XY> nextExpansion;
        for (auto checkPoint : checkExpansion) {
            auto next = expandFrom(checkPoint, maze, cells);
            if (next.has_value()) {
                nextExpansion.push_back(next.value());
            }
        }
        
        if (nextExpansion.empty()) {
            // If no paths were extended, return the last known list of endpoints.
            return checkExpansion;
        } else {
            // If paths were extended, continue to the next iteration with those paths.
            checkExpansion = nextExpansion;
        }
    }
    
    return checkExpansion;
}

std::unique_ptr<Maze> ChainBurstGenerator::generate(int width, int height, int seed) {
    auto maze = std::make_unique<Maze>(width, height);
    
    srand(seed);
    maze->setStart(randomEdgePoint(width, height));
    maze->setFinish(randomEdgePoint(width, height));
    while (maze->getStart() == maze->getFinish() || maze->getStart().isAdjacent(maze->getFinish())) {
        maze->setFinish(randomEdgePoint(width, height));
    }
    
    // Values above zero indicate a visited cell.
    // Values below zero are used to track
    std::vector<std::vector<ChainBurstCell>> cells;
    cells.resize(width);
    for (int x = 0; x < width; ++x) {
        cells[x].resize(height);
    }
    
    XY finish = maze->getFinish();
    XY nextBurst = maze->getStart();
    int currentBurstIndex = 0;
    while (nextBurst != finish && !cells[finish.x][finish.y].visited()) {
        currentBurstIndex++;
        cells[nextBurst.x][nextBurst.y].index = ++currentBurstIndex;
        auto ends = burst(maze.get(), cells, nextBurst);
        
        if (cells[finish.x][finish.y].visited()) {
            // If the finish cell has been visited, exit. The maze is solvable.
            break;
        } else if (!ends.empty()) {
            // If the last burst was successful, pick a random endpoint and burst from it.
            nextBurst = ends[rand() % ends.size()];
        } else {
            // If the last burst was not successful, do a search from the
            // finish point to determine the best place to burst from.
            // The best burst point is the furthest visited cell from the
            // start position that is reachable from the finish position.
            std::optional<XY> best;
            int bestMetric = 0;
            std::queue<XY> bfs;
            bfs.push(finish);
            cells[finish.x][finish.y].index = -currentBurstIndex;
            while (!bfs.empty()) {
                XY check = bfs.front();
                bfs.pop();
                for (auto adjacent : check.allAdjacent()) {
                    auto &adjacentCell = cells[adjacent.x][adjacent.y];
                    if (maze->contains(adjacent)) {
                        if (adjacentCell.visited()) {
                            int metric = adjacentCell.distanceFromStart;
                            if (!best.has_value() || metric > bestMetric) {
                                bestMetric = metric;
                                best = adjacent;
                            }
                        } else if (adjacentCell.index != -currentBurstIndex) {
                            adjacentCell.index = -currentBurstIndex;
                            bfs.push(adjacent);
                        }
                    }
                }
            }
            if (best.has_value()) {
                nextBurst = best.value();
            } else {
                break;
            }
        }
    }
    
    // Fill in remaining unreachable areas. First, search for unreachable
    // areas that are just off the main solution path, then expand outward
    // from the main path. This way, new dead ends are created near where
    // a player might be traversing.
    std::deque<XY> checkPoints;
    checkPoints.push_front(maze->getFinish());
    while (checkPoints.front() != maze->getStart()) {
        auto check = checkPoints.front();
        auto &pathCell = cells[check.x][check.y];
        if (pathCell.visited() && pathCell.link != check) {
            checkPoints.push_front(pathCell.link);
        } else {
            break;
        }
    }
    // Don't test cells twice. This check could be more optimal using a hash
    // map or even a property in ChainBurstCell, but this works for now.
    std::set<XY> testedCells;
    while (!checkPoints.empty()) {
        auto check = checkPoints.front();
        checkPoints.pop_front();
        if (testedCells.contains(check)) {
            continue;
        }
        testedCells.insert(check);
        
        auto &checkCell = cells[check.x][check.y];
        for (auto adjacent : check.allAdjacent()) {
            if (maze->contains(adjacent)) {
                auto &adjacentCell = cells[adjacent.x][adjacent.y];
                
                // Expand to this cell and beyond if it hasn't been visited.
                if (!adjacentCell.visited()) {
                    maze->makeTraversable(check, adjacent);
                    adjacentCell.visit(check, checkCell);
                    
                    std::optional<XY> expandLocation = adjacent;
                    for (int i = 0; i < 7 && expandLocation.has_value(); ++i) {
                        expandLocation = expandFrom(expandLocation.value(), maze.get(), cells);
                    }
                }
                
                if (adjacentCell.distanceFromStart > checkCell.distanceFromStart) {
                    checkPoints.push_back(adjacent);
                }
            }
        }
    }
    
    return maze;
}
