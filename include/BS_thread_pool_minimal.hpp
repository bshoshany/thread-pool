#pragma once

#define BS_THREAD_POOL_MINIMAL_VERSION "v1.0.0 (2023-08-25)"

#include <condition_variable> // std::condition_variable
#include <functional>         // std::function
#include <memory>             // std::make_unique, std::unique_ptr
#include <mutex>              // std::mutex, std::unique_lock
#include <queue>              // std::queue
#include <thread>             // std::thread

namespace BS
{
struct [[nodiscard]] thread_pool_minimal
{
    std::condition_variable task_available_cv = {}, tasks_done_cv = {};
    std::queue<std::function<void()>> tasks = {};
    size_t tasks_running = 0;
    mutable std::mutex tasks_mutex = {};
    unsigned int thread_count = 0;
    std::unique_ptr<std::thread[]> threads = nullptr;
    bool waiting = false, workers_running = true;
    
    thread_pool_minimal(const unsigned int thread_count_ = 0) : thread_count(determine_thread_count(thread_count_)), threads(std::make_unique<std::thread[]>(determine_thread_count(thread_count_)))
    {
        for (unsigned int i = 0; i < thread_count; ++i)
        {
            threads[i] = std::thread(&thread_pool_minimal::worker, this);
        }
    }

    ~thread_pool_minimal()
    {
        std::unique_lock tasks_lock(tasks_mutex);
        waiting = true;
        tasks_done_cv.wait(tasks_lock, [this] { return !tasks_running && tasks.empty(); });
        workers_running = false;
        tasks_lock.unlock();
        task_available_cv.notify_all();
        for (unsigned int i = 0; i < thread_count; ++i)
        {
            threads[i].join();
        }
    }
    
    void wait_for_tasks()
    {
        std::unique_lock tasks_lock(tasks_mutex);
        waiting = true;
        tasks_done_cv.wait(tasks_lock, [this] { return !tasks_running && tasks.empty(); });
        waiting = false;
    }

private:
    
    void worker()
    {
        std::function<void()> task;
        while (true)
        {
            std::unique_lock tasks_lock(tasks_mutex);
            task_available_cv.wait(tasks_lock, [this] { return !tasks.empty() || !workers_running; });
            if (!workers_running)
                break;
            task = std::move(tasks.front());
            tasks.pop();
            ++tasks_running;
            tasks_lock.unlock();
            task();
            tasks_lock.lock();
            --tasks_running;
            if (waiting && !tasks_running && tasks.empty())
                tasks_done_cv.notify_all();
        }
    }
    
    [[nodiscard]] unsigned int determine_thread_count(const unsigned int thread_count_) const
    {
        if (thread_count_ > 0)
            return thread_count_;
        else
        {
            if (std::thread::hardware_concurrency() > 0)
                return std::thread::hardware_concurrency();
            else
                return 1;
        }
    }
};
} // namespace BS