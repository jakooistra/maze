//
//  main.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#include <iostream>
#include "lodepng.h"

#include "GeneratorFactory.h"
#include "Maze.h"
#include "MazeToImage.h"
#include "Solver.h"
#include "Stats.h"

int main(int argc, const char * argv[]) {
    int wallWidth = 2;
    int cellSize = 12;
    
    int width = 12;
    int height = 12;
    int numMazesToGenerateForStats = 1000;
    
    std::vector<MazeType> types = {
        MazeType::RemoveRandomWalls,
        MazeType::VerticalPathBreaks,
    };
    
    for (auto type : types) {
        auto generator = createGenerator(type);
        auto typeName = getMazeTypeName(generator->getType());
        
        std::shared_ptr<Maze const> maze;
        std::shared_ptr<Maze const> longestSolvableMaze;
        size_t longestPathLength = 0;
        
        Stats stats;
        for (int i = 0; i < numMazesToGenerateForStats; ++i) {
            maze = generator->generate(width, height, i);
            
            Analysis analysis;
            solve(maze.get(), analysis);
            if (analysis.isSolvable() && (longestSolvableMaze == nullptr || analysis.shortestPath.size() > longestPathLength)) {
                longestPathLength = analysis.shortestPath.size();
                longestSolvableMaze = maze;
            }
            
            stats.accumulate(&analysis);
        }
        stats.print(typeName);
        
        // Write an image of a solvable maze, or the last-generated unsolvable one.
        if (longestSolvableMaze != nullptr) {
            maze = longestSolvableMaze;
        }
        Analysis analysis;
        solve(maze.get(), analysis);
        auto image = convertToImage(maze.get(), wallWidth, cellSize, analysis.shortestPath);
        std::vector<unsigned char> rgba;
        image->encodeRGBA(rgba);
        lodepng::encode(typeName + ".png", &rgba[0], image->width, image->height);
    }
    
    return 0;
}
