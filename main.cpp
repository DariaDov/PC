#include <iostream>
#include <thread>
#include <cstdlib>
#include <vector>
#include <algorithm>

//var 19

const int n = 10;
const int max_num = 100;
const int threads_num = 10;

int matrix[n][n]; 

void fillRandom() {
    for (int i = 0; i < n; ++i) {
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
 
int main() {
    fillRandom();
    printMatrix();

    std::vector<std::thread> threads;
 
	for (int i = 0; i < threads_num; i++)	{
		threads.push_back(std::thread(findMax, i));
	}

    for (auto& t : threads) {
        t.join();
    }

    printMatrix();
    
    return 0;
}
