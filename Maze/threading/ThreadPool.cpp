//
//  ThreadPool.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-16.
//

#include <chrono>
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
    currentCohort->increment();
    jobs.push({job, currentCohort});
    jobAvailableSignal.release();
}

void ThreadPool::waitForCompletion(int periodMillis, std::function<void(int)> periodicWaitFunction) {
    jobMutex.lock();
    std::shared_ptr<JobCohort> waitCohort;
    if (currentCohort->getJobCount() > 0) {
        waitCohort = currentCohort;
        currentCohort = std::make_shared<JobCohort>();
        
        // Make sure the new cohort waits for the previous cohort to finish, at the very least.
        // This ensures that concurrent calls to waitForCompletion are deterministic.
        enqueue([waitCohort]{
            waitCohort->wait();
        });
    }
    jobMutex.unlock();
    
    if (waitCohort != nullptr) {
        // If there are jobs to be waited on, and the periodMillis value is > 0,
        // the caller expects to have a function called periodically. Set up a
        // thread to do this, only if necessary.
        std::optional<std::thread> periodicCallbackThread;
        std::shared_ptr<bool> waiting;
        if (periodMillis > 0) {
            waiting = std::make_shared<bool>(true);
            periodicCallbackThread = std::thread([waitCohort, waiting, periodMillis, periodicWaitFunction](){
                while (*waiting) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(periodMillis));
                    if (*waiting) {
                        periodicWaitFunction(waitCohort->getJobCount());
                    }
                }
            });
        }
        
        // Block and wait until the job cohort has finished executing.
        waitCohort->wait();
        
        // If the callback thread was created, signal it to stop, and detach
        // the thread so that we can return immediately and let the thread
        // clean itself up when it's ready. No need to wait/join.
        if (periodicCallbackThread.has_value()) {
            *waiting = false;
            periodicCallbackThread->detach();
        }
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
            job->cohort->decrement();
            jobMutex.unlock();
        }
    }
    
    exitSignal.release();
}


void JobCohort::increment() {
    if (outstandingJobCount == 0) {
        activeMutex.lock();
    }
    outstandingJobCount++;
}

void JobCohort::decrement() {
    outstandingJobCount--;
    if (outstandingJobCount == 0) {
        activeMutex.unlock();
    }
}

void JobCohort::wait() {
    // The mutex will be unlocked when all jobs are done.
    std::lock_guard wait(activeMutex);
}
