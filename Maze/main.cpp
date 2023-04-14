//
//  main.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#include <iostream>
#include <set>

#include "lodepng.h"

#include "Assessment.h"
#include "GeneratorFactory.h"
#include "Maze.h"
#include "MazeToImage.h"
#include "Stats.h"

int main(int argc, const char * argv[]) {
    int wallWidth = 2;
    int cellSize = 12;
    
    int width = 12;
    int height = 12;
    int numMazesToGenerateForStats = 1000;
    
    int bestAndWorstCount = 3;
    
    std::vector<MazeType> types = {
        MazeType::RemoveRandomWalls,
        MazeType::VerticalPathBreaks,
    };
    
    for (auto type : types) {
        auto generator = createGenerator(type);
        auto typeName = getMazeTypeName(generator->getType());
        
        std::set<FullAssessment> bestMazes;
        std::set<FullAssessment> worstMazes;
        
        Stats stats;
        for (int i = 0; i < numMazesToGenerateForStats; ++i) {
            auto assessment = assessValue(generator->generate(width, height, i));
            stats.accumulate(assessment.analysis.get());
            
            bestMazes.insert(assessment);
            if (bestMazes.size() > bestAndWorstCount) {
                bestMazes.erase(bestMazes.begin());
            }
            
            worstMazes.insert(assessment);
            if (worstMazes.size() > bestAndWorstCount) {
                worstMazes.erase(*worstMazes.rbegin());
            }
        }
        stats.print(typeName);
        
        // Write an image of a solvable maze, or the last-generated unsolvable one.
        auto assessment = *bestMazes.rbegin();
        auto image = convertToImage(assessment.maze.get(), wallWidth, cellSize,
                                    assessment.analysis->shortestPath);
        std::vector<unsigned char> rgba;
        image->encodeRGBA(rgba);
        lodepng::encode(typeName + ".png", &rgba[0], image->width, image->height);
    }
    
    return 0;
}
