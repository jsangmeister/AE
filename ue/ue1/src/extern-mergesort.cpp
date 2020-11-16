#include "extern-mergesort.h"
#include "mergesort.h"
#include <iostream>

void sort_chunks(Q_t* data, uint64_t n, uint64_t m) {
    uint64_t chunksize = m / 2;

    // Special case, if M/2 is larger than N -> Everything fits in memory,
    // so the loop will only have one itreation sorting everything.
    if (chunksize > n) {
        chunksize = n;
    }
    for (uint64_t start = 0; start < n; start += chunksize) {
        // std::sort(data + start, data + start + chunksize);
        mergesort(data + start, chunksize);
    }
}

struct QueueEntry {
    Q_t value; 
    uint64_t index;

    QueueEntry() 
        : value(0), index(0)
    { } 

    QueueEntry(Q_t value, uint64_t index) 
        : value(value), index(index)
    { } 
};

// repairs the heap a between l and r
void sift(QueueEntry* a, uint64_t l, uint64_t r) {
    QueueEntry x = a[l];
    uint64_t i = l;
    uint64_t j = 2*i + 1;
    if ((j < r) && (a[j + 1].value < a[j].value)) {
        j++;
    }
    while ((j <= r) && (a[j].value < x.value)) {
        a[i] = a[j];
        i = j;
        j = 2*i + 1;
        if ((j < r) && (a[j + 1].value < a[j].value)) {
            j++;
        }
    }
    a[i] = x;
}

void k_merge(Q_t* from, Q_t* into, uint64_t start, uint64_t k, uint64_t run_size) {
    QueueEntry* pq = new QueueEntry[k];
    uint64_t elements_in_pq = k;

    // Fill the pq with every first value of each run
    for (uint64_t l = 0; l < k; l++) {
        uint64_t index = start + l*run_size;
        // std::cout << "push entry " << (unsigned long long)from[index] << " at " << index << std::endl;
        pq[l] = QueueEntry(from[index], index);
    }

    // Build heap
    for (int64_t l = (k - 2) / 2; l >= 0; l--) {
        sift(pq, l, k - 1);
    }
    /*for (uint64_t l = 0; l < elements_in_pq; l++) {
        std::cout << (unsigned long long)pq[l].value << std::endl;
    }*/

    uint64_t end = start + k * run_size;
    // std::cout << "from " << start << " to " << end << std::endl;
    for (uint64_t i = start; i < end; i++) {
        // find the value for into[i]: the min of the pq.
        uint64_t index = pq[0].index;
        into[i] = pq[0].value;

        // std::cout << i << " top element " << (unsigned long long)pq[0].value << " at " << index << std::endl;

        if ((index + 1) % run_size != 0) {
            // pq.push(QueueEntry(from[entry.index + 1], entry.index + 1));
            pq[0].value = from[index + 1];
            pq[0].index = index + 1;
            // std::cout << "push " << (unsigned long long)from[index + 1] << " at " << index + 1 << std::endl;
        } else {
            pq[0].value = pq[elements_in_pq - 1].value;
            pq[0].index = pq[elements_in_pq - 1].index;
            // copy last element into first position
            elements_in_pq--;
        }

        // repair
        if (elements_in_pq > 0) {
            sift(pq, 0, elements_in_pq - 1);
        }

        /*for (uint64_t l = 0; l < elements_in_pq; l++) {
            std::cout << (unsigned long long)pq[l].value << std::endl;
        }*/
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
