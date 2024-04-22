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

struct Task {
    std::function<void()> run;
    size_t id;
    std::mutex mtx;
    std::condition_variable cv;
    bool finished = false;

    Task(std::function<void()> _run, size_t _id) : run(_run), id(_id) {}
};

class Pool {
   private:
    enum class worker_status { WAITING, ACTIVE };

    const int num_threads;
    std::vector<std::thread> all_threads;

    std::mutex q_mutex;
    std::condition_variable q_cv;
    std::queue<std::shared_ptr<Task>> q;
    std::atomic<size_t> tasks_in_queue = 0;
    std::atomic<size_t> tasks_added = 0;
    std::unordered_map<size_t, std::shared_ptr<Task>> task_waiter;

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

    [[nodiscard]] size_t add_task(const std::function<void()>& t);

    void wait_all();
    void detach_all();

    size_t size();

    void wait_for_task(size_t id);

    template<typename T>
    void wait_for_task_list(const T& task_list) {
        for (auto& i : task_list) {
            wait_for_task(i);
        }
    }

    ~Pool();
};

}    // namespace ThreadPool

#endif    // LAB5_POOL_H
