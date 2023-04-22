# maze
A maze generator that started as an interview question.

## What can it do?
The app can generate mazes using different algorithms specified in the generators/types directory. 
Images for each maze can be output with or without the shortest path solution. 
To get the best or worst maze from a generator, it is possible to generate a number of mazes and then output the best or worst maze, or a ranked maze (35/500), or a percentile-ranked maze (25, 50, 75).

Maze algorithms can be analyzed, with statistics aggregated from all generated mazes.
Statistics include average shortest path length, whether the shortest path is a unique solution, and the number and length distribution of dead ends in the maze.
When a maze generator is bad, statistics can also include what percentage of mazes are solvable, how many cells are unreachable from the start point, and how many degenerate corners (fully connected 2x2 blocks) exist per maze.

## Show me some mazes!

```
maze -t all -n 1000 -o -collate
```
This command outputs the best of the first 1000 seeds of all maze generator types in a single image.
![All (1 of 1000)](https://user-images.githubusercontent.com/5326140/233759804-9e3f06a4-4f3d-4349-9285-f662fb690eeb.png)

```
maze -t all -n 1000 -op 50 -collate -p
```
This command outputs the 10th percentile best maze of the first 1000 seeds as a single image, including the solution path.
![All (100 of 1000)](https://user-images.githubusercontent.com/5326140/233760289-a3607ef6-2ff2-45e7-b685-d2ffcbc25211.png)

## How fast are these maze generators?
```
maze -t all -perf

Mazes generated per second:
          cell count:     1000     2000     4000     8000
Chain Burst                261      109       35       10
Fifty Fifty               7575     3981     2039     1029
Remove Random Walls       3349     1728      871      442
Vertical Path Breaks     27544    16181     9046     4822

Normalized rate per cell count:
          cell count:     1000     2000     4000     8000
Chain Burst               100%      82%      53%      31%
Fifty Fifty                94%      98%     100%     100%
Remove Random Walls        97%      99%      99%     100%
Vertical Path Breaks       73%      85%      94%     100%
```

## What properties are calculated during analysis?
```
maze -t rr -n 1000 -a

Remove Random Walls statistics:
  1000 mazes analyzed, 25x25
  20% solvable
  0% of solvable mazes have singular solutions
  44 average path length
  67% of cells are unreachable
  44 average degenerate corners per maze
  138 dead ends per solvable maze on average
    65-128: X
     33-64: XXXXXXXXXXXXXXXXXXX
     17-32: XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
      9-16: XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
       5-8: XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
       3-4: XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
         2: XXXXXXXXXXXXXXXXXXX
         1: XXXXXXXXXXXXXXXXXXXXXXXXX
```

## Show me everything this tool can do.
```
maze -usage

Usage:
  maze [-t type [-t type] ...] [-w width] [-h height] [-n count] [-a] [-o [rank] [-o rank] ...]
  maze [-t type [-t type] ...] [-w width] [-h height] [-n count] [-s seed] [-a] [-p] [-ww width] [-cs size] [-f file] [-o [rank] [-o rank] ...] [-op percentile [-op percentile] ...] [-usage] [-perf]
Arguments:
  -t type        Add a maze type to generate.
                   'cb' Chain Burst - Sequentially picks a recently filled location and splits the path.
                   'ff' Fifty Fifty - Every wall has a 50% chance to exist.
                   'rr' Remove Random Walls - Random walls are iteratively removed.
                   'vp' Vertical Path Breaks - Full height paths with one random opening between each.
                   'all' - (cb, ff, rr, vp)
                   'complex' - (cb)
                   'trivial' - (ff, rr, vp)
                   Can be specified multiple times.
  -w width       The width of the maze.
                   (25) if unspecified.
  -h height      The height of the maze.
                   (25) if unspecified.
  -n count       The number of each maze type to generate.
                   (1) if unspecified.
  -s seed        The seed used to generate the maze.
                   Only valid when generating a single maze
  -a             Analyze the generated mazes and output statistics.
  -p             Show the shortest path solution in image output.
  -ww width      The width of maze walls in pixels.
                   (1) if unspecified.
  -cs size       The size of maze cells in pixels.
                   (9) if unspecified.
  -f file        Specifies the base file name to use when outputting maze images.
                   Defaults to using the maze type name.
                   Only valid when a single maze type is specified.
  -o [rank]      Creates an image of the nth best maze of the ones generated (1-n).
                   If 'rank' is unspecified, outputs the best maze (1).
                   Can be specified multiple times.
  -op percentile Creates an image of the best maze in the given percentile (1-100).
                   Can be specified multiple times.
  -usage         Ignores other commands, and outputs detailed usage notes.
  -perf          Measures maze generation speed for all specified types.
```

## Want to make your own maze generator?
The following code can be added to generators/types with no additional glue code, and the new maze generator will be available for use on the command line.
This works by statically registering the generator with MazeGenerator using the REGISTER_AND_STORE_GENERATOR macro.
```
#include <random>

#include "MazeGenerator.h"
#include "Utility.h"

class NewGeneratorClass : public MazeGenerator {
public:
    NewGeneratorClass() : MazeGenerator("arg", "New Generator Name", "Description of generation technique.") {}
    virtual ~NewGeneratorClass() {}
    
protected:
    virtual std::unique_ptr<Maze> generateInternal(int width, int height, int seed) const {
        auto maze = std::make_unique<Maze>(width, height);
        
        // Be sure to use std::random or another local pseudorandom generator.
        // Note: using rand()/srand() will result in unpredictable behaviour, as generating mazes is multithreaded.
        auto rng = std::bind(std::uniform_int_distribution<>(0, INT_MAX), std::mt19937(seed));
        
        // Sets up a random start and finish point along the edges of the maze.
        setRandomStartAndFinish(maze.get(), width, height, rng);
        
        // TODO: use the Maze API to set up maze topology.
        
        return maze;
    }
};

// This line ensures that the generator class can be accessed when running the app.
REGISTER_AND_STORE_GENERATOR(NewGeneratorClass);
```
