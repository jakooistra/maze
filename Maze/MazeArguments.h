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
#include "MazeOutput.h"
#include "PixelSizes.h"

struct MazeArguments {
    int width = 25;
    int height = 25;
    PixelSizes sizes;
    bool showAnalysis = false;
    bool measurePerformance = false;
    bool showPath = false;
    bool collateAllMazeTypeImages = false;
    std::optional<std::string> baseFileName;
    std::set<std::string> types;
    std::vector<int> seedsToGenerate;
    std::set<MazeOutput> imagesToOutput;
    
    static std::optional<MazeArguments> parse(int argc, const char * argv[]);
    
    int count() const;
    void printWarnings() const;
    
    std::optional<MazeOutput> getOutputFor(int seed, int rank) const;
};

#endif /* MazeArguments_h */
