//
//  OutputRequest.h
//  Maze
//
//  Created by John Kooistra on 2023-04-23.
//

#ifndef OutputRequest_h
#define OutputRequest_h

#include <string>
#include <sstream>

struct OutputRequest {
    enum class Type {
        All,
        Seed,
        Rank,
        Percentile,
    };
    
    Type type;
    int value;
    
    OutputRequest(Type type, int value) : type(type), value(value) {}
    
    static OutputRequest all() { return OutputRequest(Type::All, -1); }
    static OutputRequest seed(int seed) { return OutputRequest(Type::Seed, seed); }
    static OutputRequest rank(int rank) { return OutputRequest(Type::Rank, rank); }
    static OutputRequest percentile(int percentile) { return OutputRequest(Type::Percentile, percentile); }
    static OutputRequest best() { return OutputRequest(Type::Rank, 1); }
    
    static int rankFromPercentile(int percentile, int count) {
        return (percentile * count + 99) / 100;
    }
    
    bool matches(int seed, int rank, int count) {
        switch (type) {
            case Type::All:
                return true;
            case Type::Seed:
                return seed == value;
            case Type::Rank:
                return rank == value;
            case Type::Percentile:
                return (value == 1) ? (rank == 1) : (rank == rankFromPercentile(value, count));
        }
    }
    
    std::string fileNameMetadata(int count) const {
        std::stringstream metadata;
        switch (type) {
            case Type::All:
            case Type::Seed:
                metadata << " (seed " << value << ")";
                break;
            case Type::Rank:
                metadata << " (" << value << " of " << count << ")";
                break;
            case Type::Percentile:
                metadata << " (" << rankFromPercentile(value, count) << " of " << count << ")";
                break;
        }
        return metadata.str();
    }
    
    bool operator<(OutputRequest const &other) const {
        return type < other.type || (type == other.type && value < other.value);
    }
};

#endif /* OutputRequest_h */
