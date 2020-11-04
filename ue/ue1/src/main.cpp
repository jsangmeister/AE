#include <iostream>
#include <cstdint>
#include <cstring>
#include <random>
#include <limits>
#include <chrono>
#include <thread>
#include "extern-mergesort.h"
#include "mergesort.h"

int usage();
uint64_t parse(char* arg);
Q_t* generate_data(uint64_t N);

int main(int argc, char** argv) {
    // Arguments: N M B
    // Add k as suffix for a factor of 1024, m for a factor of 1024*1024
    // e.g. "64k"

    if (argc != 4) {
        std::cout << "Wrong number of arguments!" << std::endl;
        return usage();
    }

    uint64_t N = parse(argv[1]);
    uint64_t M = parse(argv[2]);
    uint64_t B = parse(argv[3]);
    Q_t* data = generate_data(N);

    auto start = std::chrono::high_resolution_clock::now();
    extern_mergesort(data, N, M / sizeof(Q_t), B / sizeof(Q_t));
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    Q_t* array_to_check = data;
    bool found_error = false;
    Q_t last = array_to_check[0];
    for(uint64_t i = 1; i < N; i++) {
        if (array_to_check[i] < last) {
            std::cout << "Error: i=" << i << " last=" << (unsigned long long)last << " array_to_check[i]=" << (unsigned long long)array_to_check[i] << std::endl;
            found_error = true;
        }
        last = array_to_check[i];
    }
    if (found_error) {
        return 1;
    }

    std::cout << N << std::endl << M << std::endl << B << std::endl << duration << std::endl;
    return 0;
}

int usage() {
    std::cout << "Usage:\n./mergesort N M B\nuse 'k' or 'm' as suffixes -> e.g. 64k" << std::endl;
    return 1;
}

uint64_t parse(char* arg) {
    int n = strlen(arg);
    unsigned int factor = 1;
    if (arg[n - 1] == 'k') {
        factor = 1024;
        arg[n - 1] = 0;
    }
    if (arg[n - 1] == 'm') {
        factor = 1024*1024;
        arg[n - 1] = 0;
    }
    uint64_t i = std::stoull(arg);
    uint64_t value = i * factor;

    // check, if power of 2
    if (value <= 1 || (value & (value - 1)) != 0) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "The value %s must be a power of 2 and greater than 1!", arg);
        throw std::invalid_argument(buffer);
    }
    return value;
}

Q_t* generate_data(uint64_t N) {
    int ms = 42; //std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::seed_seq ssq{ms};
    std::mt19937 gen(ssq);
    Q_t min = std::numeric_limits<Q_t>::min();
    Q_t max = std::numeric_limits<Q_t>::max();
    std::uniform_int_distribution<Q_t> dist(min, max);

    Q_t* arr = new Q_t[N];
    for (uint64_t i = 0; i < N; ++i) {
        arr[i] = dist(gen);
    }
    return arr;
}
