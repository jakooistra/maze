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
    auto generator = createGenerator(MazeType::RemoveRandomWalls);
    auto maze = generator->generate(10, 10, 0);
    
    Analysis analysis;
    solve(maze.get(), analysis);
    analysis.print();
    
    // TODO: add solution line to image
    
    auto image = convertToImage(maze.get(), 4, 20);
    std::vector<unsigned char> rgba;
    image->encodeRGBA(rgba);
    lodepng::encode("maze.png", &rgba[0], image->width, image->height);
    
    Stats stats;
    for (int i = 0; i < 1000; ++i) {
        maze = generator->generate(10, 10, i);
        solve(maze.get(), analysis);
        stats.accumulate(&analysis);
    }
    stats.print(getMazeTypeName(generator->getType()));
    
    return 0;
}
