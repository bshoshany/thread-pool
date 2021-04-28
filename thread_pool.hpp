#pragma once

/**
 * @file thread_pool.hpp
 * @author Barak Shoshany (baraksh@gmail.com) (http://baraksh.com)
 * @version 1.1
 * @date 2021-04-24
 * @copyright Copyright (c) 2021 Barak Shoshany. Licensed under the MIT license.
 *
 * @brief A simple but powerful C++17 thread pool class. Please visit the GitHub repository at https://github.com/bshoshany/thread-pool for documentation and updates, or to submit feature requests and bug reports.
 */

#include <algorithm>   // std::max
#include <atomic>      // std::atomic
#include <cstdint>     // std::uint_fast32_t
#include <functional>  // std::function
#include <future>      // std::promise
#include <iostream>    // std::cout, std::ostream
#include <memory>      // std::shared_ptr, std::unique_ptr
#include <mutex>       // std::mutex, std::scoped_lock
#include <queue>       // std::queue
#include <thread>      // std::this_thread, std::thread
#include <type_traits> // std::decay_t, std::enable_if_t, std::is_void_v, std::invoke_result_t
#include <utility>     // std::move, std::swap

/**
 * @brief A simple but powerful thread pool class. Maintains a queue of tasks, which are executed by threads in the pool as they become available.
 */
class thread_pool
{
    typedef std::uint_fast32_t ui32;

public:
    // ============================
    // Constructors and destructors
    // ============================

    /**
     * @brief Construct a new thread pool.
     *
     * @param _thread_count The number of threads to use. Default value is the total number of hardware threads available, as reported by the implementation. With a hyperthreaded CPU, this will be twice the number of CPU cores. If the argument is zero, 1 thread will be used.
     */
    thread_pool(const ui32 &_thread_count = std::thread::hardware_concurrency()-1)
        : thread_count(std::max<ui32>(_thread_count, 1)), threads(new std::thread[std::max<ui32>(_thread_count, 1)])
    {
        create_threads();
    }

    /**
     * @brief Destruct the thread pool. Waits for all submitted tasks to be completed, then destroys all threads.
     */
    ~thread_pool()
    {
        wait_for_tasks();
        running = false;
        destroy_threads();
    }

    // =======================
    // Public member functions
    // =======================

    /**
     * @brief Get the number of threads in the pool.
     *
     * @return The number of threads.
     */
    ui32 get_thread_count() const
    {
        return thread_count;
    }

    /**
     * @brief Parallelize a loop by splitting it into blocks, submitting each block separately to the thread pool, and waiting for all blocks to finish executing. The loop will be equivalent to "for (T i = first_index; i <= last_index; i++) loop(i);".
     *
     * @tparam T The type of the loop index. Should be a signed or unsigned integer.
     * @tparam F The type of the function to loop through.
     * @param first_index The first index in the loop (inclusive).
     * @param last_index The last index in the loop (inclusive).
     * @param loop The function to loop through. Should take exactly one argument, the loop index.
     * @param num_tasks The maximum number of tasks to split the loop into. Default is to use the number of threads in the pool.
     */
    template <typename T, typename F>
    void parallelize_loop(T first_index, T last_index, const F &loop, ui32 num_tasks = 0)
    {
        if (num_tasks == 0)
            num_tasks = thread_count;
        if (last_index < first_index)
            std::swap(last_index, first_index);
        size_t total_size = last_index - first_index + 1;
        size_t block_size = total_size / num_tasks;
        if (block_size == 0)
        {
            block_size = 1;
            num_tasks = std::max((ui32)1, (ui32)total_size);
        }
        std::atomic<ui32> blocks_running = 0;
        for (ui32 t = 0; t < num_tasks; t++)
        {
            T start = (T)(t * block_size + first_index);
            T end = (t == num_tasks - 1) ? last_index : (T)((t + 1) * block_size + first_index - 1);
            std::cout << start << '-' << end << '\n';
            blocks_running++;
            push_task([&start, &end, &loop, &blocks_running] {
                for (T i = start; i <= end; i++)
                    loop(i);
                blocks_running--;
            });
            while (blocks_running != 0)
            {
                std::this_thread::yield();
            }
        }
    }

    /**
     * @brief Push a function with no arguments or return value into the task queue.
     *
     * @tparam F The type of the function.
     * @param task The function to push.
     */
    template <typename F>
    void push_task(const F &task)
    {
        tasks_waiting++;
        {
            const std::scoped_lock lock(queue_mutex);
            tasks.push(std::move(std::function<void()>(task)));
        }
    }

    /**
     * @brief Push a function with arguments, but no return value, into the task queue.
     * @details The function is wrapped inside a lambda in order to hide the arguments, as the tasks in the queue must be of type std::function<void()>, so they cannot have any arguments or return value. If no arguments are provided, the other overload will be used, in order to avoid the (slight) overhead of using a lambda.
     *
     * @tparam F The type of the function.
     * @tparam A The types of the arguments.
     * @param task The function to push.
     * @param args The arguments to pass to the function.
     */
    template <typename F, typename... A>
    void push_task(const F &task, const A &...args)
    {
        push_task([task, args...] { task(args...); });
    }

    /**
     * @brief Reset the number of threads in the pool. Waits for all submitted tasks to be completed, then destroys all threads and creates a new thread pool with the new number of threads.
     *
     * @param _thread_count The number of threads to use. Default value is the total number of hardware threads available, as reported by the implementation. With a hyperthreaded CPU, this will be twice the number of CPU cores. If the argument is zero, 1 thread will be used.
     */
    void reset(const ui32 &_thread_count = std::thread::hardware_concurrency())
    {
        wait_for_tasks();
        running = false;
        destroy_threads();
        thread_count = std::max<ui32>(_thread_count, 1);
        threads.reset(new std::thread[std::max<ui32>(_thread_count, 1)]);
        running = true;
        create_threads();
    }

