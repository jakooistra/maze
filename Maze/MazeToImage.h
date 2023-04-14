//
//  MazeToImage.h
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#ifndef MazeToImage_h
#define MazeToImage_h

#include <memory>

#include "Image.h"
#include "Maze.h"

std::unique_ptr<Image> convertToImage(Maze const *maze, int wallWidth, int cellWidth);

#endif /* MazeToImage_h */
