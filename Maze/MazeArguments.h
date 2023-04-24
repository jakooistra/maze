//
//  MazeArguments.h
//  maze
//
//  Created by John Kooistra on 2023-04-20.
//

#ifndef MazeArguments_h
#define MazeArguments_h

#include <optional>
#include <set>
#include <string>

#include "MazeGenerator.h"
#include "OutputRequest.h"
#include "PixelSizes.h"

struct MazeArguments {
    int width = 25;
    int height = 25;
    PixelSizes sizes;
    bool showAnalysis = false;
    bool measurePerformance = false;
    bool showPath = false;
    bool collateAllMazeTypeImages = false;
    std::optional<std::string> singleFileName;
    std::set<std::string> types;
    std::vector<int> seedsToGenerate;
    std::set<OutputRequest> imagesToOutput;
    
    static std::optional<MazeArguments> parse(int argc, const char * argv[]);
    
    int count() const;
    void printWarnings() const;
    
    std::optional<OutputRequest> getOutputFor(int seed, int rank) const;
    
    // Returns an error string if validation fails.
    std::optional<std::string> validateArguments() const;
};

#endif /* MazeArguments_h */
