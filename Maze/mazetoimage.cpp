//
//  mazetoimage.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#include "mazetoimage.h"

static void paintRect(Image *image, int x, int y, int width, int height, unsigned char value = 255) {
    for (int row = y; row < y + height; ++row) {
        memset(&image->data[x + row * image->width], value, width);
    }
}

static void clearTopWall(Image *image, XY pos, int wallWidth, int cellWidth, unsigned char value = 255) {
    int stride = cellWidth + wallWidth;
    paintRect(image, wallWidth + pos.x * stride, pos.y * stride, cellWidth, wallWidth, value);
}

static void clearLeftWall(Image *image, XY pos, int wallWidth, int cellWidth, unsigned char value = 255) {
    int stride = cellWidth + wallWidth;
    paintRect(image, pos.x * stride, wallWidth + pos.y * stride, wallWidth, cellWidth, value);
}

static void indicateStartEnd(Image *image, Maze const *maze, XY pos, int wallWidth, int cellWidth, unsigned char value = 128) {
    int stride = cellWidth + wallWidth;
    int inset = std::max(0, std::min(2, (cellWidth - 1) / 2));
    paintRect(image,
              wallWidth + pos.x * stride + inset,
              wallWidth + pos.y * stride + inset,
              cellWidth - inset * 2,
              cellWidth - inset * 2,
              value);

    if (pos.x == 0) {
        clearLeftWall(image, pos, wallWidth, cellWidth, value);
        paintRect(image, wallWidth + pos.x * stride, stride / 2 + pos.y * stride, inset, wallWidth, value);
    } else if (pos.x == maze->getWidth() - 1) {
        clearLeftWall(image, XY(pos.x + 1, pos.y), wallWidth, cellWidth, value);
        paintRect(image, (pos.x + 1) * stride - inset, stride / 2 + pos.y * stride, inset, wallWidth, value);
    } else if (pos.y == 0) {
        clearTopWall(image, pos, wallWidth, cellWidth, value);
        paintRect(image, stride / 2 + pos.x * stride, wallWidth + pos.y * stride, wallWidth, inset, value);
    } else if (pos.y == maze->getHeight() - 1) {
        clearTopWall(image, XY(pos.x, pos.y + 1), wallWidth, cellWidth, value);
        paintRect(image, stride / 2 + pos.x * stride, (pos.y + 1) * stride - inset, wallWidth, inset, value);
    } else {
        paintRect(image,
                  wallWidth + pos.x * stride + inset,
                  wallWidth + pos.y * stride + inset,
                  cellWidth - inset * 2,
                  cellWidth - inset * 2,
                  128);
    }
}

std::unique_ptr<Image> convertToImage(Maze const *maze, int wallWidth, int cellWidth) {
    auto image = std::make_unique<Image>();
    image->width = wallWidth + ((wallWidth + cellWidth) * maze->getWidth());
    image->height = wallWidth + ((wallWidth + cellWidth) * maze->getHeight());
    
    const int stride = wallWidth + cellWidth;
    
    image->data.resize(image->width * image->height);
    for (int cx = 0; cx < maze->getWidth(); ++ cx) {
        for (int cy = 0; cy < maze->getHeight(); ++ cy) {
            XY xy(cx, cy);
            Cell cell = (*maze)[xy];
            
            // Cell area
            paintRect(image.get(), wallWidth + cx * stride, wallWidth + cy * stride, cellWidth, cellWidth);
            
            // Walls
            if (!cell.topWall) {
                clearTopWall(image.get(), xy, wallWidth, cellWidth);
            }
            if (!cell.leftWall) {
                clearLeftWall(image.get(), xy, wallWidth, cellWidth);
            }
            
            // TODO: pathing? Or do that as an extra pass?
        }
    }
    
    // Start and end definition
    indicateStartEnd(image.get(), maze, maze->getStart(), wallWidth, cellWidth, 255);
    indicateStartEnd(image.get(), maze, maze->getFinish(), wallWidth, cellWidth, 255);
    
    return image;
}
