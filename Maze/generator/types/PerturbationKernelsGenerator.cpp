//
//  PerturbationKernelsGenerator.cpp
//  maze
//
//  Created by John Kooistra on 2023-04-22.
//

#include <iostream>

#include <mutex>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "MazeGenerator.h"
#include "Utility.h"

// Empty namespace to keep these definitions local.
namespace {
    struct PKCell {
        // Wall state for this cell.
        Cell cell;
        
        // Whether the cell is traversible.
        bool populated { false };
    };

    // Represents the connectivity and state of a corner of a 2x2 maze area.
    struct CornerState {
        // This bitmask contains three bits:
        // [0] - if the cell is populated
        // [1] - if there is an external path to this cell (open wall on either edge)
        // [2] - if the clockwise-wound wall exists
        unsigned int bitmask;
        
        CornerState(unsigned int bitmask = 0b111) : bitmask(bitmask & 0x7) {}
        
        CornerState(bool populated, bool externalAccess, bool clockwiseWall)
        :   bitmask((populated ? 1 : 0) | (externalAccess ? 2 : 0) | (clockwiseWall ? 4 : 0)) {}
        
        bool populated() const {
            return bitmask & 1;
        }
        
        bool externalPath() const {
            return bitmask & 2;
        }
        
        bool clockwiseWall() const {
            return bitmask & 4;
        }
    };
    
    struct GridState {
        // The bitmask represents the relevant state of walls and 
        unsigned int bitmask;
        
        GridState(unsigned int bitmask) : bitmask(bitmask & 0xFFF) {}
        
        GridState(CornerState tl, CornerState tr, CornerState br, CornerState bl)
        :   bitmask(tl.bitmask | (tr.bitmask << 3) | (br.bitmask << 6) | (bl.bitmask << 9)) {}
        
        CornerState tl() const { return CornerState(bitmask & 0x7); }
        CornerState tr() const { return CornerState((bitmask >> 3) & 0x7); }
        CornerState br() const { return CornerState((bitmask >> 6) & 0x7); }
        CornerState bl() const { return CornerState((bitmask >> 9) & 0x7); }
        
        // Applies the internal state to cells.
        // Note: external connectivity is unchanged, "externalPath" is unused.
        void apply(std::vector<std::vector<PKCell>> &cells, int lx, int ty) const {
            auto tlState = tl();
            auto trState = tr();
            auto brState = br();
            auto blState = bl();
            cells[lx][ty].populated = tlState.populated();
            cells[lx+1][ty].populated = trState.populated();
            cells[lx+1][ty+1].populated = brState.populated();
            cells[lx][ty+1].populated = blState.populated();
            cells[lx+1][ty].cell.leftWall = tlState.clockwiseWall();
            cells[lx+1][ty+1].cell.topWall = trState.clockwiseWall();
            cells[lx+1][ty+1].cell.leftWall = brState.clockwiseWall();
            cells[lx][ty+1].cell.topWall = blState.clockwiseWall();
        }
        
        GridState rotated() const {
            return GridState((bitmask << 3) | (bitmask >> 9));
        }
        
        static GridState from(std::vector<std::vector<PKCell>> const &cells, Maze const *maze, int lx, int ty) {
            auto c00 = cells[lx][ty];
            auto c10 = cells[lx+1][ty];
            auto c20 = cells[lx+2][ty];
            auto c01 = cells[lx][ty+1];
            auto c11 = cells[lx+1][ty+1];
            auto c21 = cells[lx+2][ty+1];
            auto c02 = cells[lx][ty+2];
            auto c12 = cells[lx+1][ty+2];
            return GridState(CornerState(c00.populated, !c00.cell.leftWall || !c00.cell.topWall || maze->isStartOrFinish(lx, ty), c10.cell.leftWall),
                             CornerState(c10.populated, !c10.cell.topWall || !c20.cell.leftWall || maze->isStartOrFinish(lx+1, ty), c11.cell.topWall),
                             CornerState(c11.populated, !c21.cell.leftWall || !c12.cell.topWall || maze->isStartOrFinish(lx+1, ty+1), c11.cell.leftWall),
                             CornerState(c01.populated, !c01.cell.leftWall || !c02.cell.topWall || maze->isStartOrFinish(lx, ty+1), c01.cell.topWall));
        }
        
        bool operator==(GridState const &other) const {
            return bitmask == other.bitmask;
        }
    };

    struct GridStateHashFunction {
    public:
        size_t operator()(const GridState& gridState) const{
            return gridState.bitmask;
        }
    };

    struct TransformKernel2x2 {
        GridState before;
        GridState after;
        
        TransformKernel2x2(GridState from, GridState to) : before(from), after(to) {}
        TransformKernel2x2(TransformKernel2x2 const &other) : before(other.before), after(other.after) {}
        
        TransformKernel2x2 rotated() const {
            return TransformKernel2x2(before.rotated(), after.rotated());
        }
        
        bool operator==(TransformKernel2x2 const &other) const {
            return before == other.before && after == other.after;
        }
    };

