//
//  image.h
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#ifndef image_h
#define image_h

#include <vector>

// A grayscale image definition.
struct Image {
    int width { 0 };
    int height { 0 };
    std::vector<uint8_t> data;
    
    bool encodeRGBA(std::vector<unsigned char> &out) const;
    unsigned int encodePNG(std::vector<unsigned char> &out) const;
};

#endif /* image_h */
