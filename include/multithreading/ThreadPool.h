#ifndef ASYNC_SERVER_EXAMPLE_THREADPOOL_H
#define ASYNC_SERVER_EXAMPLE_THREADPOOL_H

#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <mutex>
#include <optional>
#include <queue>
#include <utility>


namespace Server::Multithreading {
class ThreadPool {
   public:
    class Task {
       public:
        template <typename F, typename... Args>
        Task(F&& _task,
             std::shared_ptr<std::promise<decltype(_task(std::declval<Args>()...))>>&& promise_ptr,
             Args&&... args) {
            task = [_task = std::forward<F>(_task), pr = std::move(promise_ptr),
                    args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
                try {
                    if constexpr (std::is_void_v<decltype(std::apply(_task, args))>) {
                        std::apply(_task, args);
                        pr->set_value();
                    } else {
                        pr->set_value(std::apply(_task, args));
                    }
                } catch (...) {
                    pr->set_exception(std::current_exception());
                }
            };
        }

        void operator()() { task(); }

       private:
        std::function<void()> task;
    };

    ThreadPool() = delete;
    ThreadPool(const ThreadPool& other) = delete;
    ThreadPool& operator=(const ThreadPool& other) = delete;

    ThreadPool(ThreadPool&& other) noexcept =
        delete;    // пока мне это не надо, поэтому не сделал :)
    ThreadPool& operator=(ThreadPool&& other) noexcept = delete;

    explicit ThreadPool(size_t _threads_count);

    template <typename F, typename... Args>
    auto add_task(F task, const Args&... args) -> std::future<decltype(task(args...))> {
        using return_t = decltype(task(args...));
        auto promise = std::make_shared<std::promise<return_t>>();
        auto future = promise->get_future();
        Task t(task, std::move(promise), args...);
        std::lock_guard<std::mutex> lock(q_mtx);
        tasks.push(std::move(t));
        q_cv.notify_one();
        return future;
    }

    ~ThreadPool();

   private:
    std::queue<Task> tasks;
    std::mutex q_mtx{};
    std::condition_variable q_cv{};

    std::vector<std::thread> threads;
    size_t threads_count;
    std::atomic<bool> stop = false;
    void solve_task();
};
}    // namespace Server::Multithreading

#endif    //ASYNC_SERVER_EXAMPLE_THREADPOOL_H
