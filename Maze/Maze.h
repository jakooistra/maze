//
//  Maze.h
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#ifndef Maze_h
#define Maze_h

#include <vector>

#include "XY.h"

struct Cell {
    bool leftWall;
    bool topWall;
    
    Cell() : leftWall(true), topWall(true) {}
    Cell(bool _leftWall, bool _topWall) : leftWall(_leftWall), topWall(_topWall) {}
};

class Maze {
private:
    int width;
    int height;
    std::vector<std::vector<Cell>> cell;
    
    XY start;
    XY finish;
    
public:
    Maze(int width, int height);
    virtual ~Maze();
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    XY getSize() const { return XY(width, height); }
    
    XY getStart() const { return start; }
    XY getFinish() const { return finish; }
    
    void setStart(XY start);
    void setFinish(XY finish);
    
    void setCell(int x, int y, Cell cell);
    inline void setCell(XY const &xy, Cell cell) { setCell(xy.x, xy.y, cell); }
    Cell operator[](XY pos) const;
    
    bool contains(int x, int y) const;
    inline bool contains(XY position) const { return contains(position.x, position.y); }
    
    // Returns true if there is no wall between the adjacent points p1 and p2.
    bool canTraverse(XY p1, XY p2) const;
    
    // If the given points are adjacent, clears the wall between them.
    void makeTraversable(XY p1, XY p2);
};


#endif /* Maze_h */
