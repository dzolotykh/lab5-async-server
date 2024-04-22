#ifndef ASYNC_SERVER_EXAMPLE_ASYNCMERGESORT_H
#define ASYNC_SERVER_EXAMPLE_ASYNCMERGESORT_H

#include <thread-pool/Pool.h>

class AsyncMergeSort {
public:
    AsyncMergeSort(ThreadPool::Pool& _pool): pool(_pool) {}

    template<typename RandomAccessIterator>
    void operator()(RandomAccessIterator seq_begin, RandomAccessIterator seq_end) {
        if (seq_end - seq_begin < 2) {
            return;
        }
        size_t size = seq_end - seq_begin;
        for (size_t group_size_pow = 0; static_cast<size_t>(1) << group_size_pow < size; ++group_size_pow) {
            std::vector<size_t> tasks_in_threadpool;
            for (size_t i = 0; i < size; i += static_cast<size_t>(1) << (group_size_pow + 1)) {
                size_t left = i;
                size_t right = std::min(i + (static_cast<size_t>(1) << group_size_pow), size);
                size_t end = std::min(i + (static_cast<size_t>(1) << (group_size_pow + 1)), size);
                size_t task_id = pool.add_task([seq_begin, left, right, end]() {
                    std::inplace_merge(seq_begin + left, seq_begin + right, seq_begin + end);
                });
                tasks_in_threadpool.push_back(task_id);
            }

            pool.wait_for_task_list(tasks_in_threadpool);
        }

    }

private:
    ThreadPool::Pool& pool;
};


#endif //ASYNC_SERVER_EXAMPLE_ASYNCMERGESORT_H
