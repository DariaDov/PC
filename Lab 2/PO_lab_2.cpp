#include <iostream>
#include <thread>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <chrono>
#include <mutex>

//var 19: Знайти кількість елементів, більших за 20, а також найбільше таке число.

std::vector<int> numbers;
std::mutex mtx;

const int threshold = 20;
const int max_num = 40;
int threads_num = 6;
int n = 100000000;

int cnt = 0;
int max_el = 0;

void fillRandom() {
 for (int i = 0; i < n; ++i) {
        numbers.push_back(rand() % max_num);
    }
}

void printVector()
{
    for (auto i : numbers) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
}

void noParTask () {
    max_el = *std::max_element(numbers.begin(), numbers.end());
    if (max_el <= threshold) {
        max_el = 0;
        return;
    }
    cnt = std::count_if(numbers.begin(), numbers.end(), [](int num) {
        return num > threshold;
    });
}

void mutexTask (const int start_pos, const int end_pos) {
    // int local_cnt = 0;
    // int local_max_el = 0;

    // local_max_el = *std::max_element(numbers.begin() + start_pos, numbers.begin() + end_pos);
    // if (local_max_el <= threshold) {
    //     return;
    // }

    // local_cnt = std::count_if(numbers.begin() + start_pos, numbers.begin() + end_pos, [](int num) {
    //     return num > threshold;
    // });    

    for (int i = start_pos; i < end_pos; ++i) {
        if (numbers[i] > 20) {
            mtx.lock();
            cnt++;
            if (numbers[i] > max_el) {
                max_el = numbers[i];
            }
            mtx.unlock();
        }
    }
}
 
// p1 - n, p2 - threads_num
int main(int argc, char* argv[]) {
    if (argc > 1) {
        n = atoi(argv[1]);
        threads_num = atoi(argv[2]);
    }

    numbers.reserve(n);
    fillRandom();
    // printVector();

    if (threads_num == 1) {
        auto start1 = std::chrono::steady_clock::now();
        noParTask();
        auto end1 = std::chrono::steady_clock::now();
        auto diff1 = end1 - start1;
        if (argc == 1) {
            std::cout << "Кількість елементів більша за 20: " << cnt << std::endl;
            std::cout << "Найбільший елемент: " << max_el << std::endl;
            std::cout << "Час без використання паралелізації: " << std::chrono::duration_cast<std::chrono::microseconds>(diff1).count() << std::endl;
        }
        else {
            std::cout << std::chrono::duration_cast<std::chrono::microseconds>(diff1).count() << std::endl;
        }
    }
    else {
        std::vector<std::thread> threads;
    
        int start_threads = std::min(n, threads_num);
        threads.reserve(start_threads);

        auto start2 = std::chrono::steady_clock::now();

        int start_pos = 0;
        int end_pos = 0;
        int step = n / start_threads;
        for (int i = 0; i < start_threads; i++) {
            start_pos = i * step;
            end_pos = (i == start_threads - 1) ? n : start_pos + step;

            threads.push_back(std::thread(mutexTask, start_pos, end_pos));
        }

        for (int i = 0; i < start_threads; i++) {
            threads[i].join();
        }

        auto end2 = std::chrono::steady_clock::now();
        auto diff2 = end2 - start2;
        if (argc == 1) {
            std::cout << "Кількість елементів більша за 20: " << cnt << std::endl;
            std::cout << "Найбільший елемент: " << max_el << std::endl;
            std::cout << "Час mutex: " << std::chrono::duration_cast<std::chrono::microseconds>(diff2).count() << std::endl;
        }
        else {
            std::cout << std::chrono::duration_cast<std::chrono::microseconds>(diff2).count() << std::endl;
        }
    }
    
    return 0;
}