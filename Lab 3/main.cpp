#include <iostream>
#include "threadPool.h"

// program
std::atomic<bool> end_program(false);
const int time_to_work = 30;

// for generator
const int g_num = 2;
std::atomic<bool> end_generation(false);
std::atomic<int> task_id(0);
std::vector<std::thread> generators;


int getValueBetweenTwo(int max, int min) {
    return std::rand() % (max - min + 1) + min;
}

void executeTask(Task task) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << task.id << " task" << " started working." << std::endl;
    }
        std::this_thread::sleep_for(std::chrono::seconds(task.duration_t));
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << task.id << " task" << " complited. Time: " << task.duration_t << "." << std::endl;
    }
}

int main() {
    ThreadPool pool;
    pool.initialize();

    generators.reserve(g_num);

    for (int i = 0; i < g_num; ++i) {
        generators.emplace_back([&pool] {
            while (!end_program) {
                int id = task_id.fetch_add(1);
                int duration = getValueBetweenTwo(task_max_t, task_min_t);

                Task newTask{id, duration};

                pool.add_task(executeTask, newTask);

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