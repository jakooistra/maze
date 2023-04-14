//
//  Stats.h
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#ifndef Stats_h
#define Stats_h

#include "Analysis.h"

struct Stats {
    // The number of analyses accumulated in these statistics.
    int count { 0 };
    
    int numSolvable { 0 };
    int numSingular { 0 };
    
    int totalSolvablePathLength { 0 };
    
    int totalReachable { 0 };
    int totalUnreachable { 0 };
    
    int numDeadEnds { 0 };
    
    // TODO: buckets for dead end branch length?
    
    void accumulate(Analysis const *analysis);
    void print(std::string const &title) const;
};

#endif /* Stats_h */
