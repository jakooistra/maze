//
//  Assessment.h
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#ifndef Assessment_h
#define Assessment_h

#include "Analysis.h"
#include "GeneratedMaze.h"

struct FullAssessment {
    int value;
    GeneratedMaze maze;
    std::shared_ptr<Analysis const> analysis;
    
    bool operator<(FullAssessment const &other) const {
        return value < other.value || (value == other.value && maze.seed < other.maze.seed);
    }
};

// Returns a singular metric for the relative value of a given maze.
// Higher is better.
FullAssessment assessValue(GeneratedMaze const &maze);

// Returns a singular metric for the relative value of a maze, given its analysis.
// Higher is better.
int assessValue(Analysis const *analysis);

#endif /* Assessment_h */
