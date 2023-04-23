//
//  Stats.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#include <iostream>
#include <sstream>
#include <iomanip>

#include "Stats.h"

void Stats::accumulate(Analysis const *analysis, int seed) {
    if (count == 0) {
        smallest = analysis->size;
        largest = analysis->size;
        smallestSeed = seed;
        largestSeed = seed;
    } else {
        int manhattan = analysis->size.manhattanDistance();
        if (manhattan < smallest.manhattanDistance()) {
            smallest = analysis->size;
        }
        if (manhattan > largest.manhattanDistance()) {
            largest = analysis->size;
        }
        smallestSeed = std::min(smallestSeed, seed);
        largestSeed = std::max(largestSeed, seed);
    }
    
    count++;
    
    if (analysis->isSolvable()) {
        numSolvable++;
        totalSolvablePathLength += analysis->shortestPath.size();
        if (analysis->singularPath) {
            numSingular++;
        }
        if (analysis->isDegenerate()) {
            numDegenerate++;
        }
    }
    
    totalDegenerateCorners += analysis->numDegenerateCorners;
    
    totalReachable += analysis->reachableCells;
    totalUnreachable += analysis->unreachableCells;
    
    numDeadEnds += analysis->branches.size();
    
    for (auto branch : analysis->branches) {
        int bucket = 1;
        while (branch.length > bucket) {
            // There is a better way to do this.
            bucket *= 2;
        }
        deadEndLength[bucket]++;
    }
}

void Stats::print(std::string const &title) const {
    std::cout << title << ":" << std::endl;
    if (count == 0) {
        std::cout << "  No analyses accumulated." << std::endl;
        return;
    } else {
        if (count == 1) {
            std::cout << "  Analysis for maze size " << smallest.x << "x" << smallest.y << ", seed " << smallestSeed;
        } else {
            std::cout << "  " << count << " mazes analyzed, " << smallest.x << "x" << smallest.y;
            if (smallest != largest) {
                std::cout << "-" << largest.x << "x" << largest.y;
            }
        }
        std::cout << std::endl;
    }
    
    if (numSolvable < count) {
        if (count == 1) {
            std::cout << "  The maze has no solution." << std::endl;
        } else {
            int percentSolvable = (100 * numSolvable + count / 2) / count;
            std::cout << "  " << percentSolvable << "% solvable" << std::endl;
        }
    }
    
    if (numSolvable > 0) {
        if (numSolvable == 1) {
            std::cout << "  The solution " << (numSingular == 0 ? "is not" : "is") << " singular" << std::endl;
        } else {
            int percentSingular = (100 * numSingular + numSolvable / 2) / numSolvable;
            std::cout << "  " << percentSingular << "% of solvable mazes have singular solutions" << std::endl;
        }
        
        int averageLength = (totalSolvablePathLength + numSolvable / 2) / numSolvable;
        if (numSolvable == 1) {
            std::cout << "  " << averageLength << " path length" << std::endl;
        } else {
            std::cout << "  " << averageLength << " average path length" << std::endl;
        }
        
        if (numDegenerate > 0) {
            if (numSolvable == 1) {
                std::cout << "  The solution is trivial" << std::endl;
            } else {
                int percentDegenerate = (100 * numDegenerate + numSolvable / 2) / numSolvable;
                std::cout << "  " << percentDegenerate << "% of solvable mazes are trivial" << std::endl;
            }
        }
    }
    
    if (totalUnreachable > 0) {
        int totalCells = totalReachable + totalUnreachable;
        int percentReachable = (100 * totalUnreachable + totalCells / 2) / totalCells;
        std::cout << "  " << percentReachable << "% of cells are unreachable" << std::endl;
    }
    
    if (totalDegenerateCorners > 0) {
        int averageDegenerateCorners = (totalDegenerateCorners + count / 2) / count;
        if (averageDegenerateCorners < 1) {
            std::cout << "  <1";
        } else {
            std::cout << "  " << averageDegenerateCorners;
        }
        if (count == 1) {
            std::cout << " degenerate corners" << std::endl;
        } else {
            std::cout << " average degenerate corners per maze" << std::endl;
        }
    }
    
    if (numSolvable > 0) {
        int averageDeadEnds = (numDeadEnds + numSolvable / 2) / numSolvable;
        if (numSolvable == 1) {
            std::cout << "  " << averageDeadEnds << " dead ends" << std::endl;
        } else {
            std::cout << "  " << averageDeadEnds << " dead ends per solvable maze on average" << std::endl;
        }
        
        int largestBucketCount = 0;
        for (auto deadEndCount : deadEndLength) {
            largestBucketCount = std::max(largestBucketCount, deadEndCount.second);
        }
        if (!deadEndLength.empty()) {
            int normalizedLength = std::min(64, largestBucketCount);
            int lastBucket = deadEndLength.rbegin()->first;
            std::stringstream lastbucketStream;
            lastbucketStream << lastBucket / 2 + 1 << "-" << lastBucket;
            int bucketPadLength = (int)lastbucketStream.str().size();
            for (int bucket = deadEndLength.rbegin()->first; bucket >= deadEndLength.begin()->first; bucket /= 2) {
                std::stringstream bucketText;
                if (bucket > 2) {
                    bucketText << bucket / 2 + 1 << "-";
                }
                bucketText << bucket;
                std::cout << "    " << std::setw(bucketPadLength) << std::setfill(' ') << bucketText.str() << ": ";
                auto tuple = deadEndLength.find(bucket);
                int length = ((tuple == deadEndLength.end() ? 0 : tuple->second) * normalizedLength + largestBucketCount - 1) / largestBucketCount;
                std::cout << std::setw(length) << std::setfill('X') << "";
                std::cout << std::endl << std::setfill(' ');
            }
        }
    }
}
