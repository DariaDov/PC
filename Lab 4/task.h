#pragma once
#include <vector>
#include <thread>
#include <algorithm>
#include <atomic>
#include <iostream>

class Task{
public:
    int matrix_size;
    int matrix_size_elem;
    int matrix_size_bytes;
    int *matrix;
    int thread_num;
    std::atomic<int> progress;
    std::atomic<bool> stop_flag = false;
    std::vector<std::thread> threads;

    Task(int matrix_size, int *matrix, int thread_num) {
        this->matrix_size = matrix_size;
        this->matrix_size_elem = matrix_size * matrix_size;
        this->matrix_size_bytes = this->matrix_size_elem * sizeof(int);
        this->thread_num = thread_num;
        this->matrix = matrix;
        this->progress = 0;

        this->thread_num = std::min(this->thread_num, this->matrix_size);
    };
    ~Task() {
        finish();
    };
    
    void run() {
        threads.reserve(thread_num);

        for (int i = 0; i < thread_num; i++) {
            threads.push_back(std::thread(findMax, i, this));
        }
    }

    int get_progress() {
        return progress * 100 / matrix_size; //percents
    }

    bool is_finished() {
        return progress == matrix_size;
    }

    void finish() {
        stop_flag = true;
        for (int i = 0; i < threads.size(); i++) {
            threads[i].join();
        }
    }

private:
    static void findMax(int start_pos, Task * task) {
        for (int c = start_pos * task->matrix_size; c < task->matrix_size * task->matrix_size; c += task->thread_num * task->matrix_size) {
            if (task->stop_flag) return;
            int max_val_id = std::max_element(task->matrix + c, task->matrix + c + task->matrix_size) - task->matrix - c;
            std::swap(task->matrix[c + c / task->matrix_size], task->matrix[max_val_id]);
            
            sleep(1);

            task->progress++;
        }
    }
};