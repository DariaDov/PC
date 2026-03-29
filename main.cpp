#include <iostream>
#include <mutex>
#include <vector>
#include <thread>
#include <functional>
#include <atomic>
#include <queue>
#include <chrono>

struct Task{
    int id;
    int duration_t;
    // std::function<void()> to_do;
};

// program
std::atomic<bool> end_program(false);
const int time_to_work = 30;

// for tasks
const int pool_size = 4;
const int task_min_t = 3;
const int task_max_t = 6;

// for thread pool
std::mutex mtx;

// for queue
std::queue<Task> tasks;

// for generator
const int g_num = 2;
std::atomic<bool> end_generation(false);
std::atomic<int> task_id(0);
std::vector<std::thread> generators;

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
        // {
        //     read_lock lock(m_rw_lock);
        //     if (!working_unsafe()) {
        //         return;
        //     }
        // }

        auto task = std::bind(forward<task_t>(task), forward<arguments>(parameters)...);
        m_tasks.emplace(PrioritizedTask{priority, bind});
        m_task_waiter.notify_one();
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

int getValueBetweenTwo(int max, int min) {
    return std::rand() % (max - min + 1) + min;
}

void executeTask(Task task) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "Task " << task.id << " started, duration: " << task.duration_t << " seconds.\n";
    }
        std::this_thread::sleep_for(std::chrono::seconds(task.duration_t));
    {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "Task " << task.id << " completed.\n";
    }
}

int main() {
    ThreadPool pool;
    pool.initialize();

    generators.reserve(g_num);

    for (int i = 0; i < g_num; ++i) {
        generators.emplace_back([&pool] {
            while (!end_generation) {
                int id = task_id.fetch_add(1);
                int duration = getValueBetweenTwo(task_max_t, task_min_t);

                Task newTask{id, duration};

                pool.add_task(executeTask, newTask);

                int interval = getValueBetweenTwo(1, 3);
                std::this_thread::sleep_for(std::chrono::seconds(interval));
            }
        });
        // generators.emplace_back([&pool] {
        //     while (!end_generation) {
        //         int duration = getValueBetweenTwo(task_max_t, task_min_t);
        //         int id = task_id.fetch_add(1);
        //         pool.add_task(duration, executeTask, id, duration);

        //         int interval = getValueBetweenTwo(1, 3);
        //         std::this_thread::sleep_for(std::chrono::seconds(interval));
        //     }
        // });
    }

    std::this_thread::sleep_for(std::chrono::seconds(time_to_work));
    end_generation = true;
    end_program = true;

    for (int i = 0; i < g_num; i++) {
            generators[i].join();
    }

    pool.terminate();

    return 0;
}