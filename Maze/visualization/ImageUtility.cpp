//
//  ImageUtility.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-24.
//

#include "ImageUtility.h"

std::shared_ptr<Image> collate(int borderSize, std::vector<Image const *> images) {
    std::shared_ptr<Image> collatedImage = std::make_shared<Image>();
    
    // Calculate the dimensions of the collated image
    for (auto image : images) {
        collatedImage->width += image->width;
        collatedImage->height = std::max(collatedImage->height, image->height);
    }
    collatedImage->width -= ((int)images.size() - 1) * borderSize;
    
    // Allocate the image pixels and fill with white.
    RGBA borderColor = RGBA::gray(255);
    collatedImage->pixels.resize(collatedImage->width * collatedImage->height, borderColor);
    
    // Blit each input image into the collated image.
    int x = 0;
    for (auto image : images) {
        collatedImage->blit(image, x, 0);
        x += image->width - borderSize;
    }
    
    return collatedImage;
}
