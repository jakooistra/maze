//
//  ThreadUtility.h
//  Maze
//
//  Created by John Kooistra on 2023-04-16.
//

#ifndef ThreadUtility_h
#define ThreadUtility_h

#include <functional>
#include <vector>
#include <mutex>

#include "ThreadPool.h"

template<typename Input, typename Output>
inline std::vector<Output> threadedTransform(std::vector<Input> const &input, std::function<Output(Input)> transform, ThreadPool &pool = ThreadPool::shared()) {
    std::mutex outputMutex;
    std::vector<Output> output;
    
    // Executes as many transform operations as possible in parallel on all input data.
    for (auto data : input) {
        pool.enqueue([data, &transform, &outputMutex, &output]{
            auto transformedData = transform(data);
            
            // Lock the output vector when outputting data.
            std::lock_guard guard(outputMutex);
            output.push_back(transformedData);
        });
    }
    
    // Wait for this cohort of jobs to complete, guaranteeing that all output has been collected.
    pool.waitForCompletion();
    
    return output;
}

#endif /* ThreadUtility_h */
