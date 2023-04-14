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

void Stats::accumulate(Analysis const *analysis) {
    if (count == 0) {
        smallest = analysis->size;
        largest = analysis->size;
    } else {
        int manhattan = analysis->size.manhattanDistance();
        if (manhattan < smallest.manhattanDistance()) {
            smallest = analysis->size;
        }
        if (manhattan > largest.manhattanDistance()) {
            largest = analysis->size;
        }
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
        std::cout << "  " << count << " mazes analyzed, " << smallest.x << "x" << smallest.y;
        if (smallest != largest) {
            std::cout << "-" << largest.x << "x" << largest.y;
        }
        std::cout << std::endl;
    }
    
    int percentSolvable = (100 * numSolvable + count / 2) / count;
    std::cout << "  " << percentSolvable << "% solvable" << std::endl;
    
    int percentSingular = (100 * numSingular + numSolvable / 2) / numSolvable;
    std::cout << "  " << percentSingular << "% of solvable mazes have singular solutions" << std::endl;
    
    int averageLength = (totalSolvablePathLength + numSolvable / 2) / numSolvable;
    std::cout << "  " << averageLength << " average path length" << std::endl;
    
    int percentDegenerate = (100 * numDegenerate + numSolvable / 2) / numSolvable;
    std::cout << "  " << percentDegenerate << "% of solvable mazes have adjacent start/finish" << std::endl;
    
    int totalCells = totalReachable + totalUnreachable;
    int percentReachable = (100 * totalUnreachable + totalCells / 2) / totalCells;
    std::cout << "  " << percentReachable << "% of cells are unreachable" << std::endl;
    
    int averageDeadEnds = (numDeadEnds + count / 2) / count;
    std::cout << "  " << averageDeadEnds << " dead ends per solvable maze on average" << std::endl;
    
    int largestBucketCount = 0;
    for (auto deadEndCount : deadEndLength) {
        largestBucketCount = std::max(largestBucketCount, deadEndCount.second);
    }
    if (!deadEndLength.empty()) {
        int normalizedLength = std::min(64, largestBucketCount);
        int lastBucket = deadEndLength.rbegin()->first;
        std::stringstream lastbucketStream;
        lastbucketStream << lastBucket;
        int bucketPadLength = (int)lastbucketStream.str().size();
        for (int bucket = deadEndLength.rbegin()->first; bucket >= deadEndLength.begin()->first; bucket /= 2) {
            std::cout << "    " << std::setw(bucketPadLength) << std::setfill(' ') << bucket << ": ";
            auto tuple = deadEndLength.find(bucket);
            int length = ((tuple == deadEndLength.end() ? 0 : tuple->second) * normalizedLength + largestBucketCount - 1) / largestBucketCount;
            std::cout << std::setw(length) << std::setfill('X') << "";
            std::cout << std::endl;
        }
    }
}
