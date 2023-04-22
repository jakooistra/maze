//
//  GenerationRate.h
//  maze
//
//  Created by John Kooistra on 2023-04-21.
//

#ifndef GenerationRate_h
#define GenerationRate_h

#include <string>

// A measured rate of maze generation.
struct GenerationRate {
    // The number of mazes generated in the given time period.
    int count { 0 };
    
    // The number of cells per maze.
    int cellCount { 0 };
    
    // The period of time (in seconds) mazes were generated in.
    double seconds { 0 };
    
    double mazesPerSecond() const;
    double cellsPerSecond() const;
    
    // Simplifies the cell count display.
    // For instance, 999 is close enough to 1000, 4002 is close enough to 4000.
    // Simply display 1000 or 4000 and not the precise number.
    int simplifiedCellCount();
    
    // Measures performance of the given generator type, size, and minimum time bound.
    static GenerationRate measure(std::string const &type, int width, int height, double minimumTimeSeconds);
};

#endif /* GenerationRate_h */
