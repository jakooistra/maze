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

static void writeMazeImageFile(std::string const &fileName, FullAssessment const &assessment, int wallWidth, int cellSize) {
    auto image = convertToImage(assessment.maze.get(), wallWidth, cellSize,
                                assessment.analysis->shortestPath);
    std::vector<unsigned char> rgba;
    image->encodeRGBA(rgba);
    lodepng::encode(fileName, &rgba[0], image->width, image->height);
}

int main(int argc, const char * argv[]) {
    int wallWidth = 1;
    int cellSize = 9;
    
    int width = 16;
    int height = 16;
    int numMazesToGenerateForStats = 250;
    
    std::vector<MazeType> types = {
        MazeType::ChainBurst,
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
            bestMazes.insert(assessment);
            worstMazes.insert(assessment);
            stats.accumulate(assessment.analysis.get());
        }
        stats.print(typeName);
        
        writeMazeImageFile(typeName + " (Best).png", *bestMazes.rbegin(), wallWidth, cellSize);
        writeMazeImageFile(typeName + " (Worst).png", *worstMazes.begin(), wallWidth, cellSize);
    }
    
    return 0;
}
