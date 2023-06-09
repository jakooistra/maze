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

void Maze::setCell(int x, int y, Cell cellValue) {
    if (contains(x, y)) {
        if (x == 0) { cellValue.leftWall = true; }
        if (y == 0) { cellValue.topWall = true; }
        cell[x][y] = cellValue;
    }
}

Cell Maze::operator[](XY pos) const {
    if (contains(pos)) {
        return cell[pos.x][pos.y];
    } else {
        return Cell();
    }
}

bool Maze::contains(int x, int y) const {
    return x >= 0 && y >= 0 && x < width && y < height;
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

void Maze::makeTraversable(XY p1, XY p2) {
    if (!p1.isAdjacent(p2) || !contains(p1) || !contains(p2)) {
        return;
    }
    
    if (p1.x < p2.x) {
        cell[p2.x][p2.y].leftWall = false;
    } else if (p1.y < p2.y) {
        cell[p2.x][p2.y].topWall = false;
    } else if (p2.x < p1.x) {
        cell[p1.x][p1.y].leftWall = false;
    } else if (p2.y < p1.y) {
        cell[p1.x][p1.y].topWall = false;
    }
}
