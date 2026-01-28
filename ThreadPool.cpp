#include "ThreadPool.h"
#include <fstream>
#include <iostream>
#include <thread>

ThreadPool::ThreadPool(size_t workerCount)
    : activeWorkers(workerCount), producerFinished(false)
{
    for (size_t i = 0; i < workerCount; ++i) {
        workers.emplace_back(&ThreadPool::workerThread, this);
    }
}

ThreadPool::~ThreadPool() {
    wait();
}

void ThreadPool::startProducer(const std::vector<std::string>& files) {
    producer = std::thread([this, files]() {
        for (const auto& file : files) {
            taskQueue.push(Task{file});
        }
        taskQueue.stop();
    });
}

void ThreadPool::startAggregator() {
    aggregator = std::thread(&ThreadPool::aggregatorThread, this);
}

void ThreadPool::workerThread() {
    while (true) {
        auto taskOpt = taskQueue.pop();
        if (!taskOpt.has_value())
            break;

        std::ifstream file(taskOpt->filename);
        if (!file.is_open()) {
            continue;
        }

        int lines = 0;
        std::string line;
        while (std::getline(file, line)) {
            ++lines;
        }

        resultQueue.push(lines);
        ++processedFiles;
    }

    if (--activeWorkers == 0) {
        resultQueue.stop();
    }
}

void ThreadPool::aggregatorThread() {
    while (true) {
        auto result = resultQueue.pop();
        if (!result.has_value())
            break;

        totalLines += *result;
    }
}

void ThreadPool::wait() {
    if (producer.joinable()) producer.join();

    for (auto& w : workers) {
        if (w.joinable()) w.join();
    }

    if (aggregator.joinable()) aggregator.join();
}