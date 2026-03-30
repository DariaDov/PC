#pragma once

#include <vector>
#include <thread>
#include <functional>
#include <atomic>
#include <chrono>
#include <condition_variable>

#include "queue.h"
#include "task.h"
#include "cout.h"

const int pool_size = 4;

// std::shared_lock lock(mtx);
// std::unique_lock lock(mtx);

class ThreadPool {
public:
    ThreadPool() = default;
    ~ThreadPool() { terminate(); }

    std::atomic<int> waiting_time{0};
    std::atomic<int> tasks_complited{0};
    std::atomic<int> queue_size{0};

    void initialize(int pool_size) {
        std::unique_lock lock(mtx);

        if (initialized || terminated) return;

        workers.reserve(pool_size);
        for (int i = 0; i < pool_size; ++i) {
            workers.emplace_back(&ThreadPool::routine, this);
        }

        initialized = !workers.empty();

        waiting_time = 0;
        tasks_complited = 0;
        queue_size = 0;
    }

    template <typename F>
    void enqueue(F task) {
        if (!working()) {
            return;
        }

        tasks.emplace(task);
        condition.notify_one();
    }

    void terminate() {
        terminate(terminated);
    }

    void force_terminate() {
        terminate(force_terminated);
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
    std::atomic<int> worker_id{0};

    bool initialized = false;
    bool force_terminated = false;
    bool terminated = false;
    bool paused = false;

    void routine() {
        int id = worker_id.fetch_add(1);
        while (true) {
            if (terminated || force_terminated) {
                return; 
            }

            Task task;
            {
                std::unique_lock lock(mtx);
                auto wait_condition = [this] {
                    return !terminated && !force_terminated && !tasks.empty() && !paused;
                };

                auto start = std::chrono::steady_clock::now();

                condition.wait(lock, wait_condition);

                auto end = std::chrono::steady_clock::now();
                auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                waiting_time.fetch_add(dur.count());

                if (!tasks.empty()) {
                    queue_size.fetch_add(tasks.size());
                    tasks.pop(task);
                }

                std::lock_guard<std::mutex> cout_lock(cout_mtx);
                std::cout << task.id << " task" << " started working. Worker id: " << id << std::endl;
            }
            
            for (int i = 0; i < 100; i++) {
                if (force_terminated) return;
                std::this_thread::sleep_for(std::chrono::milliseconds(task.duration * 10));
            }

            tasks_complited.fetch_add(1);

            if (task.callback) {
                task.callback(task);
            }
        }
    }

    bool working() {
        std::shared_lock lock(mtx);
        return working_unsafe();
    }

    bool working_unsafe() const {
        return initialized && !terminated && !force_terminated;
    }

    void reset() {
        std::unique_lock lock(mtx);
        workers.clear();
        terminated = false;
        force_terminated = false;
        paused = false;
        initialized = false;
        worker_id = 0;
    }

    void terminate(bool& termination_f) {
        {
            std::unique_lock lock(mtx);
            
            if (working_unsafe()) {
                termination_f = true;
            }
            else {
                return;
            }
        }

        condition.notify_all();
        
        for (std::thread& worker : workers) {
            if (worker.joinable()){
                worker.join();
            }
        }

        reset();
    }
};