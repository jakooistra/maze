//
//  maze.h
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#ifndef maze_h
#define maze_h

#include <vector>

#include "xy.h"

struct Cell {
    bool leftWall { true };
    bool topWall { true };
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
    
    XY getStart() const { return start; }
    XY getFinish() const { return finish; }
    
    void setStart(XY start);
    void setFinish(XY finish);
    
    Cell &operator[](XY pos);
    Cell operator[](XY pos) const;
    
    bool contains(XY position) const;
    
    // Returns true if there is no wall between the adjacent points p1 and p2.
    bool canTraverse(XY p1, XY p2) const;
};


#endif /* maze_h */
