//
//  ImageUtility.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-24.
//

#include <math.h>

#include "ImageUtility.h"
#include "XY.h"

std::shared_ptr<Image> collateImages(int borderSize, std::vector<Image const *> images) {
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

std::shared_ptr<Image> collectImages(PixelSizes const &sizes, std::vector<Image const *> images) {
    std::shared_ptr<Image> collectedImage = std::make_shared<Image>();
    if (images.empty()) {
        return collectedImage;
    }
    
    // Determine the layout, horizontal and vertical counts, of the grid of images.
    XY mazeSize = XY(images[0]->width, images[0]->height) - sizes.border;
    double vCountMin = sqrt(((4.0 * mazeSize.x) / (3.0 * mazeSize.y)) * images.size());
    double hCountMin = vCountMin * (3.0 * mazeSize.y) / (4.0 * mazeSize.x);
    int vCount = (int)vCountMin;
    int hCount = (int)hCountMin;
    while (vCount * hCount < images.size()) {
        if ((vCountMin - vCount) > (hCountMin - hCount)) {
            vCountMin = vCount + 1;
            hCountMin = vCountMin * (3.0 * mazeSize.y) / (4.0 * mazeSize.x);
        } else {
            hCountMin = hCount + 1;
            vCountMin = hCountMin * (4.0 * mazeSize.x) / (3.0 * mazeSize.y);
        }
        vCount = (int)vCountMin;
        hCount = (int)hCountMin;
    }
    
    
    // Allocate the image pixels and fill with white.
    collectedImage->width = (hCount * mazeSize.x) + sizes.border;
    collectedImage->height = (vCount * mazeSize.y) + sizes.border;
    RGBA borderColor = RGBA::gray(255);
    collectedImage->pixels.resize(collectedImage->width * collectedImage->height, borderColor);
    
    // Blit each input image into the grid.
    for (int i = 0; i < images.size(); ++i) {
        auto image = images[i];
        int x = (i % hCount) * mazeSize.x;
        int y = (i / hCount) * mazeSize.y;
        
        collectedImage->blit(image, x, y);
    }
    
    return collectedImage;
}
