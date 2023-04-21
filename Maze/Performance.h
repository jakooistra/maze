//
//  Performance.h
//  maze
//
//  Created by John Kooistra on 2023-04-21.
//

#ifndef Performance_h
#define Performance_h

#include <chrono>
#include <map>
#include <set>
#include <string>
#include <vector>

// A measured rate of maze generation.
struct GenerationRate {
    // The number of mazes generated in the given time period.
    int count { 0 };
    
    // The number of cells per maze.
    int cellCount { 0 };
    
    // The period of time (in seconds) mazes were generated in.
    double seconds { 0 };
    
    double perSecond() const { return seconds >= 0 ? (double)count / seconds : 0.0; }
    
    // Measures performance of the given generator type, size, and minimum time bound.
    static GenerationRate measure(std::string const &type, int width, int height, double minimumTimeSeconds);
};

struct Performance {
    std::string type;
    
    // Lists measured performance rates.
    std::vector<GenerationRate> measuredRates;
    
    // Measures performance of the given generator type in absolute terms.
    static Performance measure(std::string const &type);

    // Measures performance of these types against a reference trivial generator.
    // If there is not a trivial generator included in the types, a reference trivial generator is included for comparison.
    static std::vector<Performance> measure(std::set<std::string> const &types);
    
    // Prints performance for the equivalent measure() call to the command line.
    static void print(std::set<std::string> const &types);
};

#endif /* Performance_h */
