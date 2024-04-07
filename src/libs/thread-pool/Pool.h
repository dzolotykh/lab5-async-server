#ifndef LAB5_POOL_H
#define LAB5_POOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <unordered_map>

namespace ThreadPool {

using Task = std::function<void()>;

class Pool {
   private:
    enum class worker_status { WAITING, ACTIVE };

    const int num_threads;
    std::vector<std::thread> all_threads;

    std::mutex q_mutex;
    std::condition_variable q_cv;
    std::queue<Task> q;
    std::atomic<size_t> tasks_in_queue = 0;

    std::atomic<bool> stop_flag = false;

    std::unordered_map<int, worker_status> thread_status;
    std::mutex status_mutex;
    std::atomic<int> waiting_threads = 0;
    std::condition_variable status_cv;

    void run_worker(int id);

    void update_status(int id,
                       worker_status st);    // потокобезопасно обновляет статус потока
   public:
    enum class destructor_policy {
        JOIN,    // перед деструктором будем ждать завершения всех задач
        DETACH    // грубо все оборвем
    };

    destructor_policy policy;

    Pool(int _num_threads, destructor_policy _policy = destructor_policy::JOIN);

    void add_task(const Task& t);

    void wait_all();
    void detach_all();

    size_t size();

    ~Pool();
};

}    // namespace ThreadPool

#endif    // LAB5_POOL_H
