#include <iostream>
#include <cstdint>
#include <cstring>
#include <random>
#include <limits>

#define Q_t int

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

    std::cout << N << std::endl << M << std::endl << B << std::endl;
}

int usage() {
    std::cout << "Usage:\n./mergesort N M B\nuse 'k' or 'm' as suffixes -> e.g. 64k" << std::endl;
    return 1;
}

uint64_t parse(char* arg){
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
    int i = std::stoull(arg);
    return i * factor;
}

Q_t* generate_data(uint64_t N)
{
    std::seed_seq ssq{42};
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