    /**
     * @brief Submit a function with zero or more arguments and no return value into the task queue, and get an std::future<bool> that will be set to true upon completion of the task.
     *
     * @tparam F The type of the function.
     * @tparam A The types of the zero or more arguments to pass to the function.
     * @param task The function to submit.
     * @param args The zero or more arguments to pass to the function.
     * @return A future to be used later to check if the function has finished its execution.
     */
    template <typename F, typename... A, typename = std::enable_if_t<std::is_void_v<std::invoke_result_t<std::decay_t<F>, std::decay_t<A>...>>>>
    std::future<bool> submit(const F &task, const A &...args)
    {
        std::shared_ptr<std::promise<bool>> promise(new std::promise<bool>);
        std::future<bool> future = promise->get_future();
        push_task([task, args..., promise] {
            task(args...);
            promise->set_value(true);
        });
        return future;
    }

    /**
     * @brief Submit a function with zero or more arguments and a return value into the task queue, and get a future for its eventual returned value.
     *
     * @tparam F The type of the function.
     * @tparam A The types of the zero or more arguments to pass to the function.
     * @tparam R The return type of the function.
     * @param task The function to submit.
     * @param args The zero or more arguments to pass to the function.
     * @return A future to be used later to obtain the function's returned value, waiting for it to finish its execution if needed.
     */
    template <typename F, typename... A, typename R = std::invoke_result_t<std::decay_t<F>, std::decay_t<A>...>, typename = std::enable_if_t<!std::is_void_v<R>>>
    std::future<R> submit(const F &task, const A &...args)
    {
        std::shared_ptr<std::promise<R>> promise(new std::promise<R>);
        std::future<R> future = promise->get_future();
        push_task([task, args..., promise] {
            promise->set_value(task(args...));
        });
        return future;
    }

    /**
     * @brief Wait for all submitted tasks to be completed - both those that are currently being executed by threads, and those that are still waiting in the queue. To wait for a specific task, use push_task_future instead.
     */
    void wait_for_tasks()
    {
        while (tasks_waiting != 0)
        {
            std::this_thread::yield();
        }
    }

private:
    // ========================
    // Private member functions
    // ========================

    /**
     * @brief Create the threads in the pool and assign a worker to each thread.
     */
    void create_threads()
    {
        for (ui32 i = 0; i < thread_count; i++)
        {
            threads[i] = std::thread(&thread_pool::worker, this);
        }
    }

    /**
     * @brief Destroy the threads in the pool by joining them.
     */
    void destroy_threads()
    {
        for (ui32 i = 0; i < thread_count; i++)
        {
            threads[i].join();
        }
    }

    /**
     * @brief Try to pop a new task out of the queue.
     *
     * @param task A reference to the task. Will be populated with a function if the queue is not empty.
     * @return true if a task was found, false if the queue is empty.
     */
    bool pop_task(std::function<void()> &task)
    {
        const std::scoped_lock lock(queue_mutex);
        if (tasks.empty())
            return false;
        else
        {
            task = std::move(tasks.front());
            tasks.pop();
            return true;
        }
    }

    /**
     * @brief A worker function to be assigned to each thread in the pool. Pops tasks out of the queue and executes them, until the atomic variable running is set to false.
     */
    void worker()
    {
        while (running)
        {
            std::function<void()> task;
            if (pop_task(task))
            {
                task();
                tasks_waiting--;
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }

    // ============
    // Private data
    // ============

    /**
     * @brief An atomic variable indicating to the workers to keep running.
     */
    std::atomic<bool> running = true;

    /**
     * @brief An atomic variable to keep track of how many tasks are currently waiting to finish - either still in the queue, or running in a thread.
     */
    std::atomic<ui32> tasks_waiting = 0;

    /**
     * @brief A mutex to synchronize access to the task queue by different threads.
     */
    mutable std::mutex queue_mutex;

    /**
     * @brief A queue of tasks to be executed by the threads.
     */
    std::queue<std::function<void()>> tasks;

    /**
     * @brief The number of threads in the pool.
     */
    ui32 thread_count;

    /**
     * @brief A smart pointer to manage the memory allocated for the threads.
     */
    std::unique_ptr<std::thread[]> threads;
};

/**
 * @brief A class to synchronize printing to an output stream by different threads.
 */
class synced_stream
{
public:
    /**
     * @brief Construct a new synced stream.
     *
     * @param _out_stream The output stream to sync to. Default is std::cout.
     */
    synced_stream(std::ostream &_out_stream = std::cout)
        : out_stream(_out_stream){};

    /**
     * @brief Print any number of items into the output stream. Ensures that no other threads print to this stream simultaneously, as long as they all use this synced_stream object to print.
     *
     * @tparam T The types of the items
     * @param items The items to print.
     */
    template <typename... T>
    void print(const T &...items)
    {
        const std::scoped_lock lock(stream_mutex);
        (out_stream << ... << items);
    }

    /**
     * @brief Print any number of items into the output stream, followed by a newline character. Ensures that no other threads print to this stream simultaneously, as long as they all use this synced_stream object to print.
     *
     * @tparam T The types of the items
     * @param items The items to print.
     */
    template <typename... T>
    void println(const T &...items)
    {
        print(items..., '\n');
    }

private:
    /**
     * @brief A mutex to synchronize printing.
     */
    mutable std::mutex stream_mutex;

    /**
     * @brief The output stream to print to.
     */
    std::ostream &out_stream;
};
