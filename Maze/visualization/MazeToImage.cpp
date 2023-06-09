//
//  MazeToImage.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#include "MazeToImage.h"

constexpr RGBA PATH_COLOR = { 140, 160, 230, 255 };
constexpr RGBA BG_COLOR = { 255, 255, 255, 255 };
constexpr RGBA START_COLOR = { 0, 204, 0, 255 };
constexpr RGBA FINISH_COLOR = { 255, 0, 0, 255 };

static void paintRect(Image *image, int x, int y, int width, int height, RGBA color = BG_COLOR) {
    for (int row = y; row < y + height; ++row) {
        for (int column = 0; column < width; ++column) {
            image->pixels[x + column + row * image->width] = color;
        }
    }
}

static void clearTopWall(Image *image, XY pos, PixelSizes const &sizes, RGBA color = BG_COLOR) {
    int stride = sizes.cell + sizes.wall;
    paintRect(image, sizes.border + sizes.wall + pos.x * stride, sizes.border + pos.y * stride, sizes.cell, sizes.wall, color);
}

static void clearLeftWall(Image *image, XY pos, PixelSizes const &sizes, RGBA color = BG_COLOR) {
    int stride = sizes.cell + sizes.wall;
    paintRect(image, sizes.border + pos.x * stride, sizes.border + sizes.wall + pos.y * stride, sizes.wall, sizes.cell, color);
}

static void paintUpArrow(Image *image, XY pos, PixelSizes const &sizes, bool entering, RGBA color = START_COLOR) {
    int stride = sizes.cell + sizes.wall;
    int yOffset = entering ? 0 : (sizes.cell / 2 + 1 + sizes.wall * 2);
    int startInset = (entering || sizes.cell < 3) ? 0 : 1;
    for (int inset = startInset; inset <= sizes.cell / 2; ++inset) {
        paintRect(image,
                  sizes.border + sizes.wall + pos.x * stride + inset + 1,
                  sizes.border + sizes.wall + (pos.y + 1) * stride - inset - 1 - yOffset,
                  sizes.cell - inset * 2 - 2,
                  1,
                  color);
    }
}

static void paintDownArrow(Image *image, XY pos, PixelSizes const &sizes, bool entering, RGBA color = START_COLOR) {
    int stride = sizes.cell + sizes.wall;
    int yOffset = entering ? 0 : (sizes.cell / 2 + 1 + sizes.wall * 2);
    int startInset = (entering || sizes.cell < 3) ? 0 : 1;
    for (int inset = startInset; inset <= sizes.cell / 2; ++inset) {
        paintRect(image,
                  sizes.border + sizes.wall + pos.x * stride + inset + 1,
                  sizes.border + pos.y * stride + inset + yOffset,
                  sizes.cell - inset * 2 - 2,
                  1,
                  color);
    }
}

static void paintRightArrow(Image *image, XY pos, PixelSizes const &sizes, bool entering, RGBA color = START_COLOR) {
    int stride = sizes.cell + sizes.wall;
    int xOffset = entering ? 0 : (sizes.cell / 2 + 1 + sizes.wall * 2);
    int startInset = (entering || sizes.cell < 3) ? 0 : 1;
    for (int inset = startInset; inset <= sizes.cell / 2; ++inset) {
        paintRect(image,
                  sizes.border + pos.x * stride + inset + xOffset,
                  sizes.border + sizes.wall + pos.y * stride + inset + 1,
                  1,
                  sizes.cell - inset * 2 - 2,
                  color);
    }
}
static void paintLeftArrow(Image *image, XY pos, PixelSizes const &sizes, bool entering, RGBA color = START_COLOR) {
    int stride = sizes.cell + sizes.wall;
    int xOffset = entering ? 0 : (sizes.cell / 2 + 1 + sizes.wall * 2);
    int startInset = (entering || sizes.cell < 3) ? 0 : 1;
    for (int inset = startInset; inset <= sizes.cell / 2; ++inset) {
        paintRect(image,
                  sizes.border + sizes.wall + (pos.x + 1) * stride - inset - 1 - xOffset,
                  sizes.border + sizes.wall + pos.y * stride + inset + 1,
                  1,
                  sizes.cell - inset * 2 - 2,
                  color);
    }
}

