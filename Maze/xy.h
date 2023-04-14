//
//  xy.h
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#ifndef xy_h
#define xy_h

#include <vector>

struct XY {
    int x;
    int y;
    
    XY(int _x, int _y) : x(_x), y(_y) {}
    
    bool operator==(const XY &other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const XY &other) const {
        return x != other.x || y != other.y;
    }
    
    XY left() const { return XY(x - 1, y); }
    XY right() const { return XY(x + 1, y); }
    XY up() const { return XY(x, y - 1); }
    XY down() const { return XY(x, y + 1); }
    
    bool isAdjacent(const XY &other) const {
        return (x == other.x && abs(y - other.y) == 1)
            || (y == other.y && abs(x - other.x) == 1);
    }
    
    std::vector<XY> allAdjacent() const {
        return { left(), right(), up(), down() };
    };
};

#endif /* xy_h */
