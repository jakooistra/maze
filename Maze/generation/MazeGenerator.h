//
//  MazeGenerator.h
//  Maze
//
//  Created by John Kooistra on 2023-04-14.
//

#ifndef MazeGenerator_h
#define MazeGenerator_h

#include <memory>

#include "GeneratedMaze.h"
#include "Maze.h"
#include "MazeQuality.h"

struct MazeGeneratorRegistry;

class MazeGenerator {
private:
    // TODO: expose these as const properties rather than using accessors.
    
    // Lower case short string with no spaces identifying this generator type. Used on the command line.
    std::string type;
    
    // The human-readable name of this generator.
    std::string name;
    
    // A short description of what this generator does, used on the command line.
    std::string description;
    
    // A rough quality type of this generator: trivial, complex, or any other defined value.
    MazeQuality quality;
    
protected:
    MazeGenerator(std::string const &type, std::string const &name, std::string const &description, MazeQuality quality);
    
    // Implement this function in a subclass to allow maze generation.
    virtual std::unique_ptr<Maze> generateInternal(int width, int height, int seed) const = 0;
    
public:
    virtual ~MazeGenerator() {}
    
    // Generate a maze with the given parameters.
    GeneratedMaze generate(int width, int height, int seed) const;
    
    // Metadata for this maze generator.
    std::string const &getType() const { return type; }
    std::string const &getName() const { return name; }
    std::string const &getDescription() const { return description; }
    MazeQuality getQuality() const { return quality; }
    
    // A global registry for maze generators, populated by distributed generator files.
    static std::shared_ptr<MazeGenerator const> registerGenerator(std::shared_ptr<MazeGenerator const> generator);
    static std::shared_ptr<MazeGenerator const> get(std::string const &type);
    static std::vector<std::shared_ptr<MazeGenerator const>> all();
    static std::vector<std::shared_ptr<MazeGenerator const>> allOfQuality(MazeQuality quality);
    
private:
    static MazeGeneratorRegistry *getRegistry();
};

// A convenience macro for registering and storing a given generator with the global registry.
#define REGISTER_AND_STORE_GENERATOR(GeneratorClassName) \
    static auto s_##GeneratorClassName = MazeGenerator::registerGenerator(std::make_shared<GeneratorClassName>());

#endif /* MazeGenerator_h */