    enum class Connectivity {
        External,   // Cell has external pathways.
        Present,    // Cell is populated but has no external pathways.
        Empty,      // Cell not populated or don't care.
    };

    struct GridDescription {
        Connectivity path[4];  // tl, tr, br, bl - 2x2 points wound clockwise.
        
        GridDescription(Connectivity tl,
                        Connectivity tr = Connectivity::Empty,
                        Connectivity br = Connectivity::Empty,
                        Connectivity bl = Connectivity::Empty)
        :   path{tl, tr, br, bl}
        {
        }
        
        GridState state() const {
            CornerState corner[4];
            bool lastCellPopulated = (path[0] != Connectivity::Empty) && (path[1] == Connectivity::Empty || path[2] == Connectivity::Empty);
            for (int i = 3; i >= 0; --i) {
                bool cellPopulated = path[i] != Connectivity::Empty;
                corner[i] = CornerState(cellPopulated, path[i] == Connectivity::External, !lastCellPopulated || !cellPopulated);
                lastCellPopulated = cellPopulated;
            }
            return GridState(corner[0], corner[1], corner[2], corner[3]);
        }
        
        GridDescription withNoExternalRequirements() const {
            return GridDescription(path[0] == Connectivity::External ? Connectivity::Present : path[0],
                                   path[1] == Connectivity::External ? Connectivity::Present : path[1],
                                   path[2] == Connectivity::External ? Connectivity::Present : path[2],
                                   path[3] == Connectivity::External ? Connectivity::Present : path[3]);
        }
    };

    static GridDescription const kGridPoint(Connectivity::External);
    static GridDescription const kGridRight(Connectivity::External, Connectivity::External); // ─
    static GridDescription const kGridRightHook(Connectivity::External, Connectivity::Present, Connectivity::External); // ┐
    static GridDescription const kGridLeftHook(Connectivity::External, Connectivity::Empty, Connectivity::External, Connectivity::Present); // └
    static GridDescription const kGridDown(Connectivity::External, Connectivity::Empty, Connectivity::Empty, Connectivity::External); // │
    static GridDescription const kGridRightBend(Connectivity::External, Connectivity::Present, Connectivity::Present, Connectivity::External);

    struct PrecomputedTransforms {
        // Given a 2x2 "before" grid state, lists the possible 2x2 "after" grid states.
        std::unordered_map<GridState, std::vector<GridState>, GridStateHashFunction> transform;
        
        static std::shared_ptr<PrecomputedTransforms const> shared() {
            static std::mutex s_mutex;
            static std::shared_ptr<PrecomputedTransforms> s_lookup;
            
            std::lock_guard guard(s_mutex);
            if (s_lookup == nullptr) {
                s_lookup = std::make_shared<PrecomputedTransforms>();
                
                // TODO: define complete set of transform kernels
                
                s_lookup->addTransform(kGridPoint, kGridRight);
                s_lookup->addTransform(kGridPoint, kGridDown);
                s_lookup->addTransform(kGridPoint, kGridRightHook);
                s_lookup->addTransform(kGridPoint, kGridLeftHook);
                
                s_lookup->addTransform(kGridDown.withNoExternalRequirements(), kGridRightBend, 4);
                
                s_lookup->addTransform(kGridRightHook.withNoExternalRequirements(), kGridRightBend, 5);
                
                std::cout << "Total transforms precalculated: " << s_lookup->totalTransforms() << std::endl;
            }
            
            return s_lookup;
        }
        
        int totalTransforms() const {
            int total = 0;
            for (auto tuple : transform) {
                total += (int)tuple.second.size();
            }
            return total;
        }
        
        void addTransform(GridDescription before, GridDescription after, int weight = 1) {
            bool cellsThatDontMatter[4];
            for (int i = 0; i < 4; ++i) {
                cellsThatDontMatter[i] = (before.path[i] == Connectivity::Empty) && (after.path[i] == Connectivity::Empty);
            }
            bool wallsThatDontMatter[4];
            for (int i = 0; i < 4; ++i) {
                wallsThatDontMatter[i] = cellsThatDontMatter[i] && cellsThatDontMatter[(i+1)&3];
            }
            std::vector<unsigned int> bitsThatDontMatter;
            unsigned int cellBaseFlag = 1;
            for (int i = 0; i < 4; ++i) {
                // Population bit doesn't matter if the cell doesn't matter.
                if (cellsThatDontMatter[i]) {
                    bitsThatDontMatter.push_back(cellBaseFlag);
                }
                // External connection bit doesn't matter if the cell doesn't matter or if the cell matters but external connection was not specified.
                if (cellsThatDontMatter[i] || (before.path[i] != Connectivity::External)) {
                    bitsThatDontMatter.push_back(cellBaseFlag << 1);
                }
                if (wallsThatDontMatter[i]) {
                    bitsThatDontMatter.push_back(cellBaseFlag << 2);
                }
                cellBaseFlag = cellBaseFlag << 3;
            }
            unsigned int bitmaskThatDoesntMatter = 0;
            for (auto bit : bitsThatDontMatter) {
                bitmaskThatDoesntMatter = bitmaskThatDoesntMatter | bit;
            }
            
            std::vector<TransformKernel2x2> validTransforms {
                TransformKernel2x2(before.state().bitmask & ~bitmaskThatDoesntMatter, after.state().bitmask & ~bitmaskThatDoesntMatter)
            };
            for (auto bit : bitsThatDontMatter) {
                for (int i = (int)validTransforms.size() - 1; i >= 0; --i) {
                    auto const &transform = validTransforms[i];
                    validTransforms.push_back(TransformKernel2x2(transform.before.bitmask | bit, transform.after.bitmask | bit));
                }
            }
            
            for (auto const &transform : validTransforms) {
                addTransform(transform, weight);
            }
        }
        
