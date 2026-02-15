#include <iostream>
#include <thread>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <chrono>

//var 19

int n = 10;
int threads_num = 5;
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

void findMax(const int tread_id) {
    int max_val_id = std::max_element(&matrix[tread_id][0], &matrix[tread_id][0] + n) - &matrix[tread_id][0];
    std::swap(matrix[tread_id][tread_id], matrix[tread_id][max_val_id]);
}
 
// p1 - n, p2 - threads_num
int main(int argc, char* argv[]) {
    if (argc > 1) {
        n = atoi(argv[1]);
        threads_num = atoi(argv[2]);
    }

    fillRandom();
    // printMatrix();

    auto start = std::chrono::steady_clock::now();

    if (threads_num == 1) {
        for (int i = 0; i < n; i++) {
            findMax(i);
        }
    }
    else {
        std::vector<std::thread> threads;
    
        int start_threads = std::min(n, threads_num);

        for (int i = 0; i < start_threads; i++) {
            threads.push_back(std::thread(findMax, i));
        }

        for (int i = 0; i < start_threads; i++) {
            threads[i].join();
            if (start_threads < n) {
                threads.push_back(std::thread(findMax, start_threads));
                start_threads++;
            }
        }
    }

    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(diff).count();

    // printMatrix();
    
    return 0;
}