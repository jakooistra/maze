//
//  XY.h
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#ifndef XY_h
#define XY_h

#include <vector>

struct XY {
    int x;
    int y;
    
    XY(int _x, int _y) : x(_x), y(_y) {}
    XY(int both) : x(both), y(both) {}
    
    bool operator==(const XY &other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const XY &other) const {
        return x != other.x || y != other.y;
    }
    
    bool operator<(const XY &other) const {
        return y < other.y || (y == other.y && x < other.x);
    }
    
    XY operator*(const XY &other) const {
        return XY(x * other.x, y * other.y);
    }
    
    XY operator+(const XY &other) const {
        return XY(x + other.x, y + other.y);
    }
    
    XY operator-(const XY &other) const {
        return XY(x - other.x, y - other.y);
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
    
    int manhattanDistance() const { return abs(x) + abs(y); }
    int area() const { return abs(x * y); }
    int lengthSquared() const { return x * x + y * y; }
};

struct XYHashFunction {
public:
    size_t operator()(const XY& pos) const{
        return std::hash<int>{}(pos.x) ^ std::hash<int>{}(pos.y);
    }
};

#endif /* XY_h */
