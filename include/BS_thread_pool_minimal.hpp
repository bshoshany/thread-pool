#pragma once

#define BS_THREAD_POOL_MINIMAL_VERSION "v1.0.0 (2023-08-25)"

#include <condition_variable> // std::condition_variable
#include <functional>         // std::function
#include <mutex>              // std::mutex, std::unique_lock
#include <queue>              // std::queue
#include <type_traits>        // std::invoke_result_t
#include <thread>             // std::thread

namespace BS
{
using concurrency_t = std::invoke_result_t<decltype(std::thread::hardware_concurrency)>;
struct [[nodiscard]] thread_pool_minimal
{
    std::condition_variable task_available_cv = {}, tasks_done_cv = {};
    std::queue<std::function<void()>> tasks = {};
    size_t tasks_running = 0;
    mutable std::mutex tasks_mutex = {};
    concurrency_t thread_count = 0;
    std::thread* threads;
    bool waiting = false, workers_running = true;

    thread_pool_minimal(const concurrency_t thread_count_ = 0)
    {
        if (thread_count_ > 0)
            thread_count = thread_count_;
        else
        {
            if (std::thread::hardware_concurrency() > 0)
                thread_count = std::thread::hardware_concurrency();
            else
                thread_count = 1;
        }
        threads = reinterpret_cast<std::thread*>(malloc(sizeof(std::thread) * thread_count));
        for (concurrency_t i = 0; i < thread_count; ++i)
            threads[i] = std::thread(&thread_pool_minimal::worker, this);
    }
    
    ~thread_pool_minimal()
    {
        std::unique_lock tasks_lock(tasks_mutex);
        waiting = true;
        tasks_done_cv.wait(tasks_lock, [this] { return !tasks_running && tasks.empty(); });
        workers_running = false;
        tasks_lock.unlock();
        task_available_cv.notify_all();
        for (concurrency_t i = 0; i < thread_count; ++i)
            threads[i].join();
        std::destroy_n(threads, thread_count);
        free(threads);
    }

#ifdef BS_THREAD_POOL_MINIMAL_ENABLE_WAIT_FOR_TASKS
    void wait_for_tasks()
    {
        std::unique_lock tasks_lock(tasks_mutex);
        waiting = true;
        tasks_done_cv.wait(tasks_lock, [this] { return !tasks_running && tasks.empty(); });
        waiting = false;
    }
#endif

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
};
} // namespace BS