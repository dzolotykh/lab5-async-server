#ifndef LAB5_POOL_H
#define LAB5_POOL_H

#include <functional>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <unordered_map>

namespace ThreadPool {

    using Task = std::function<void()>;
    class Pool {
    private:
        enum class worker_status {
            WAITING,
            ACTIVE
        };

        const int num_threads;
        std::mutex q_mutex;
        std::condition_variable q_cv;
        std::queue<Task> q;
        std::vector<std::thread> all_threads;
        std::atomic<bool> stop_flag = false;

        std::unordered_map<int, worker_status> thread_status;
        std::mutex status_mutex;

        void run_worker(int id);

        void update_status(int id, worker_status st); // потокобезопасно обновляет статус потока
    public:
        enum class destructor_policy {
            JOIN,
            DETACH
        };

        destructor_policy policy;

        Pool(int _num_threads, destructor_policy _policy = destructor_policy::JOIN);

        void add_task(const Task &t);

        void join_all();

        size_t size();

        ~Pool();
    };

}


#endif //LAB5_POOL_H
