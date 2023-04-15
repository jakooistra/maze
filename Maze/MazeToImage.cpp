//
//  MazeToImage.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#include "MazeToImage.h"

constexpr RGBA PATH_COLOR = { 192, 212, 255, 255 };
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

static void clearTopWall(Image *image, XY pos, int wallWidth, int cellWidth, RGBA color = BG_COLOR) {
    int stride = cellWidth + wallWidth;
    paintRect(image, wallWidth + pos.x * stride, pos.y * stride, cellWidth, wallWidth, color);
}

static void clearLeftWall(Image *image, XY pos, int wallWidth, int cellWidth, RGBA color = BG_COLOR) {
    int stride = cellWidth + wallWidth;
    paintRect(image, pos.x * stride, wallWidth + pos.y * stride, wallWidth, cellWidth, color);
}

static void paintUpArrow(Image *image, XY pos, int wallWidth, int cellWidth, bool entering, RGBA color = START_COLOR) {
    int stride = cellWidth + wallWidth;
    int yOffset = entering ? 0 : (cellWidth / 2 + 1 + wallWidth * 2);
    for (int inset = 0; inset <= cellWidth / 2; ++inset) {
        paintRect(image,
                  wallWidth + pos.x * stride + inset + 1,
                  wallWidth + (pos.y + 1) * stride - inset - 1 - yOffset,
                  cellWidth - inset * 2 - 2,
                  1,
                  color);
    }
}

static void paintDownArrow(Image *image, XY pos, int wallWidth, int cellWidth, bool entering, RGBA color = START_COLOR) {
    int stride = cellWidth + wallWidth;
    int yOffset = entering ? 0 : (cellWidth / 2 + 1 + wallWidth * 2);
    for (int inset = 0; inset <= cellWidth / 2; ++inset) {
        paintRect(image,
                  wallWidth + pos.x * stride + inset + 1,
                  pos.y * stride + inset + yOffset,
                  cellWidth - inset * 2 - 2,
                  1,
                  color);
    }
}

static void paintRightArrow(Image *image, XY pos, int wallWidth, int cellWidth, bool entering, RGBA color = START_COLOR) {
    int stride = cellWidth + wallWidth;
    int xOffset = entering ? 0 : (cellWidth / 2 + 1 + wallWidth * 2);
    for (int inset = 0; inset <= cellWidth / 2; ++inset) {
        paintRect(image,
                  pos.x * stride + inset + xOffset,
                  wallWidth + pos.y * stride + inset + 1,
                  1,
                  cellWidth - inset * 2 - 2,
                  color);
    }
}
static void paintLeftArrow(Image *image, XY pos, int wallWidth, int cellWidth, bool entering, RGBA color = START_COLOR) {
    int stride = cellWidth + wallWidth;
    int xOffset = entering ? 0 : (cellWidth / 2 + 1 + wallWidth * 2);
    for (int inset = 0; inset <= cellWidth / 2; ++inset) {
        paintRect(image,
                  wallWidth + (pos.x + 1) * stride - inset - 1 - xOffset,
                  wallWidth + pos.y * stride + inset + 1,
                  1,
                  cellWidth - inset * 2 - 2,
                  color);
    }
}

static void indicateStartEnd(Image *image, Maze const *maze, XY pos, int wallWidth, int cellWidth, RGBA color = PATH_COLOR) {
    int stride = cellWidth + wallWidth;
    
    bool isStart = (pos == maze->getStart());
    RGBA endpointIndicatorColor = isStart ? START_COLOR : FINISH_COLOR;

    if (pos.x == 0) {
        clearLeftWall(image, pos, wallWidth, cellWidth, BG_COLOR);
        paintRect(image,
                  pos.x * stride,
                  stride / 2 + pos.y * stride,
                  stride / 2,
                  wallWidth,
                  color);
        if (isStart) {
            paintRightArrow(image, pos, wallWidth, cellWidth, true, endpointIndicatorColor);
        } else {
            paintLeftArrow(image, pos, wallWidth, cellWidth, false, endpointIndicatorColor);
        }
    } else if (pos.x == maze->getWidth() - 1) {
        clearLeftWall(image, XY(pos.x + 1, pos.y), wallWidth, cellWidth, BG_COLOR);
        paintRect(image,
                  wallWidth + (pos.x + 1) * stride - stride / 2,
                  stride / 2 + pos.y * stride,
                  stride / 2,
                  wallWidth,
                  color);
        if (isStart) {
            paintLeftArrow(image, pos, wallWidth, cellWidth, true, endpointIndicatorColor);
        } else {
            paintRightArrow(image, pos, wallWidth, cellWidth, false, endpointIndicatorColor);
        }
    } else if (pos.y == 0) {
        clearTopWall(image, pos, wallWidth, cellWidth, BG_COLOR);
        paintRect(image,
                  stride / 2 + pos.x * stride,
                  pos.y * stride,
                  wallWidth,
                  stride / 2,
                  color);
        if (isStart) {
            paintDownArrow(image, pos, wallWidth, cellWidth, true, endpointIndicatorColor);
        } else {
            paintUpArrow(image, pos, wallWidth, cellWidth, false, endpointIndicatorColor);
        }
    } else if (pos.y == maze->getHeight() - 1) {
        clearTopWall(image, XY(pos.x, pos.y + 1), wallWidth, cellWidth, BG_COLOR);
        paintRect(image,
                  stride / 2 + pos.x * stride,
                  wallWidth + (pos.y + 1) * stride - stride / 2,
                  wallWidth,
                  stride / 2,
                  color);
        if (isStart) {
            paintUpArrow(image, pos, wallWidth, cellWidth, true, endpointIndicatorColor);
        } else {
            paintDownArrow(image, pos, wallWidth, cellWidth, false, endpointIndicatorColor);
        }
    } else {
        // Draw a square to indicate that this is an endpoint.
        int inset = std::max(0, ((cellWidth - 1) * 2) / 5);
        paintRect(image,
                  wallWidth + pos.x * stride + inset,
                  wallWidth + pos.y * stride + inset,
                  cellWidth - inset * 2,
                  cellWidth - inset * 2,
                  endpointIndicatorColor);
    }
}

std::unique_ptr<Image> convertToImage(Maze const *maze, int wallWidth, int cellWidth, std::vector<XY> const &shortestPath) {
    auto image = std::make_unique<Image>();
    image->width = wallWidth + ((wallWidth + cellWidth) * maze->getWidth());
    image->height = wallWidth + ((wallWidth + cellWidth) * maze->getHeight());
    image->pixels.resize(image->width * image->height);
    
    const int stride = wallWidth + cellWidth;
    
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
        }
    }
    
    // Start and end definition
    auto pathColor = shortestPath.empty() ? BG_COLOR : PATH_COLOR;
    indicateStartEnd(image.get(), maze, maze->getStart(), wallWidth, cellWidth, pathColor);
    indicateStartEnd(image.get(), maze, maze->getFinish(), wallWidth, cellWidth, pathColor);
    
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
                  stride / 2 + p1.x * stride,
                  stride / 2 + p1.y * stride,
                  (p2.x - p1.x) * stride + wallWidth,
                  (p2.y - p1.y) * stride + wallWidth,
                  pathColor);
    }
    
    return image;
}
