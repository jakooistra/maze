//
//  Maze.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#include "Maze.h"

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
    if (contains(pos)) {
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
    if (contains(pos)) {
        Cell cellValue = cell[pos.x][pos.y];
        if (pos.x == 0) { cellValue.leftWall = true; }
        if (pos.y == 0) { cellValue.topWall = true; }
        return cellValue;
    } else {
        return Cell();
    }
}

bool Maze::contains(XY position) const {
    return position.x >= 0
        && position.y >= 0
        && position.x < width
        && position.y < height;
}

bool Maze::canTraverse(XY p1, XY p2) const {
    if (!p1.isAdjacent(p2) || !contains(p1) || !contains(p2)) {
        return false;
    }
    
    if (p1.x < p2.x) {
        return !(*this)[p2].leftWall;
    } else if (p1.y < p2.y) {
        return !(*this)[p2].topWall;
    } else if (p2.x < p1.x) {
        return !(*this)[p1].leftWall;
    } else if (p2.y < p1.y) {
        return !(*this)[p1].topWall;
    } else {
        return false;
    }
}
