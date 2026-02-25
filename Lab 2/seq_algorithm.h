#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>

std::vector<int> numbers;

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

void singleThread (int argc) {
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