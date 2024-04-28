//
// Created by Dmitriy Zolotykh on 27.04.2024.
//

#include "multithreading/ThreadPool.h"

Server::Multithreading::ThreadPool::ThreadPool(size_t threads_count) : threads(threads_count) {
    for (size_t i = 0; i < threads_count; ++i) {
        threads[i] = std::thread([this] {
            while (!stop.load()) {
                solve_task();
            }
        });
    }
}

void Server::Multithreading::ThreadPool::solve_task() {
    std::unique_lock<std::mutex> lock(q_mtx);
    q_cv.wait(lock, [this] { return !tasks.empty(); });
    Task task = std::move(tasks.front());
    tasks.pop();
    lock.unlock();
    task();
}

Server::Multithreading::ThreadPool::~ThreadPool() {
    stop.store(true);
    q_cv.notify_all();
    for (auto& thread : threads) {
        thread.detach();
    }
}
