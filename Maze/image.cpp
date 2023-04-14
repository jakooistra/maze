//
//  image.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#include "image.h"
#include "lodepng.h"

bool Image::encodeRGBA(std::vector<unsigned char> &out) const {
    if (width <= 0 || height <= 0 || data.size() < width * height) {
        return false;
    }
    
    out.clear();
    out.reserve(width * height * 4);
    int index = 0;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            unsigned char grayValue = data[index++];
            out.push_back(grayValue);
            out.push_back(grayValue);
            out.push_back(grayValue);
            out.push_back(255);
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
