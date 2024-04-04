#include "Pool.h"
#include <condition_variable>
#include <exception>

void ThreadPool::Pool::run_worker(int id) {
    while (true) {
        std::unique_lock<std::mutex> lock(q_mutex); // блокируем мьютекс для работы с очередью
        q_cv.wait(lock, [this](){ return (!q.empty() || stop_flag);}); // ждем, пока не появится задача или не придет сигнал на завершение
        if (stop_flag) { // если пришел сигнал на завершение, завершаем работу
            lock.unlock(); // так как condition variable блокирует мьютекс после выхода из wait, нужно его разблокировать
            update_status(id, worker_status::WAITING); // обновляем статус потока
            return;
        }
        update_status(id, worker_status::ACTIVE); // поток взял на себя задачу
        ThreadPool::Task t = std::move(q.front()); // забираем задачу из очереди
        q.pop(); // удаляем задачу из очереди
        lock.unlock(); // разблокируем мьютекс (чтобы другие потоки могли работать с очередью)
        t(); // выполняем задачу
        update_status(id, worker_status::WAITING); // выполнили задачу, обновляем статус
    }
}

ThreadPool::Pool::Pool(int _num_threads, ThreadPool::Pool::destructor_policy _policy): all_threads(_num_threads), num_threads(_num_threads), policy(_policy) {
    for (int i = 0; i < num_threads; ++i) {
        all_threads[i] = std::thread([this, i]() {
            ThreadPool::Pool::run_worker(i);
        });
        update_status(i, worker_status::WAITING);
    }
}

void ThreadPool::Pool::add_task(const ThreadPool::Task &t) {
    std::lock_guard<std::mutex> lock(q_mutex); // блокируем очередь, чтобы добавить туда задачу
    q.push(t); // добавляем задачу в очередь
    q_cv.notify_one(); // оповещаем один из свободных потоков, что появилась задача
}

size_t ThreadPool::Pool::size() {
    std::lock_guard<std::mutex> lock(q_mutex); // блокируем очередь, чтобы узнать ее размер
    return q.size();
}

/*
 * Грубо обрывает всю работу потоков. Поэтому, пока что destruction_policy ни на что не влияет
 * В будущем в зависимости от этого будет или приостанавливаться взятие новых задач,
 * или завершаться как есть сейчас
 */
ThreadPool::Pool::~Pool() {
    stop_flag = true;
    q_cv.notify_all();
    for (auto &i : all_threads) {
        if (policy == destructor_policy::JOIN) {
            i.join();
        } else {
            i.detach();
        }
    }
}

void ThreadPool::Pool::update_status(int id, ThreadPool::Pool::worker_status st) {
    std::lock_guard<std::mutex> lock(status_mutex);
    thread_status[id] = st;
}

// TODO реализовать
void ThreadPool::Pool::join_all() {
    throw std::logic_error("function not implemented");
}


