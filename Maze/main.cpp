//
//  main.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#include <iostream>

#include "lodepng.h"
#include "maze.h"
#include "mazetoimage.h"

int main(int argc, const char * argv[]) {
    Maze maze(3, 3);
    maze[{0, -1}].topWall = false;
    maze[{0, 0}].topWall = false;
    maze[{0, 1}].topWall = false;
    maze[{2, 2}].leftWall = false;
    maze[{1, 1}].leftWall = false;
    
    auto image = convertToImage(&maze, 4, 20);
    
    std::vector<unsigned char> rgba;
    image->encodeRGBA(rgba);
    
    auto error = lodepng::encode("maze.png", &rgba[0], image->width, image->height);
    
    if (error != 0) {
        std::cout << "Image written with error code: " << lodepng_error_text(error) << std::endl;
    } else {
        std::cout << "Image written sccessfully." << std::endl;
    }
    
    return 0;
}
