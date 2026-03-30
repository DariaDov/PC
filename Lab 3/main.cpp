#include <iostream>
#include "threadPool.h"

// program
std::atomic<bool> end_program(false);
const int time_to_work = 10;
std::mutex mtx;

// for generator
const int g_num = 2;
std::atomic<bool> end_generation(false);
std::atomic<int> task_id(0);
std::vector<std::thread> generators;

const int task_min_t = 3;
const int task_max_t = 6;


int getValueBetweenTwo(int max, int min) {
    return std::rand() % (max - min + 1) + min;
}

void executeTask(Task task) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << task.id << " task" << " started working." << std::endl;
    }
        std::this_thread::sleep_for(std::chrono::seconds(task.duration));
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << task.id << " task" << " complited. Time: " << task.duration << "." << std::endl;
    }
}

// Done (but not shure)
int main() {
    ThreadPool pool;
    pool.initialize(pool_size);

    generators.reserve(g_num);

    for (int i = 0; i < g_num; ++i) {
        generators.emplace_back([&pool, i] {
            while (!end_program) {
                int id = task_id.fetch_add(1);
                int duration = getValueBetweenTwo(task_max_t, task_min_t);

                Task newTask{id, duration, std::function(executeTask)};

                pool.enqueue(newTask);
                
                {
                std::lock_guard<std::mutex> lock(mtx);
                std::cout << "-----Task " << id << " added. G id: " << i << std::endl;
                }
                int interval = getValueBetweenTwo(1, 3);
                std::this_thread::sleep_for(std::chrono::seconds(interval));
            }
        });
    }

    std::this_thread::sleep_for(std::chrono::seconds(time_to_work));
    end_program = true;

    for (int i = 0; i < g_num; i++) {
            generators[i].join();
    }

    pool.terminate();

    return 0;
}