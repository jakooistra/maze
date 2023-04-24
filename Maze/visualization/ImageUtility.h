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

// Combine all images into a single wide row.
std::shared_ptr<Image> collate(int borderSize, std::vector<Image const *> images);

#endif /* ImageUtility_h */
