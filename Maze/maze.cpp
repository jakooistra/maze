//
//  maze.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#include "maze.h"

Maze::Maze(int _width, int _height)
:   width(_width)
,   height(_height)
,   start(0, 0)
,   finish(_width - 1, _height - 1)
{
    cell.resize(width);
    for (int x = 0; x < width; ++x) {
        cell[x].resize(height);
    }
}

Maze::~Maze() {
}

void Maze::setStart(XY _start) {
    start = _start;
}

void Maze::setFinish(XY _finish) {
    finish = _finish;
}

Cell &Maze::operator[](XY pos) {
    if (pos.x >= 0 && pos.x < width && pos.y >= 0 && pos.y < height) {
        Cell &cellValue = cell[pos.x][pos.y];
        if (pos.x == 0) { cellValue.leftWall = true; }
        if (pos.y == 0) { cellValue.topWall = true; }
        return cellValue;
    } else {
        static Cell s_dummyCell;
        s_dummyCell = Cell();
        return s_dummyCell;
    }
}

Cell Maze::operator[](XY pos) const {
    if (pos.x >= 0 && pos.x < width && pos.y >= 0 && pos.y < height) {
        Cell cellValue = cell[pos.x][pos.y];
        if (pos.x == 0) { cellValue.leftWall = true; }
        if (pos.y == 0) { cellValue.topWall = true; }
        return cellValue;
    } else {
        return Cell();
    }
}
