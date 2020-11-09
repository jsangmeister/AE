#include "extern-mergesort.h"
#include "mergesort.h"
#include <iostream>

void sort_chunks(Q_t* data, uint64_t n, uint64_t m) {
    uint64_t chunksize = m / 2;
    for (uint64_t start = 0; start < n; start += chunksize) {
        // std::sort(data + start, data + start + chunksize);
        mergesort(data + start, chunksize);
    }
}

struct QueueEntry {
    Q_t value; 
    uint64_t index;

    QueueEntry(Q_t value, uint64_t index) 
        : value(value), index(index)
    { } 
};

struct CompareQueueEntries { 
    bool operator()(QueueEntry const& p1, QueueEntry const& p2) {
        return p1.value > p2.value; 
    } 
};

void k_merge(Q_t* from, Q_t* into, uint64_t start, uint64_t k, uint64_t run_size) {
    std::priority_queue<QueueEntry, std::vector<QueueEntry>, CompareQueueEntries> pq; 

    // Fill the pq with every first value of each run
    for (uint64_t l = 0; l < k; l++) {
        uint64_t index = start + l*run_size;
        // std::cout << "push entry " << (unsigned long long)from[index] << " at " << index << std::endl;
        pq.push(QueueEntry(from[index], index));
    }

    uint64_t end = start + k * run_size;
    // std::cout << "from " << start << " to " << end << std::endl;
    for (uint64_t i = start; i < end; i++) {
        // find the value for into[i]: the min of the pq.
        QueueEntry entry = pq.top();
        pq.pop();
        into[i] = entry.value;

        // std::cout << i << " top element " << (unsigned long long)entry.value << " at " << entry.index << std::endl;

        if ((entry.index + 1) % run_size != 0) {
            pq.push(QueueEntry(from[entry.index + 1], entry.index + 1));
            // std::cout << "push " << (unsigned long long)from[entry.index + 1] << " at " << entry.index + 1 << std::endl;
        }
    }
}

// Every number must be a power of 2, else this algorithm is not going to work!!
// n: amount elements in data
// m: amount of elements in the memory
// b: amount of elements in one block
void extern_mergesort(Q_t* data, uint64_t n, uint64_t m, uint64_t b) {
    Q_t* start_pt = data;
    sort_chunks(data, n, m);

    /*for(uint64_t i = 0; i < n; i++) {
        // std::cout << i << ": " << (unsigned long long)(data[i]) << std::endl;
    }*/
    // std::cout << "chunks ok" << std::endl;

    uint64_t k = (m/b) / 2;
    // std::cout << "k=" << (unsigned long long)k << std::endl;
    if (k < 2) {
        throw std::invalid_argument("k must be > 1 -> Choose M and B to be at least M > 4*B!");
    }
    Q_t* buffer = new Q_t[n];

    uint64_t current_run_size = m / 2; // this is the chunks size
    while (current_run_size < n) {
        // std::cout << "current run size=" << (unsigned long long)current_run_size << std::endl;
        // take k runs and merge them together;
        uint64_t size_of_all_runs = k * current_run_size;
        for (uint64_t start = 0; start < n; start += size_of_all_runs) {
            // merge `start...(start+size_of_all_runs)` of `k` runs with size `current_run_size` together
            // Last iteration: here, we might not have k runs anymore
            uint64_t _k = k;
            if (start + size_of_all_runs > n) {
                if (start != 0) {
                    throw std::runtime_error("Should not happen...");
                }
                _k = n / current_run_size;
            }

            // std::cout << _k << "-way merge from " << start << " to " << (start + current_run_size*_k) << std::endl;

            k_merge(data, buffer, start, _k, current_run_size);
            /*for(uint64_t i = 0; i < n; i++) {
                // std::cout << "b " << i << ": " << (unsigned long long)(buffer[i]) << std::endl;
            }*/
            // copy buffer -> data
            // TODO: Swapping indices in every recursion layer removes the need to do this copying...
            //for (uint64_t i = start; i < start + current_run_size*_k; i++) {
            //    data[i] = buffer[i];
            //}
        }
        // We switch the pointers between data and buffer, because buffer is the array 
        // after one sort iteration
        auto tmp_pt = data;
        data = buffer;
        buffer = tmp_pt;

        current_run_size *= k;
    }

    // data might point to the location of buffer. Because we got the data pointer 
    // by value, we might need this copy step. (Or I'm just missing a c++ feature)
    if (data != start_pt)
    {
        for (uint64_t i = 0; i < n; i++) {
            start_pt[i]=data[i];
        }
    }
}
