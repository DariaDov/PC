#include <vector>
#include <thread>
#include <functional>
#include <atomic>
#include <chrono>
#include <condition_variable>

#include "queue.h"
#include "task.h"

const int pool_size = 4;

// std::shared_lock lock(mtx);
// std::unique_lock lock(mtx);

class ThreadPool {
public:
    ThreadPool() = default;
    ~ThreadPool() { terminate(); }

    void initialize(int pool_size) {
        std::unique_lock lock(mtx);

        if (initialized || terminated) return;

        workers.reserve(pool_size);
        for (int i = 0; i < pool_size; ++i) {
            workers.emplace_back(&ThreadPool::routine, this);
        }

        initialized = !workers.empty();
    }

    template <typename F>
    void enqueue(F task) {
        {
            std::shared_lock lock(mtx);
            if (!working_unsafe()) {
                return;
            }
        }

        tasks.emplace(task);
        condition.notify_one();
    }

    void terminate() {
        {
            std::unique_lock lock(mtx);
            if (working_unsafe()) {
                terminated = true;
                force_terminate = true;
            } else {
                workers.clear();
                terminated = false;
                initialized = false;
                return;
            }
        }

        condition.notify_all();
        for (std::thread& worker : workers) {
            worker.join();
        }

        workers.clear();
        terminated = false;
        initialized = false;
    }

    void pause() {
        std::unique_lock lock(mtx);
        paused = true;
    }

    void resume() {
        {
            std::unique_lock lock(mtx);
            paused = false;
        }
        condition.notify_all();
    }

private:
    mutable std::shared_mutex mtx;
    mutable std::condition_variable_any condition;
    std::vector<std::thread> workers;
    MyQueue <Task> tasks;

    bool initialized = false;
    bool force_terminate = false;
    bool terminated = false;
    bool paused = false;

    void routine() {
        while (true) {
            Task task;
            {
                std::unique_lock lock(mtx);
                auto wait_condition = [this] {
                    return terminated || (!tasks.empty() && !paused);
                };

                condition.wait(lock, wait_condition);

                if (force_terminate) {
                    return;
                }

                if (!tasks.empty()) {
                    task = tasks.top();
                    tasks.pop(task);
                }
            }
            
            if (task.callback) {
                task.callback(task);
            }

            if (terminated) {
                return; 
            }
        }
    }

    bool working() const {
        std::shared_lock lock(mtx);
        return working_unsafe();
    }

    bool working_unsafe() const {
        return initialized && !terminated;
    }
};