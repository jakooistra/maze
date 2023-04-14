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
    
    bool operator==(const XY &other) const {
        return x == other.x && y == other.y;
    }
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
};


#endif /* maze_h */
