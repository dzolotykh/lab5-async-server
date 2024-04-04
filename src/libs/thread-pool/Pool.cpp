#include "Pool.h"
#include <condition_variable>
#include <exception>

void ThreadPool::Pool::run_worker(int id) {
    while (!stop_flag) {
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
        tasks_in_queue--; // уменьшаем счетчик задач
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
    tasks_in_queue++; // увеличиваем счетчик задач
    q_cv.notify_one(); // оповещаем один из свободных потоков, что появилась задача
}

size_t ThreadPool::Pool::size() {
    return tasks_in_queue.load();
}

ThreadPool::Pool::~Pool() {
    if (policy == destructor_policy::JOIN) {
        wait_all();
    }
    stop_flag = true;
    q_cv.notify_all();
    for (auto &i : all_threads) {
        i.detach();
    }
}

void ThreadPool::Pool::update_status(int id, ThreadPool::Pool::worker_status st) {
    std::lock_guard<std::mutex> lock(status_mutex);
    thread_status[id] = st;
    if (st == worker_status::WAITING) {
        waiting_threads++;
    } else {
        waiting_threads--;
    }
    status_cv.notify_all();
}

// метод блокирует вызвавший его поток до тех пор, пока все задачи из очереди не закончатся
void ThreadPool::Pool::wait_all() {
    std::unique_lock<std::mutex> lock(status_mutex);
    status_cv.wait(lock, [this](){ return (waiting_threads == num_threads && tasks_in_queue == 0);});
}


