//
//  ThreadPool.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-16.
//

#include <optional>

#include "ThreadPool.h"

ThreadPool::ThreadPool(int threadCount)
:   jobAvailableSignal(0)
,   exiting(false)
,   exitSignal(0)
,   currentCohort(std::make_shared<JobCohort>())
{
    // Ensure there's at least one thread available to execute the job queue.
    threadCount = std::max(1, threadCount);
    for (int i = 0; i < threadCount; ++i) {
        threads.push_back(std::thread(&ThreadPool::threadFunction, this));
    }
}

ThreadPool::~ThreadPool() {
    exiting = true;
    // Signal that there are jobs available, one per thread.
    // This will wake up threads, each of which will notice exiting = true and exit.
    for (int i = 0; i < threads.size(); ++i) {
        jobAvailableSignal.release();
    }
    // Wait for each thread to exit. The exitSignal should be released once per thread.
    for (int i = 0; i < threads.size(); ++i) {
        exitSignal.acquire();
    }
    for (auto &thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void ThreadPool::enqueue(std::function<void()> job) {
    std::lock_guard guard(jobMutex);
    if (currentCohort->outstandingJobCount == 0) {
        currentCohort->activeMutex.lock();
    }
    currentCohort->outstandingJobCount++;
    jobs.push({job, currentCohort});
    jobAvailableSignal.release();
}

void ThreadPool::waitForCompletion() {
    jobMutex.lock();
    std::shared_ptr<JobCohort> waitCohort;
    if (currentCohort->outstandingJobCount > 0) {
        waitCohort = currentCohort;
        currentCohort = std::make_shared<JobCohort>();
        
        // Make sure the new cohort waits for the previous cohort to finish, at the very least.
        // This ensures that concurrent calls to waitForCompletion are deterministic.
        enqueue([waitCohort]{
            std::lock_guard wait(waitCohort->activeMutex);
        });
    }
    jobMutex.unlock();
    
    if (waitCohort != nullptr) {
        // The mutex will be unlocked when all jobs are done.
        std::lock_guard wait(waitCohort->activeMutex);
    }
}

int ThreadPool::getThreadCount() const {
    return (int)threads.size();
}

ThreadPool &ThreadPool::shared() {
    static ThreadPool s_sharedThreadPool;
    return s_sharedThreadPool;
}

void ThreadPool::threadFunction() {
    while (!exiting) {
        jobAvailableSignal.acquire();
        
        std::optional<JobWithCohort> job;
        
        jobMutex.lock();
        if (!jobs.empty()) {
            job = jobs.front();
            jobs.pop();
        }
        jobMutex.unlock();
        
        if (job.has_value()) {
            // Execute the job.
            job->job();
            
            // Check and possibly release the cohort
            jobMutex.lock();
            job->cohort->outstandingJobCount--;
            if (job->cohort->outstandingJobCount == 0) {
                job->cohort->activeMutex.unlock();
            }
            jobMutex.unlock();
        }
    }
    
    exitSignal.release();
}
