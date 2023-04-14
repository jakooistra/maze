//
//  Assessment.h
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#ifndef Assessment_h
#define Assessment_h

#include "Analysis.h"
#include "Maze.h"

struct FullAssessment {
    int value;
    std::shared_ptr<Analysis const> analysis;
    std::shared_ptr<Maze const> maze;
    
    bool operator<(FullAssessment const &other) const {
        return value < other.value || (value == other.value && maze < other.maze);
    }
};

// Returns a singular metric for the relative value of a given maze.
// Higher is better.
FullAssessment assessValue(std::shared_ptr<Maze const> maze);

// Returns a singular metric for the relative value of a maze, given its analysis.
// Higher is better.
int assessValue(Analysis const *analysis);

#endif /* Assessment_h */
