//
//  MazeOutput.h
//  Maze
//
//  Created by John Kooistra on 2023-04-23.
//

#ifndef MazeOutput_h
#define MazeOutput_h

#include <string>
#include <sstream>

struct MazeOutput {
    enum class Type {
        All,
        Seed,
        Rank,
        Percentile,
    };
    
    Type type;
    int value;
    
    MazeOutput(Type type, int value) : type(type), value(value) {}
    
    static MazeOutput all() { return MazeOutput(Type::All, -1); }
    static MazeOutput seed(int seed) { return MazeOutput(Type::Seed, seed); }
    static MazeOutput rank(int rank) { return MazeOutput(Type::Rank, rank); }
    static MazeOutput percentile(int percentile) { return MazeOutput(Type::Percentile, percentile); }
    static MazeOutput best() { return MazeOutput(Type::Rank, 1); }
    
    bool matches(int seed, int rank, int count) {
        switch (type) {
            case Type::All:
                return true;
            case Type::Seed:
                return seed == value;
            case Type::Rank:
                return rank == value;
            case Type::Percentile:
                return (value == 1) ? (rank == 1) : (rank == (value * count + 99) / 100);
        }
    }
    
    std::string fileNameMetadata(int seed, int rank, int count) const {
        std::stringstream metadata;
        switch (type) {
            case Type::All:
            case Type::Seed:
                metadata << " (seed " << seed << ")";
                break;
            case Type::Rank:
            case Type::Percentile:
                metadata << " (" << rank << " of " << count << ")";
                break;
        }
        return metadata.str();
    }
    
    bool operator<(MazeOutput const &other) const {
        return type < other.type || (type == other.type && value < other.value);
    }
};

#endif /* MazeOutput_h */