        void addTransform(TransformKernel2x2 k, int weight = 1) {
            auto &list = transform[k.before];
            list.reserve(list.size() + 4 * weight);
            for (int count = 0; count < weight; ++count) {
                list.push_back(k.after);
            }
            for (int i = 0; i < 3; ++i) {
                k = k.rotated();
                auto &list = transform[k.before];
                for (int count = 0; count < weight; ++count) {
                    list.push_back(k.after);
                }
            }
        }
    };

    class ConsideredLocations {
    private:
        int width;
        int height;
        std::unordered_set<XY, XYHashFunction> unique;
        std::vector<XY> list;
        
    public:
        ConsideredLocations(int width, int height) : width(width), height(height) {}
        
        bool empty() const {
            return unique.empty();
        }
        
        void reconsiderAfterApplyingKernel(XY tl, std::vector<std::vector<PKCell>> const &cells) {
            int left = std::max(tl.x - 1, 0);
            int right = std::min(tl.x + 2, width - 1);
            int top = std::max(tl.y - 1, 0);
            int bottom = std::min(tl.y + 2, height - 1);
            for (int x = left; x <= right; ++x) {
                for (int y = top; y <= bottom; ++y) {
                    if (cells[x][y].populated || cells[x+1][y].populated || cells[x][y+1].populated || cells[x+1][y+1].populated) {
                        push({x,y});
                    }
                }
            }
        }
        
        void push(XY pos) {
            if (unique.find(pos) == unique.end()) {
                unique.insert(pos);
                list.push_back(pos);
            }
        }
        
        template<typename random_generator>
        XY pop(random_generator &rng) {
            if (unique.empty()) {
                return { 0, 0 };
            }
            int index = rng() % list.size();
            XY value = list[index];
            list[index] = list.back();
            list.pop_back();
            unique.erase(value);
            return value;
        }
    };
}

class PerturbationKernelsGenerator : public MazeGenerator {
public:
    PerturbationKernelsGenerator()
    :   MazeGenerator("pk",
                      "Perturbation Kernels",
                      "Applies 2x2 kernel transformations until the maze is filled.")
    {
    }
    
    virtual ~PerturbationKernelsGenerator() {}
    
protected:
    virtual std::unique_ptr<Maze> generateInternal(int width, int height, int seed) const {
        auto maze = std::make_unique<Maze>(width, height);
        auto lookup = PrecomputedTransforms::shared();
        
        // Be sure to use std::random or another local pseudorandom generator.
        // Note: using rand()/srand() will result in unpredictable behaviour, as generating mazes is multithreaded.
        auto rng = std::bind(std::uniform_int_distribution<>(0, INT_MAX), std::mt19937(seed));
        
        // Sets up a random start and finish point along the edges of the maze.
        setRandomOppositeStartAndFinish(maze.get(), rng);
        
        // If the maze is smaller than 2x2, no kernel can apply to it and this is a degenerate case that needs special handling.
        if (width < 2 || height < 2) {
            fillDegenerateMaze(maze.get());
            return maze;
        }
        
        std::vector<std::vector<PKCell>> cells;
        cells.resize(maze->getWidth() + 1);
        for (int x = 0; x <= maze->getWidth(); ++x) {
            cells[x].resize(maze->getHeight() + 1);
        }
        cells[maze->getStart().x][maze->getStart().y].populated = true;
        ConsideredLocations locations(width - 1, height - 1);
        locations.reconsiderAfterApplyingKernel(maze->getStart(), cells);
        
        while (!locations.empty()) {
            auto pos = locations.pop(rng);
            auto state = GridState::from(cells, maze.get(), pos.x, pos.y);
            auto applicableTransformIter = lookup->transform.find(state);
            if (applicableTransformIter != lookup->transform.end()) {
                auto const &applicableTransforms = applicableTransformIter->second;
                auto const &transform = applicableTransforms[rng() % applicableTransforms.size()];
                transform.apply(cells, pos.x, pos.y);
                locations.reconsiderAfterApplyingKernel(pos, cells);
            }
        }
        
        // Write the contents of the computed PKCells out to the maze object.
        for (int x = 0; x < maze->getWidth(); ++x) {
            for (int y = 0; y < maze->getHeight(); ++y) {
                maze->setCell(x, y, cells[x][y].cell);
            }
        }
        
        return maze;
    }
};

// This line ensures that the generator class can be accessed when running the app.
REGISTER_AND_STORE_GENERATOR(PerturbationKernelsGenerator);
