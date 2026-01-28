#include <iostream>
#include <chrono>
#include "ThreadPool.h"

int main() {
    int numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 4;

    std::vector<std::string> files = {
        "file_test_1.txt",
        "file_test_2.txt",
        "file_test_3.txt"
    };

    auto start = std::chrono::high_resolution_clock::now();

    ThreadPool pool(numThreads);
    pool.startAggregator();
    pool.startProducer(files);
    pool.wait();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "\nResult:" << std::endl;

    std::cout << "Total lines: " << pool.getTotalLines() << std::endl;
    std::cout << "Files processed: " << pool.getProcessedFiles() << std::endl;
    std::cout << "Threads used: " << pool.getThreadCount() << std::endl;
    std::cout << "Time: " << duration.count() << " (ms)" << std::endl;

    return 0;
}