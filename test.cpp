// g++ -std=c++11 -o test test.cpp

#include <iostream>
#include <random>

int main() {

    #ifdef _WIN32
        std::cout << "Running on Windows" << std::endl;
    #elif __linux__
        std::cout << "Running on Linux" << std::endl;
    #else
        std::cout << "Unknown OS" << std::endl;
    #endif

    
    std::random_device rd;           // Seed
    std::mt19937 gen(rd());          // Random number engine
    std::uniform_int_distribution<int> dist(0, 1); // Uniform distribution [0, 1]

    // Generate random numbers
    int count_0 = 0, count_1 = 0;
    for (int i = 0; i < 10000; ++i) {
        int num = dist(gen);
        if (num == 0) count_0++;
        else count_1++;
    }

    std::cout << "Generated 0: " << count_0 << " times\n";
    std::cout << "Generated 1: " << count_1 << " times\n";

    return 0;
}