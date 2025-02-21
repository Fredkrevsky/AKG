#include "ThreadPool.hpp"
#include <algorithm>

ThreadPool::ThreadPool(int threads_count) noexcept 
    : m_threads_count(threads_count)
    , m_end(false) 
{
    m_threads.reserve(m_threads_count);

    for (int i = 0; i < m_threads_count; ++i) {
        m_threads.emplace_back([this] { this->worker_thread(); });
    }
}

ThreadPool::~ThreadPool() {
    stop();

    std::ranges::for_each(m_threads, [](auto& thread){
        if (thread.joinable()) thread.join();
    });
}

void ThreadPool::stop() {
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_end = true;
    }
    m_cv.notify_all();
}

void ThreadPool::worker_thread() {
    task_t task;

    while (true) {
        if (!get_task(task)) {
            break;
        }
        task();
    }
}

bool ThreadPool::get_task(task_t& task) {
    std::unique_lock<std::mutex> lock(m_mtx);

    m_cv.wait(lock, [this]() {
        return !m_tasks.empty() || m_end;
    });

    if (m_end && m_tasks.empty()) {
        return false;
    }

    task = std::move(m_tasks.front());
    m_tasks.pop();

    return true;
}
