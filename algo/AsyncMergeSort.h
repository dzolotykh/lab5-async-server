#ifndef ASYNC_SERVER_EXAMPLE_ASYNCMERGESORT_H
#define ASYNC_SERVER_EXAMPLE_ASYNCMERGESORT_H

#include <thread-pool/Pool.h>

class AsyncMergeSort {
public:
    // AsyncMergeSort(ThreadPool::Pool& _pool);
    // void operator()();

private:
    ThreadPool::Pool& pool;
};


#endif //ASYNC_SERVER_EXAMPLE_ASYNCMERGESORT_H
