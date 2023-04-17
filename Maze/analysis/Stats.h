//
//  Stats.h
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#ifndef Stats_h
#define Stats_h

#include <map>

#include "Analysis.h"

struct Stats {
    int smallestSeed { 0 };
    int largestSeed { 0 };
    
    XY smallest { 0, 0 };
    XY largest { 0, 0 };
    
    // The number of analyses accumulated in these statistics.
    int count { 0 };
    
    int numSolvable { 0 };
    int numSingular { 0 };
    int numDegenerate { 0 };
    
    int totalSolvablePathLength { 0 };
    
    int totalDegenerateCorners { 0 };
    
    int totalReachable { 0 };
    int totalUnreachable { 0 };
    
    int numDeadEnds { 0 };
    
    // Map of dead end length buckets to a count of lengths in that bucket.
    // Length buckets are exponential (1, 2, 4, 8, 16, 32), and a dead end
    // branch will be included in that bucket if it is <= the bucket value.
    // Example: lengths of (5, 7, 10) would map to { 8:2, 16:1 }
    std::map<int, int> deadEndLength;
    
    void accumulate(Analysis const *analysis, int seed);
    void print(std::string const &title) const;
};

#endif /* Stats_h */
