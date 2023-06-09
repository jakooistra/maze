//
//  MazeToImage.h
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#ifndef MazeToImage_h
#define MazeToImage_h

#include <memory>
#include <vector>

#include "Image.h"
#include "Maze.h"
#include "PixelSizes.h"

std::unique_ptr<Image> convertToImage(Maze const *maze, PixelSizes const &sizes, std::vector<XY> const &shortestPath = {});

#endif /* MazeToImage_h */
