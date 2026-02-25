#include <mutex>
#include "seq_algorithm.h"

//var 19: Знайти кількість елементів, більших за 20, а також найбільше таке число.

std::mutex mtx;

void mutexTask (const int start_pos, const int end_pos) {
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
        singleThread(argc);
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