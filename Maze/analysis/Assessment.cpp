//
//  Assessment.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#include "Assessment.h"
#include "Solver.h"

FullAssessment valueOfMaze(GeneratedMaze const &maze) {
    FullAssessment assessment;
    auto analysis = std::make_shared<Analysis>();
    solve(maze.maze.get(), *analysis);
    
    assessment.maze = maze;
    assessment.analysis = analysis;
    assessment.value = valueOfAnalysis(analysis.get());
    
    return assessment;
}

int valueOfAnalysis(Analysis const *analysis) {
    constexpr int kSingleMetricLargeReward = 1000;
    constexpr int kSingleMetricSmallReward = 500;
    
    // Reward longer paths
    int value = (kSingleMetricLargeReward * (int)analysis->shortestPath.size()) / analysis->size.area();
    
    // Reward singular paths (debatable)
    if (analysis->singularPath) {
        value += kSingleMetricSmallReward;
    }
    
    if (analysis->isDegenerate()) {
        value -= kSingleMetricSmallReward;
    }
    if (!analysis->isSolvable()) {
        value -= kSingleMetricLargeReward;
    }
    
    // Punish unreachable cells
    value -= (analysis->unreachableCells * kSingleMetricLargeReward * 5) / analysis->totalCellCount();
    
    return value;
}
