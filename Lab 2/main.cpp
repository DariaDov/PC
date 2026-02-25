#include <iostream>
#include <thread>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <chrono>

//var 19

int n = 10;
int threads_num = 1;
const int max_num = 100;

int **matrix; 

void fillRandom() {
    matrix = new int*[n];

    for (int i = 0; i < n; ++i) {
        matrix[i] = new int[n];

        for (int j = 0; j < n; ++j) {
            matrix[i][j] = rand() % max_num;
        }
    }
}

void printMatrix()
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            std::cout << matrix[j][i] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void findMax(const int thread_id, const int num_threads) {
    for (int c = thread_id; c < n; c += num_threads) {
        int max_val_id = std::max_element(&matrix[c][0], &matrix[c][0] + n) - &matrix[c][0];
        std::swap(matrix[c][c], matrix[c][max_val_id]);
    }
}
 
// p1 - n, p2 - threads_num
int main(int argc, char* argv[]) {
    if (argc > 1) {
        n = atoi(argv[1]);
        threads_num = atoi(argv[2]);
    }

    fillRandom();
    // printMatrix(); // print

    auto start = std::chrono::steady_clock::now();
    int threads_time = 0;

    if (threads_num == 1) {
        findMax(0, 1);
    }
    else {
        std::vector<std::thread> threads;
    
        int start_threads = std::min(n, threads_num);
        threads.reserve(start_threads);

        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < start_threads; i++) {
            threads.push_back(std::thread(findMax, i, start_threads));
        }
        auto end = std::chrono::steady_clock::now();
        auto diff_threads = end - start;
        threads_time = std::chrono::duration_cast<std::chrono::microseconds>(diff_threads).count();

        for (int i = 0; i < start_threads; i++) {
            threads[i].join();
        }
    }

    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(diff).count() << std::endl;
    std::cout << threads_time << std::endl;

    // printMatrix(); // print
    
    return 0;
}