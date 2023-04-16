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
    // TODO: use command line parameters to determine what to do
    // TODO: single maze analysis
    // TODO: output of maze collections
    // TODO: analysis of best mazes given heuristics
    // TODO: output with and without solution
    
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
        
        // Mazes in order from worst to best.
        std::set<FullAssessment> sortedMazes;
        
        Stats stats;
        for (int i = 0; i < numMazesToGenerateForStats; ++i) {
            auto assessment = assessValue(generator->generate(width, height, i));
            sortedMazes.insert(assessment);
            stats.accumulate(assessment.analysis.get());
        }
        stats.print(typeName);
        
        writeMazeImageFile(typeName + " (Best).png", *sortedMazes.rbegin(), wallWidth, cellSize);
        writeMazeImageFile(typeName + " (Worst).png", *sortedMazes.begin(), wallWidth, cellSize);
    }
    
    return 0;
}
