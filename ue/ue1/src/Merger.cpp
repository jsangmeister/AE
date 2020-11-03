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
    Q_t arr[100];
    for (int i=0; i<100; i++) arr[i] = std::rand()%500;
    merge(arr, 100, sizeof(Q_t)*20, 2);

    for(auto val : arr)
    {
        //std::cout << val << std::endl;
    }
    return 0;
}

void merge(Q_t* data, std::uint64_t n, std::uint64_t m, std::uint64_t b)
{
    auto blocksize = (m/b)/sizeof(Q_t);
    std::cout << blocksize << std::endl;
    for (int i=0; i<n/blocksize; i++)
    {
        std::sort(data+(i*blocksize), data + (i+1)*blocksize);
    }
    if (n%blocksize>1)
    {
        std::sort(data + n-(n%b), data + n);
    }
    Q_t* copy_arr = new Q_t[n];
    //if blocksize is >= n, everything is sorted
    while (blocksize<n)
    {   
        std::cout << "Loop1" << std::endl;
        //merges per level (minus the merge for the rest)
        for (int j=0; j<n/(1.0/2*m/b*blocksize); j++)
        {   
            std::cout << "  Loop2" << std::endl;
            std::priority_queue<std::pair<Q_t, std::uint64_t>> pq;
            std::uint64_t mergeshift = j*int(1.0/2*m/b)*blocksize; //shift to section where our runs begin;
            std::cout << mergeshift << std::endl;
            int counter=0;
            for(int k=0; k < 1.0/2*m/b && k < n/blocksize; k++){
                std::uint64_t runshift = mergeshift + k*blocksize; //shifts to the start of the run
                //a pair contains the value and the offset from data begin (so we can later simply use %)

                std::cout << "  " << runshift << std::endl;
                pq.push(std::pair<Q_t, std::uint64_t>(*(data+runshift), runshift));
            }
            auto writestart = mergeshift;//We might need mergeshift for the cleanup
            while (!pq.empty())
            {
                auto kv_pair = pq.top();
                *(copy_arr + writestart++) = kv_pair.first;
                std::cout << counter++ << "      " << kv_pair.first << std::endl;
                pq.pop();
                if ((kv_pair.second+1)%blocksize != 0)
                {   
                    pq.push(std::pair<Q_t, std::uint64_t>(*(data+kv_pair.second+1), kv_pair.second+1));
                }
            }            
        }
        for (int i=0; i<n; i++)
        {
            //std::cout << *(copy_arr+i) << std::endl;
        }
        //To-Do: sort the rest elements
        break;
        //increase blocksize to new value
        blocksize = 1.0/2*m/b*blocksize;

        //Clean up array references for the next run
        auto tmp = data;
        data = copy_arr;
        copy_arr = data;
    }
}
