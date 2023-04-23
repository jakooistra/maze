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

// Returns the number of unique cells on the edge of a grid area defined by width and height.
inline int uniqueEdgePointCount(int width, int height) {
    if (width <= 0 || height <= 0) {
        return 0;
    } else if (width == 1 || height == 1) {
        return width + height - 1;
    } else {
        return (width + height - 2) * 2;
    }
}

// Returns a point on the edge of a grid defined as {[0, width), [0, width)}.
// The circumnavigationDistance argument returns unique values when in the
// range [0, (width + height - 2) * 2), since that is the number of unique
// edge points in a grid of size {width, height}.
inline XY getEdgePoint(int width, int height, int circumnavigationDistance) {
    int distance = circumnavigationDistance % uniqueEdgePointCount(width, height);
    if (distance < width) {
        // Left to right along the top edge.
        return XY(distance, 0);
    }
    distance -= (width - 1);
    if (distance < height) {
        // Top to bottom along the right edge.
        return XY(width - 1, distance);
    }
    distance -= height;
    if (distance < width - 1) {
        // Right to left along the bottom edge.
        return XY(width - 2 - distance, height - 1);
    }
    distance -= (width - 1);
    // Bottom to top along the left edge.
    return XY(0, height - 2 - distance);
}

// Returns a random point on the edge of an area.
template<typename random_generator>
inline XY randomEdgePoint(int width, int height, random_generator &rng) {
    return getEdgePoint(width, height, rng() % uniqueEdgePointCount(width, height));
}

// Return a StartAndFinish where the points are on the edges of the area.
template<typename random_generator>
inline StartAndFinish randomStartAndFinish(int width, int height, random_generator &rng) {
    int circumference = uniqueEdgePointCount(width, height);
    if (circumference <= 1) {
        return { XY(0, 0), XY(0, 0) };
    }
    // Ensure that start/finish are unique and guaranteed not to be too close to each other.
    int exlusionZone = circumference / 6;
    int startDistance = rng() % circumference;
    int finishDistance = (startDistance + exlusionZone + (rng() % (circumference - exlusionZone * 2))) % circumference;
    return {
        getEdgePoint(width, height, startDistance),
        getEdgePoint(width, height, finishDistance),
    };
}

// Return a StartAndFinish where the points are exactly opposite from each other.
template<typename random_generator>
inline StartAndFinish randomOppositeStartAndFinish(int width, int height, random_generator &rng) {
    int circumference = uniqueEdgePointCount(width, height);
    if (circumference <= 1) {
        return { XY(0, 0), XY(0, 0) };
    }
    // Ensure that start/finish are unique and directly opposite of each other.
    int startDistance = rng() % circumference;
    int finishDistance = (startDistance + circumference / 2) % circumference;
    return {
        getEdgePoint(width, height, startDistance),
        getEdgePoint(width, height, finishDistance),
    };
}

// Sets the start and finish points of a maze, where the points are exactly opposite from each other.
template<typename random_generator>
inline void setRandomStartAndFinish(Maze *maze, random_generator &rng) {
    auto startAndFinish = randomStartAndFinish(maze->getWidth(), maze->getHeight(), rng);
    maze->setStart(startAndFinish.start);
    maze->setFinish(startAndFinish.finish);
}

// Sets the start and finish points of a maze, where the points are exactly opposite from each other.
template<typename random_generator>
inline void setRandomOppositeStartAndFinish(Maze *maze, random_generator &rng) {
    auto startAndFinish = randomOppositeStartAndFinish(maze->getWidth(), maze->getHeight(), rng);
    maze->setStart(startAndFinish.start);
    maze->setFinish(startAndFinish.finish);
}

// Returns a vector of consecutive numbers from start to end inclusive.
inline std::vector<int> consecutiveNumbers(int start, int end) {
    end = std::max(start, end);
    std::vector<int> numbers;
    numbers.reserve(end - start + 1);
    for (int i = start; i <= end; ++i) {
        numbers.push_back(i);
    }
    return numbers;
}

// Fills a maze, guaranteeing it to be solvable. Intended to be used by
// maze algorithms for which the defined size of the maze is too small.
inline void fillDegenerateMaze(Maze *maze) {
    bool tall = maze->getWidth() < maze->getHeight();
    for (int x = 0; x < maze->getWidth(); ++x) {
        for (int y = 0; y < maze->getHeight(); ++y) {
            if (tall) {
                maze->setCell(x, y, Cell(false, x != (((y % 2) == 0) ? 0 : maze->getWidth() - 1)));
            } else {
                maze->setCell(x, y, Cell(y != (((x % 2) == 0) ? 0 : maze->getHeight() - 1), false));
            }
        }
    }
}

#endif /* Utility_h */
