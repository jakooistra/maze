//
//  ImageUtility.h
//  Maze
//
//  Created by John Kooistra on 2023-04-24.
//

#ifndef ImageUtility_h
#define ImageUtility_h

#include <memory>

#include "Image.h"
#include "PixelSizes.h"

// Combine all images into a single wide row.
std::shared_ptr<Image> collateImages(int borderSize, std::vector<Image const *> images);

// Combine all images into a collection grid.
// The grid should fit best on an 8.5x11" piece of letter paper, including a 0.5" border.
std::shared_ptr<Image> collectImages(PixelSizes const &sizes, std::vector<Image const *> images);

#endif /* ImageUtility_h */
