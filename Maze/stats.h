//
//  stats.h
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#ifndef stats_h
#define stats_h

struct Stats {
    // The number of analyses accumulated in these statistics.
    int count;
    
    int numSolvable;
    int numSingular;
    
    int totalPathLength;
    
    int totalReachable;
    int totalUnreachable;
    
    int numDeadEnds;
    // TODO: buckets for dead end branch length?
};

#endif /* stats_h */
