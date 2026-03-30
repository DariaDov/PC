#include <vector>
#include <thread>
#include <functional>
#include <atomic>
#include <chrono>

#include "queue.h"
#include "task.h"

// for thread pool
std::mutex mtx; // to do

const int pool_size = 4;
const int task_min_t = 3;
const int task_max_t = 6;

class ThreadPool {
public:
    ThreadPool() = default;
    ~ThreadPool() { terminate(); }

    void initialize() {
        std::lock_guard<std::mutex> lock(mtx);

        if (initialized || terminated) {
            return;
        }

        workers.reserve(pool_size);
        for (int i = 0; i < pool_size; ++i) {
            workers.emplace_back(&ThreadPool::routine, this);
        }

        initialized = !workers.empty();
    }

    template <typename F, typename... Args>
    void add_task(F&& f, Args&&... args) {
        
    }

    void terminate() {
        
    }

private:
    std::vector<std::thread> workers;

    bool initialized = false;
    bool terminated = false;

    void routine() {
        while (true) {
            
            
            if (terminated ) {
                return;
            }
        }
    }
};