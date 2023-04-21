//
//  FiftyFiftyGenerator.h
//  maze
//
//  Created by John Kooistra on 2023-04-16.
//

#ifndef FiftyFiftyGenerator_h
#define FiftyFiftyGenerator_h

#include "MazeGenerator.h"

// Generates a maze by giving each wall segment a 50/50 chance to exist.
//
// Properties: highly variable, not guaranteed to be solvable, lots of
// unreachable areas, very quick to generate.
class FiftyFiftyGenerator : public MazeGenerator {
public:
    FiftyFiftyGenerator()
    :   MazeGenerator("ff",
                      "Fifty Fifty",
                      "Every wall has a 50% chance to exist.",
                      MazeQuality::Trivial) {}
    
    virtual ~FiftyFiftyGenerator() {}
    
protected:
    virtual std::unique_ptr<Maze> generateInternal(int width, int height, int seed) const;
};

#endif /* FiftyFiftyGenerator_h */
