//
//  mazetoimage.h
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#ifndef mazetoimage_h
#define mazetoimage_h

#include <memory>

#include "image.h"
#include "maze.h"

std::unique_ptr<Image> convertToImage(Maze const *maze, int wallWidth, int cellWidth);

#endif /* mazetoimage_h */
