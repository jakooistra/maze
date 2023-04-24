//
//  main.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-13.
//

#include <iostream>
#include <filesystem>
#include <set>
#include <sstream>

#include "lodepng.h"

#include "Assessment.h"
#include "ImageUtility.h"
#include "Maze.h"
#include "MazeArguments.h"
#include "MazeGenerator.h"
#include "MazeToImage.h"
#include "Performance.h"
#include "Stats.h"
#include "ThreadUtility.h"
#include "Utility.h"

struct MazeOutput {
    OutputRequest request;
    FullAssessment assessment;
    std::shared_ptr<Image const> image;
    
    MazeOutput(OutputRequest request, FullAssessment const &assessment) : request(request), assessment(assessment) {}
};

// TODO: clean up output name code (move to another file?)
static std::string getPerImageOutputFileName(OutputRequest request, MazeArguments const &args, std::string const &type) {
    std::stringstream fileNameStream;
    fileNameStream << MazeGenerator::get(type)->name;
    fileNameStream << " " << args.width << "x" << args.height;
    fileNameStream << request.fileNameMetadata(args.count()) << ".png";
    return fileNameStream.str();
}

// TODO: clean up output name code (move to another file?)
static std::string getCollatedFileName(OutputRequest request, MazeArguments const &args) {
    std::stringstream fileNameStream;
    if (args.singleFileName.has_value()) {
        fileNameStream << *args.singleFileName;
        std::filesystem::path path = *args.singleFileName;
        if (path.extension() != ".png") {
            fileNameStream << ".png";
        }
    } else {
        fileNameStream << "All";
        fileNameStream << " " << args.width << "x" << args.height;
        fileNameStream << request.fileNameMetadata(args.count()) << ".png";
    }
    return fileNameStream.str();
}

// TODO: move to image file, or image utility
static void writeMazeImageFile(std::string const &fileName, Image const *image) {
    std::vector<unsigned char> rgba;
    image->encodeRGBA(rgba);
    auto error = lodepng::encode(fileName, &rgba[0], image->width, image->height);
    if (error != 0) {
        std::cout << "ERROR: writing \"" << fileName << "\"" << std::endl;
        std::cout << "ERROR: " << lodepng_error_text(error) << std::endl;
    }
}

int main(int argc, const char * argv[]) {
    std::optional<MazeArguments> args = MazeArguments::parse(argc, argv);
    if (!args) {
        return 0;
    }
    
    // If processing takes a long time, it's good to output warnings before generating mazes.
    args->printWarnings();
    
    std::vector<MazeOutput> mazeOutput;
    if (!args->imagesToOutput.empty() || args->showAnalysis) {
        if (args->count() > 1) {
            std::cout << "Running on " << ThreadPool::shared().getThreadCount() << " threads." << std::endl;
        }
        
        if (args->types.size() > 1) {
            std::cout << "Generating " << args->types.size() << " maze types x" << args->count() << "..." <<std::endl;
        }
        int typeCount = 0;
        for (auto type : args->types) {
            auto generator = MazeGenerator::get(type);
            auto generatorTypeName = generator->name;
            typeCount++;
            if (args->types.size() > 1) {
                std::cout << "  (" << typeCount << "/" << args->types.size() << ") ";
            }
            if (args->seedsToGenerate.size() == 1) {
                std::cout << "Generating " << generatorTypeName << " " << args->width << "x" << args->height << " from seed " << args->seedsToGenerate.front() << "..." << std::endl;
            } else {
                std::cout << "Generating x" << args->count() << " " << generatorTypeName << " " << args->width << "x" << args->height << "..." << std::endl;
            }
            
            // Generate all specified seeds.
            std::function<GeneratedMaze(int)> generate = [args, generator](int seed){
                return generator->generate(args->width, args->height, seed);
            };
            std::vector<GeneratedMaze> mazes = threadedTransform(args->seedsToGenerate, generate, "Generate");
            
            // Analyze all generated mazes. Even if analysis output
            // is not specified, this is needed for ranked output.
            if (args->showAnalysis) {
                std::cout << "Analyzing x" << args->count() << " " << generatorTypeName << "..." << std::endl;
            }
            std::function<FullAssessment(GeneratedMaze const &)> assessmentFunction = valueOfMaze;
            std::vector<FullAssessment> assessments = threadedTransform(mazes, assessmentFunction, "Analyze");
            
            // Sort mazes in order from worst to best, and accumulate statistics.
            std::set<FullAssessment> sortedMazeSet;
            Stats stats;
            for (auto assessment : assessments) {
                sortedMazeSet.insert(assessment);
                stats.accumulate(assessment.analysis.get(), assessment.maze.seed);
            }
            
            // Output analysis if specified.
            if (args->showAnalysis) {
                stats.print(generatorTypeName + " statistics");
            }
            
            // Determine which mazes to convert to image data.
            int rank = 0;
            for (auto iter = sortedMazeSet.rbegin(); iter != sortedMazeSet.rend(); ++iter) {
                rank++;
                std::optional<OutputRequest> output = args->getOutputFor(iter->maze.seed, rank);
                if (output.has_value()) {
                    mazeOutput.push_back(MazeOutput(*output, *iter));
                }
            }
        }
    }
    
    // TODO: move most of the following code to a new OutputUtility.h?
    // Generate images for all output requests across multiple threads.
    std::function<MazeOutput(MazeOutput)> generateMazeImage = [args](MazeOutput output){
        auto path = args->showPath ? output.assessment.analysis->shortestPath : std::vector<XY>();
        output.image = convertToImage(output.assessment.maze.maze.get(), args->sizes, path);
        return output;
    };
    mazeOutput = threadedTransform(mazeOutput, generateMazeImage, "Image Conversion");
    
    // Sort output by request type.
    std::map<OutputRequest, std::vector<MazeOutput>> mazeOutputMap;
    for (auto output : mazeOutput) {
        mazeOutputMap[output.request].push_back(output);
    }
    
    // Output requested data.
    std::function<Image const *(MazeOutput)> getImages = [](MazeOutput output){ return output.image.get(); };
    for (auto iter : mazeOutputMap) {
        // Determine what to put in the file.
        if (iter.first.type == OutputRequest::Type::All) {
            auto collectionImage = collectImages(args->sizes, transform(iter.second, getImages));
            writeMazeImageFile(getCollatedFileName(iter.first, *args), collectionImage.get());
        } else if (args->collateAllMazeTypeImages && iter.second.size() > 1) {
            auto collatedImage = collateImages(args->sizes.border, transform(iter.second, getImages));
            writeMazeImageFile(getCollatedFileName(iter.first, *args), collatedImage.get());
        } else {
            for (auto output : iter.second) {
                writeMazeImageFile(getPerImageOutputFileName(iter.first, *args, output.assessment.maze.type), output.image.get());
            }
        }
    }
    
    // Measure performance of each type, if requested.
    if (args->measurePerformance && args->types.size() > 0) {
        Performance::print(args->types);
    }
    
    return 0;
}
