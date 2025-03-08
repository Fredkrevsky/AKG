#pragma once
#include <vector>
#include <queue>
#include <future>
#include <functional>
#include <memory>
#include <thread>
#include <algorithm>
#include <condition_variable>
#include <any>
#include <optional>

using task_t = std::packaged_task<std::any()>;

class ThreadPool {
public:
    explicit ThreadPool(int threads_count) noexcept;
    ~ThreadPool();

    template<typename F, typename ... Args>
    std::future<std::any> add_task(F&& f, Args&&... args);

    void stop();

private:
    void worker_thread();
    std::optional<task_t> get_task();

    std::queue<task_t> m_tasks;
    std::vector<std::thread> m_threads;

    std::mutex m_mtx;
    std::condition_variable m_cv;
    std::atomic<bool> m_end{ false };
};

template<typename F, typename ...Args>
std::future<std::any> ThreadPool::add_task(F && f, Args && ...args)
{    
    using return_t = decltype(f(args...));

    auto lambda_task = 
    [f = std::forward<F>(f), ...args = std::forward<Args>(args)] () -> std::any 
    {
        if constexpr (std::is_void_v<return_t>){
            f(args...);
            return {};
        }
        else {
            return std::any(f(args...));
        }
    };

    auto task = task_t(std::move(lambda_task));
    std::future<std::any> result = task.get_future();
    {
        std::lock_guard<std::mutex> l{m_mtx};
        m_tasks.push(std::move(task));
    }
    m_cv.notify_one();
    return result;
}