//
//  Image.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#include "lodepng.h"

#include "Image.h"

bool Image::encodeRGBA(std::vector<unsigned char> &out) const {
    if (width <= 0 || height <= 0 || pixels.size() < width * height) {
        return false;
    }
    
    out.clear();
    out.reserve(width * height * 4);
    int index = 0;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            auto color = pixels[index++];
            out.push_back(color.r);
            out.push_back(color.g);
            out.push_back(color.b);
            out.push_back(color.a);
        }
    }
    
    return true;
}

unsigned int Image::encodePNG(std::vector<unsigned char> &out) const {
    std::vector<unsigned char> rgba;
    if (!encodeRGBA(out)) {
        return -1;
    }
    
    return lodepng::encode(out, rgba, width, height);
}

void Image::blit(Image const *other, int x, int y) {
    int maxRows = std::min(other->height, height - y);
    int rowBytes = 4 * std::min(width - x, other->width);
    if (rowBytes > 0) {
        for (int srcRow = 0; srcRow < maxRows; ++srcRow) {
            memcpy(&pixels[x + (y + srcRow) * width], &other->pixels[srcRow * other->width], rowBytes);
        }
    }
}
