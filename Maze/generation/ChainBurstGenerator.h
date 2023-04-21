//
//  ChainBurstGenerator.h
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

// TODO: remove header files for these, they're not strictly necessary

#ifndef ChainBurstGenerator_h
#define ChainBurstGenerator_h

#include "MazeGenerator.h"

// Generates a maze by iteratively "bursting", branching paths from a single
// end point on the last burst. Burst length is ~7. Burst expansions are random
// but tend to go away from the finish point.
//
// Properties: guaranteed singular solutions, long average path length, if
// progress is being made the player is guaranteed not to have to back track.
//
//  250 mazes analyzed, 16x16
//  100% of solvable mazes have singular solutions
//  113 average path length
//  32 dead ends per solvable maze on average
//    33-64: X
//    17-32: XX
//     9-16: XXXXXXXXXX
//      5-8: XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//      3-4: XXXXXXXXXXXXXXXXXXXXXXX
//        2: XXXXXXXXXXXXXXXXXXXX
//        1: XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
class ChainBurstGenerator : public MazeGenerator {
public:
    ChainBurstGenerator()
    :   MazeGenerator("cb",
                      "Chain Burst",
                      "Sequentially picks a recently filled location and splits the path.",
                      MazeQuality::Complex) {}
    
    virtual ~ChainBurstGenerator() {}
    
protected:
    virtual std::unique_ptr<Maze> generateInternal(int width, int height, int seed) const;
};

#endif /* ChainBurstGenerator_h */
