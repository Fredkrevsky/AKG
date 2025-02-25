#include "ThreadPool.hpp"
#include <algorithm>

ThreadPool::ThreadPool(int threads_count) noexcept 
    : m_end(false) 
{
    m_threads.reserve(threads_count);

    for (int i = 0; i < threads_count; ++i) {
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
    m_end = true;
    m_cv.notify_all();
}

void ThreadPool::worker_thread() {
    while (true) {
        auto task_optional = get_task();
        if (!task_optional.has_value()){
            break;
        }
        auto task = std::move(task_optional.value());
        task();
    }
}

std::optional<task_t> ThreadPool::get_task() {
    std::unique_lock<std::mutex> lock(m_mtx);

    m_cv.wait(lock, [this]() {
        return !m_tasks.empty() || m_end;
    });

    if (m_end && m_tasks.empty()) {
        return {};
    }

    auto task = std::move(m_tasks.front());
    m_tasks.pop();

    return std::optional<task_t>{std::move(task)};
}
