#include <iostream>
#include <thread>
#include <cstdlib>

//var 19

const int n = 10;
const int max_num = 100;

void fillRandom(int (&matrix)[n][n]) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            matrix[i][j] = rand() % max_num;
        }
    }
}

void printMatrix(int (&matrix)[n][n])
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}
 
int main() {
    int matrix[n][n]; 

    fillRandom(matrix);
    printMatrix(matrix);

    return 0;
}
