  #pragma once
#include "BlockingQueue.h"
#include <thread>
#include <vector>
#include <atomic>
#include <string>

struct Task {
    std::string filename;
};

class ThreadPool {
public:
    ThreadPool(size_t workerCount);
    ~ThreadPool();

    void startProducer(const std::vector<std::string>& files);
    void startAggregator();
    void wait();

    size_t getTotalLines() const { return totalLines.load(); }
    size_t getProcessedFiles() const { return processedFiles.load(); }
    size_t getThreadCount() const { return workers.size(); }

private:
    void workerThread();
    void aggregatorThread();

    BlockingQueue<Task> taskQueue;
    BlockingQueue<int> resultQueue;
    std::vector<std::thread> workers;
    std::thread producer;
    std::thread aggregator;

    std::atomic<size_t> activeWorkers;
    std::atomic<bool> producerFinished;

    std::atomic<size_t> totalLines{0};
    std::atomic<size_t> processedFiles{0};
};