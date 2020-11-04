#include "extern-mergesort.h"
#include <iostream>

// Merges the data from `from` between l,m,r into `into`.
void merge(Q_t* from, Q_t* into, uint64_t l, uint64_t m, uint64_t r) {
    uint64_t i = l;
    uint64_t j = m;
 
    for (uint64_t k = l; k < r; k++) {
        if (i < m && (j >= r || from[i] <= from[j])) {
            into[k] = from[i];
            i++;
        } else {
            into[k] = from[j];
            j++;
        }
    }
}

// sorts the data from `from` and saves the result into `into`.
// `from` and `into` are only accessed between l and r. Only the
// data in `into` is modified.
// r is exclusive
void _mergesort(Q_t* from, Q_t* into, uint64_t l, uint64_t r) {
    if(r <= (l + 1)) {
        return;
    }

    uint64_t m = (l + r) / 2;

    // Note: into and from are switched: This is desired!
    _mergesort(into, from, l, m);
    _mergesort(into, from, m, r);

    merge(from, into, l, m, r);
}

void mergesort(Q_t* data, uint64_t n) {
    Q_t* buffer = new Q_t[n];
    for (uint64_t i = 0; i < n; ++i) {
        buffer[i] = data[i];
    }
    _mergesort(buffer, data, 0, n);
}
