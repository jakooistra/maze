//
//  Utility.h
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#ifndef Utility_h
#define Utility_h

#include "XY.h"

inline XY randomEdgePoint(int width, int height) {
    int point = rand() % ((width + height - 2) * 2);
    if (point < width) {
        return XY(point, 0);
    } else if (point < width * 2) {
        return XY(point - width, height - 1);
    } else if (point < width * 2 + height - 2) {
        return XY(0, point - width * 2 + 1);
    } else {
        return XY(width - 1, point - width * 2 - height + 3);
    }
}

#endif /* Utility_h */
