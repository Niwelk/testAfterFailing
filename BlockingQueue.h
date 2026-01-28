#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

template<typename T>
class BlockingQueue {
private:
    std::queue<T> queue;
    std::mutex mtx;
    std::condition_variable cv;
    bool stopped = false;

public:
    void push(const T& value) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            queue.push(value);
        }
        cv.notify_one();
    }

    std::optional<T> pop() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&] { return stopped || !queue.empty(); });

        if (queue.empty())
            return std::nullopt;

        T value = queue.front();
        queue.pop();
        return value;
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(mtx);
            stopped = true;
        }
        cv.notify_all();
    }
};