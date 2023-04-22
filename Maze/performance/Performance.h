//
//  Performance.h
//  maze
//
//  Created by John Kooistra on 2023-04-21.
//

#ifndef Performance_h
#define Performance_h

#include <set>
#include <string>
#include <vector>

#include "GenerationRate.h"

struct Performance {
public:
    std::string type;
    
    // Lists measured performance rates.
    std::vector<GenerationRate> measuredRates;
    
public:
    double mostCellsPerSecond() const;
    
    // Measures performance of the given generator type.
    static Performance measure(std::string const &type, double seconds = 1.0);

    // Measures performance of the given generator types.
    static std::vector<Performance> measure(std::set<std::string> const &types, double secondsPerType = 1.0);
    
    // Prints performance for the equivalent measure(types) call to the command line.
    static void print(std::set<std::string> const &types, double secondsPerType = 1.0);
};

#endif /* Performance_h */
