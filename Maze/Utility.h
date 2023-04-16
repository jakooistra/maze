//
//  Utility.h
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#ifndef Utility_h
#define Utility_h

#include "XY.h"
#include "Maze.h"

struct StartAndFinish {
    XY start;
    XY finish;
};

inline int uniqueEdgePointCount(int width, int height) {
    return (width > 0 && height > 0) ? (width + height - 2) * 2 : 0;
}

// Returns a point on the edge of a grid defined as {[0, width), [0, width)}.
// The circumnavigationDistance argument returns unique values when in the
// range [0, (width + height - 2) * 2), since that is the number of unique
// edge points in a grid of size {width, height}.
inline XY getEdgePoint(int width, int height, int circumnavigationDistance) {
    if (circumnavigationDistance < width) {
        return XY(circumnavigationDistance, 0);
    } else if (circumnavigationDistance < width * 2) {
        return XY(circumnavigationDistance - width, height - 1);
    } else if (circumnavigationDistance < width * 2 + height - 2) {
        return XY(0, circumnavigationDistance - width * 2 + 1);
    } else {
        return XY(width - 1, circumnavigationDistance - width * 2 - height + 3);
    }
}

template<typename random_generator>
inline XY randomEdgePoint(int width, int height, random_generator &rng) {
    return getEdgePoint(width, height, rng() % uniqueEdgePointCount(width, height));
}

template<typename random_generator>
inline StartAndFinish randomStartAndFinish(int width, int height, random_generator &rng) {
    int circumference = uniqueEdgePointCount(width, height);
    if (circumference <= 1) {
        return { XY(0, 0), XY(0, 0) };
    }
    // Ensure that start/finish are unique and outside of a reasonable exclusion zone.
    int exlusionZone = std::min(10, (circumference - 1) / 2);
    int startDistance = rng() % circumference;
    int finishDistance = rng() % (circumference - exlusionZone * 2);
    int distance = std::abs((startDistance - finishDistance + circumference + circumference / 2) % circumference - circumference / 2);
    if (distance < exlusionZone) {
        finishDistance = circumference - exlusionZone;
    }
    return {
        getEdgePoint(width, height, startDistance),
        getEdgePoint(width, height, finishDistance),
    };
}

template<typename random_generator>
inline void setRandomStartAndFinish(Maze *maze, int width, int height, random_generator &rng) {
    auto startAndFinish = randomStartAndFinish(width, height, rng);
    maze->setStart(startAndFinish.start);
    maze->setFinish(startAndFinish.finish);
}

inline std::vector<int> consecutiveNumbers(int start, int end) {
    end = std::max(start, end);
    std::vector<int> numbers;
    numbers.reserve(end - start + 1);
    for (int i = start; i <= end; ++i) {
        numbers.push_back(i);
    }
    return numbers;
}

#endif /* Utility_h */
