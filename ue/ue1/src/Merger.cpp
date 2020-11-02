#include<iostream>
#include <cstdint>
#define Q_t int
#include <algorithm>

void merge(Q_t* data, std::uint64_t n, std::uint64_t m, std::uint64_t b);

int main()
{
    Q_t arr[10] = {17, 24, 2, 6, 25, 13, 0, 76, 8, 10};
    merge(arr, 10, sizeof(Q_t)*3, 1);

    for(auto val : arr)
    {
        std::cout << val << std::endl;
    }
    return 0;
}

void merge(Q_t* data, std::uint64_t n, std::uint64_t m, std::uint64_t b)
{
    auto blocksize = (m/b)/sizeof(Q_t);
    for (int i=0; i<n/blocksize; i++)
    {
        std::sort(&data[i], &data[(i+1)*blocksize]);
    }
    if (n%blocksize>1)
    {
        std::sort(&data[n-(n%b)], &data[n]);
    }


}
