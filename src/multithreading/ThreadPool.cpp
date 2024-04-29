//
// Created by Dmitriy Zolotykh on 27.04.2024.
//

#include "multithreading/ThreadPool.h"
#include <iostream>

Server::Multithreading::ThreadPool::ThreadPool(size_t _threads_count)
    : threads(_threads_count), threads_count(_threads_count) {
    std::cout << _threads_count << std::endl;
    for (size_t i = 0; i < threads_count; ++i) {
        threads[i] = std::thread([this] {
            while (!stop) {
                solve_task();
            }
        });
    }
}

void Server::Multithreading::ThreadPool::solve_task() {
    std::unique_lock<std::mutex> lock(q_mtx);
    q_cv.wait(lock, [this] { return (!tasks.empty() || stop); });
    if (stop) {
        return;
    }
    Task task = std::move(tasks.front());
    tasks.pop();
    lock.unlock();
    task();
}

Server::Multithreading::ThreadPool::~ThreadPool() {
    stop = true;
    q_cv.notify_all();
    for (auto& thread : threads) {
        thread.join();
    }
}
