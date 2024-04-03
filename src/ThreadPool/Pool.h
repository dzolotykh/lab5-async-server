#ifndef LAB5_POOL_H
#define LAB5_POOL_H

#include <functional>
#include <queue>
#include <mutex>

using Task = std::function<void()>;

namespace ThreadPool {
    template<size_t num_threads>
    class Pool {
        friend class PoolWorker;
    private:
        std::mutex q_lock;
        std::queue<Task> q;
        std::array<std::thread, num_threads> all_threads;

        Task get_task() {
            std::lock_guard _(q_lock);
            if (q.empty()) {
                return nullptr;
            }
            Task t = q.front();
            q.pop();
            return t;
        }
    public:
        Pool() {
            for (size_t i = 0; i < num_threads; ++i) {
                all_threads[i] = std::thread();
            }
        }

        void add_task(const Task &t) {
            q_lock.lock();
            q.push(t);
            q_lock.unlock();
        }

        size_t size() {
            q_lock.lock();
            size_t sz = q.size();
            q_lock.unlock();
            return sz;
        }

    };
}


#endif //LAB5_POOL_H
