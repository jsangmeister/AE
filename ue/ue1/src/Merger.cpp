#include <iostream>
#include <cstdint>
#ifndef Q_t
    #define Q_t int
#endif
#include <algorithm>

void merge(Q_t* data, uint64_t n, uint64_t m, uint64_t b);

int _main()
{
    Q_t arr[10] = {17, 24, 2, 6, 25, 13, 0, 76, 8, 10};
    merge(arr, 10, sizeof(Q_t)*3, 1);

    for(auto val : arr)
    {
        std::cout << val << std::endl;
    }
    return 0;
}

void merge(Q_t* data, uint64_t n, uint64_t m, uint64_t b)
{
    auto blocksize = (m/b)/sizeof(Q_t);
    for (uint64_t i=0; i<n/blocksize; i++)
    {
        std::sort(&data[i], &data[(i+1)*blocksize]);
    }
    if (n%blocksize>1)
    {
        std::sort(&data[n-(n%b)], &data[n]);
    }


}
