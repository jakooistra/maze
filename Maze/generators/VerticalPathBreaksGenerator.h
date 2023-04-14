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
class VerticalPathBreaksGenerator : public MazeGenerator {
public:
    VerticalPathBreaksGenerator() : MazeGenerator(MazeType::VerticalPathBreaks) {}
    virtual ~VerticalPathBreaksGenerator() {}
    
    virtual std::unique_ptr<Maze> generate(int width, int height, int seed);
};

#endif /* VerticalPathBreaksGenerator_h */
