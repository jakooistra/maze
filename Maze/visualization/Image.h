//
//  Image.h
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#ifndef Image_h
#define Image_h

#include <vector>

struct RGBA {
    uint8_t r { 0 };
    uint8_t g { 0 };
    uint8_t b { 0 };
    uint8_t a { 255 };
    
    static RGBA gray(uint8_t value) {
        return { value, value, value, 255 };
    }
};

// An RGBA image definition.
struct Image {
    int width { 0 };
    int height { 0 };
    std::vector<RGBA> pixels;
    
    bool encodeRGBA(std::vector<unsigned char> &out) const;
    unsigned int encodePNG(std::vector<unsigned char> &out) const;
    
    void blit(Image const *other, int x, int y);
};

#endif /* Image_h */
