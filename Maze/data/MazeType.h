//
//  MazeType.h
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#ifndef MazeType_h
#define MazeType_h

#include <string>
#include <vector>

enum class MazeType {
    ChainBurst,
    FiftyFifty,
    RemoveRandomWalls,
    VerticalPathBreaks,
    
    // TODO: random perturbation generator as described briefly in the interview
};

inline std::vector<MazeType> allMazeTypes() {
    return {
        MazeType::ChainBurst,
        MazeType::FiftyFifty,
        MazeType::RemoveRandomWalls,
        MazeType::VerticalPathBreaks,
    };
}

#endif /* MazeType_h */