static void indicateStartEnd(Image *image, Maze const *maze, XY pos, PixelSizes const &sizes, RGBA color = PATH_COLOR) {
    int stride = sizes.cell + sizes.wall;
    
    bool isStart = (pos == maze->getStart());
    RGBA endpointIndicatorColor = isStart ? START_COLOR : FINISH_COLOR;

    if (pos.x == 0) {
        clearLeftWall(image, pos, sizes, BG_COLOR);
        paintRect(image,
                  sizes.border + pos.x * stride,
                  sizes.border + stride / 2 + pos.y * stride,
                  stride / 2,
                  sizes.wall,
                  color);
        if (isStart) {
            paintRightArrow(image, pos, sizes, true, endpointIndicatorColor);
        } else {
            paintLeftArrow(image, pos, sizes, false, endpointIndicatorColor);
        }
    } else if (pos.x == maze->getWidth() - 1) {
        clearLeftWall(image, XY(pos.x + 1, pos.y), sizes, BG_COLOR);
        paintRect(image,
                  sizes.border + sizes.wall + (pos.x + 1) * stride - stride / 2,
                  sizes.border + stride / 2 + pos.y * stride,
                  stride / 2,
                  sizes.wall,
                  color);
        if (isStart) {
            paintLeftArrow(image, pos, sizes, true, endpointIndicatorColor);
        } else {
            paintRightArrow(image, pos, sizes, false, endpointIndicatorColor);
        }
    } else if (pos.y == 0) {
        clearTopWall(image, pos, sizes, BG_COLOR);
        paintRect(image,
                  sizes.border + stride / 2 + pos.x * stride,
                  sizes.border + pos.y * stride,
                  sizes.wall,
                  stride / 2,
                  color);
        if (isStart) {
            paintDownArrow(image, pos, sizes, true, endpointIndicatorColor);
        } else {
            paintUpArrow(image, pos, sizes, false, endpointIndicatorColor);
        }
    } else if (pos.y == maze->getHeight() - 1) {
        clearTopWall(image, XY(pos.x, pos.y + 1), sizes, BG_COLOR);
        paintRect(image,
                  sizes.border + stride / 2 + pos.x * stride,
                  sizes.border + sizes.wall + (pos.y + 1) * stride - stride / 2,
                  sizes.wall,
                  stride / 2,
                  color);
        if (isStart) {
            paintUpArrow(image, pos, sizes, true, endpointIndicatorColor);
        } else {
            paintDownArrow(image, pos, sizes, false, endpointIndicatorColor);
        }
    } else {
        // Draw a square to indicate that this is an endpoint.
        int inset = std::max(0, ((sizes.cell - 1) * 2) / 5);
        paintRect(image,
                  sizes.border + sizes.wall + pos.x * stride + inset,
                  sizes.border + sizes.wall + pos.y * stride + inset,
                  sizes.cell - inset * 2,
                  sizes.cell - inset * 2,
                  endpointIndicatorColor);
    }
}

std::unique_ptr<Image> convertToImage(Maze const *maze, PixelSizes const &sizes, std::vector<XY> const &shortestPath) {
    auto image = std::make_unique<Image>();
    image->width = sizes.wall + ((sizes.wall + sizes.cell) * maze->getWidth()) + sizes.border * 2;
    image->height = sizes.wall + ((sizes.wall + sizes.cell) * maze->getHeight()) + sizes.border * 2;
    image->pixels.resize(image->width * image->height);
    
    paintRect(image.get(), 0, 0, image->width, sizes.border);
    paintRect(image.get(), 0, image->height - sizes.border, image->width, sizes.border);
    paintRect(image.get(), 0, sizes.border, sizes.border, image->height - sizes.border * 2);
    paintRect(image.get(), image->width - sizes.border, sizes.border, sizes.border, image->height - sizes.border * 2);
    
    const int stride = sizes.wall + sizes.cell;
    
    for (int cx = 0; cx < maze->getWidth(); ++ cx) {
        for (int cy = 0; cy < maze->getHeight(); ++ cy) {
            XY xy(cx, cy);
            Cell cell = (*maze)[xy];
            
            // Cell area
            paintRect(image.get(), sizes.border + sizes.wall + cx * stride, sizes.border + sizes.wall + cy * stride, sizes.cell, sizes.cell);
            
            // Walls
            if (!cell.topWall) {
                clearTopWall(image.get(), xy, sizes);
            }
            if (!cell.leftWall) {
                clearLeftWall(image.get(), xy, sizes);
            }
        }
    }
    
    // Start and end definition
    auto pathColor = shortestPath.empty() ? BG_COLOR : PATH_COLOR;
    indicateStartEnd(image.get(), maze, maze->getStart(), sizes, pathColor);
    indicateStartEnd(image.get(), maze, maze->getFinish(), sizes, pathColor);
    
    // Path drawing
    for (int i = 1; i < shortestPath.size(); ++i) {
        XY p1 = shortestPath[i-1];
        XY p2 = shortestPath[i];
        if (p2 < p1) {
            XY swap = p1;
            p1 = p2;
            p2 = swap;
        }
        
        paintRect(image.get(),
                  sizes.border + stride / 2 + p1.x * stride,
                  sizes.border + stride / 2 + p1.y * stride,
                  (p2.x - p1.x) * stride + sizes.wall,
                  (p2.y - p1.y) * stride + sizes.wall,
                  pathColor);
    }
    
    return image;
}
