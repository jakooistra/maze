//
//  ThreadPool.h
//  Maze
//
//  Created by John Kooistra on 2023-04-16.
//

#ifndef ThreadPool_h
#define ThreadPool_h

#include <functional>
#include <mutex>
#include <queue>
#include <semaphore>
#include <thread>
#include <vector>

class JobCohort {
private:
    // When a job is enqueued, this is incremented, and when it is finished it will be decremented.
    int outstandingJobCount { 0 };
    
    // Will be locked as soon as outstandingJobCount > 0, unlocked when outstandingJobCount = 0.
    std::mutex activeMutex;
    
public:
    virtual ~JobCohort() {}
    
    // Signal that a job has been added.
    void increment();
    
    // Signal that a job has been completed.
    void decrement();
    
    // Block and wait until the outstanding job count is 0.
    void wait();
    
    inline int getJobCount() const { return outstandingJobCount; }
};

struct JobWithCohort {
    std::function<void()> job;
    std::shared_ptr<JobCohort> cohort;
};

// A pool of threads waiting to execute jobs.
// Ensures at least one thread is allocated and waiting.
class ThreadPool {
private:
    std::vector<std::thread> threads;
    std::queue<JobWithCohort> jobs;
    std::shared_ptr<JobCohort> currentCohort;
    
    // Each enqueued job signals this semaphore to wake up a waiting thread to execute the job.
    std::counting_semaphore<> jobAvailableSignal;
    
    // When dequeuing jobs, the mutex is used to make queue access safe.
    std::recursive_mutex jobMutex;
    
    // Whether the threadpool is exiting. Set to true once in the destructor.
    bool exiting;
    
    // Released once by each thread on exit, allowing the destructor to join each thread.
    std::counting_semaphore<> exitSignal;
    
public:
    ThreadPool(int threadCount = std::thread::hardware_concurrency());
    virtual ~ThreadPool();
    
    // Adds the job to the queue of jobs to be executed.
    void enqueue(std::function<void()> job);
    
    // Waits for all jobs to be completed.
    // Every periodMillis interval, call periodicWaitFunction with the number of jobs remaining.
    // If periodMillis < 0, the function is not called.
    void waitForCompletion(int periodMillis, std::function<void(int)> periodicWaitFunction);
    void waitForCompletion() { waitForCompletion(-1, {}); }
    
    int getThreadCount() const;
    
    // Return the shared instance of ThreadPool. Unless jobs need to be
    // isolated, it is preferable to use the shared ThreadPool.
    static ThreadPool &shared();
    
private:
    // The function that threads execute.
    void threadFunction();
};

#endif /* ThreadPool_h */
