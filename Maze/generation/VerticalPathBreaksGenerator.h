//
//  VerticalPathBreaksGenerator.h
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#ifndef VerticalPathBreaksGenerator_h
#define VerticalPathBreaksGenerator_h

#include "MazeGenerator.h"

// Generates a maze by creating vertical hallways, and randomly removing walls between them.
//
// Properties: kind of boring, guaranteed to be solvable and singular, long
// paths (averaging 40% of all available cells), bounded but obvious dead ends.
//
// Stats:
//  250 mazes analyzed, 16x16
//  100% of solvable mazes have singular solutions
//  101 average path length
//  28 dead ends per solvable maze on average
//    9-16: XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//     5-8: XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//     3-4: XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
//       2: XXXXXXXXXXXXXXXXXXXXXXXX
//       1: XXXXXXXXXXXXXXXXXXXXXXXXXXX
class VerticalPathBreaksGenerator : public MazeGenerator {
public:
    VerticalPathBreaksGenerator()
    :   MazeGenerator("vp",
                      "Vertical Path Breaks",
                      "Full height paths with one random opening between each.",
                      MazeQuality::Trivial) {}
    
    virtual ~VerticalPathBreaksGenerator() {}
    
protected:
    virtual std::unique_ptr<Maze> generateInternal(int width, int height, int seed) const;
};

#endif /* VerticalPathBreaksGenerator_h */
