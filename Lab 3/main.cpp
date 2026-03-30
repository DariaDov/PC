#include <iostream>
#include "threadPool.h"
#include "cout.h"

// program
std::atomic<bool> end_program(false);
const int time_to_work = 10;
std::mutex mtx;

// for generator
const int g_num = 3;
std::atomic<int> task_id(0);
std::vector<std::thread> generators;

const int task_min_t = 3;
const int task_max_t = 6;


int getValueBetweenTwo(int max, int min) {
    return std::rand() % (max - min + 1) + min;
}

void executeTask(Task task) {       
    {
        std::lock_guard<std::mutex> cout_lock(cout_mtx);
        std::cout << task.id << " task" << " complited. Time: " << task.duration << "." << std::endl;
    }
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    std::cout << std::unitbuf;

    auto start = std::chrono::steady_clock::now();

    ThreadPool pool;
    pool.initialize(pool_size);

    generators.reserve(g_num);

    for (int i = 0; i < g_num; ++i) {
        generators.emplace_back([&pool, i] {            
            while (!end_program) {
                int duration = getValueBetweenTwo(task_max_t, task_min_t);
                int id = task_id.fetch_add(1);

                Task newTask{id, duration, std::function(executeTask)};

                {
                std::lock_guard<std::mutex> cout_lock(cout_mtx);
                std::cout << "-----Task " << id << " added. G id: " << i << std::endl;
                }

                pool.enqueue(newTask);

                int interval = getValueBetweenTwo(3, 1);
                std::this_thread::sleep_for(std::chrono::seconds(interval));
            }
        });
    }

    pool.pause();
    std::this_thread::sleep_for(std::chrono::seconds(10));
    pool.resume();

    std::this_thread::sleep_for(std::chrono::seconds(time_to_work));
    end_program = true;

    for (int i = 0; i < g_num; i++) {
            generators[i].join();
    }

    pool.terminate();

    auto end = std::chrono::steady_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << std::endl;
    std::cout << "Task created: " << task_id.load() << std::endl;
    std::cout << "Task executed: " << pool.tasks_complited.load() << std::endl;

    std::cout << "Average time a thread is in the waiting state: " << pool.waiting_time.load()/pool.tasks_complited.load() << " ms" << std::endl;
    std::cout << "Аverage length of the queue: " << pool.queue_size.load()/pool.tasks_complited.load() << std::endl;
    std::cout << "Аverage task execution time: " << dur.count()/pool.tasks_complited.load() << " ms" << std::endl;
   
    return 0;
}