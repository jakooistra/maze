//
//  maze.h
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#ifndef maze_h
#define maze_h

#include <vector>

struct Cell {
    bool leftWall { true };
    bool topWall { true };
};

struct XY {
    int x;
    int y;
    
    XY(int _x, int _y) : x(_x), y(_y) {}
};

class Maze {
private:
    int width;
    int height;
    std::vector<std::vector<Cell>> cell;
    
public:
    Maze(int width, int height);
    virtual ~Maze();
    
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    Cell &operator[](XY pos);
    Cell operator[](XY pos) const;
};


#endif /* maze_h */
