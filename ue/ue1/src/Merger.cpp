#include<iostream>
#include <cstdint>
#ifndef Q_t
    #define Q_t int
#endif
#include <algorithm>
#include <math.h>
#include <queue> 

void merge(Q_t* data, std::uint64_t n, std::uint64_t m, std::uint64_t b);

int main()
{
    Q_t arr[10] = {17, 24, 2, 6, 25, 13, 0, 76, 8, 10};
    merge(arr, 10, sizeof(Q_t)*3*2, 2);

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
        std::sort(data+(i*blocksize), data + (i+1)*blocksize);
    }
    if (n%blocksize>1)
    {
        std::sort(data + n-(n%b), data + n);
    }
    Q_t* copy_arr = new Q_t[n];
    //recursion levels
    for(int i=0; i<log10(double(n)/m)/log10(double(m)/b); i++)
    {   
        //merges per level (minus the merge for the rest)
        for (int j=0; j<n/(1.0/2*m/b*blocksize); j++){
            std::priority_queue<std::pair<Q_t, std::uint64_t>> pq;
            std::uint64_t mergeshift = j*int(1.0/2*m/b)*blocksize; //shift to section where our runs begin;
            for(int k=0; k < 1.0/2*m/b; k++){
                std::uint64_t runshift = mergeshift + k*blocksize; //shifts to the start of the run
                //a pair contains the value and the offset from data begin (so can later simply use %)
                pq.push(std::pair<Q_t, std::uint64_t>(*(data+runshift), runshift));
            }
            auto writestart = mergeshift;
            while (!pq.empty())
            {
                auto kv_pair = pq.top();
                *(copy_arr + writestart) = kv_pair.first;
                pq.pop();
                if ((kv_pair.second+1)%blocksize != 0)
                {
                    pq.push(std::pair<Q_t, std::uint64_t>(*(data+kv_pair.second+1), kv_pair.second+1));
                }
            }            
        }
        //To-Do: sort the rest elements

        //increase blocksize to new value
        blocksize = 1.0/2*m/b*blocksize;

        //Clean up array references for the next run (or end of program)
        auto tmp = data;
        data = copy_arr;
        copy_arr = data;
    }

}
