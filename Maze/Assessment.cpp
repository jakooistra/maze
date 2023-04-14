//
//  Assessment.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#include "Assessment.h"
#include "Solver.h"

FullAssessment assessValue(std::shared_ptr<Maze const> maze) {
    FullAssessment assessment;
    auto analysis = std::make_shared<Analysis>();
    solve(maze.get(), *analysis);
    
    assessment.maze = maze;
    assessment.analysis = analysis;
    assessment.value = assessValue(analysis.get());
    
    return assessment;
}

int assessValue(Analysis const *analysis) {
    // Reward longer paths
    int value = (int)analysis->shortestPath.size();
    
    // Reward singular paths (debatable)
    if (analysis->singularPath) {
        value += 10;
    }
    
    if (analysis->isDegenerate()) {
        value -= 10;
    }
    if (!analysis->isSolvable()) {
        value -= 10;
    }
    
    // Punish unreachable cells
    value -= (analysis->unreachableCells * 50) / analysis->totalCellCount();
    
    return value;
}
