#pragma once

/**
 * @file BS_thread_pool_utils.hpp
 * @author Barak Shoshany (baraksh@gmail.com) (http://baraksh.com)
 * @version 4.0.1
 * @date 2023-12-28
 * @copyright Copyright (c) 2023 Barak Shoshany. Licensed under the MIT license. If you found this project useful, please consider starring it on GitHub! If you use this library in software of any kind, please provide a link to the GitHub repository https://github.com/bshoshany/thread-pool in the source code and documentation. If you use this library in published research, please cite it as follows: Barak Shoshany, "A C++17 Thread Pool for High-Performance Scientific Computing", doi:10.5281/zenodo.4742687, arXiv:2105.00613 (May 2021)
 *
 * @brief BS::thread_pool: a fast, lightweight, and easy-to-use C++17 thread pool library. This header file contains independent utility classes that are part of the library, but are not needed to use the thread pool itself.
 */

#include <atomic>           // std::atomic
#include <chrono>           // std::chrono
#include <future>           // std::promise, std::shared_future
#include <initializer_list> // std::initializer_list
#include <iostream>         // std::cout
#include <memory>           // std::make_unique, std::unique_ptr
#include <mutex>            // std::mutex, std::scoped_lock
#include <ostream>          // std::endl, std::flush, std::ostream
#include <utility>          // std::forward

/**
 * @brief A namespace used by Barak Shoshany's projects.
 */
namespace BS {
// Macros indicating the version of the thread pool utilities library.
#define BS_THREAD_POOL_UTILS_VERSION_MAJOR 4
#define BS_THREAD_POOL_UTILS_VERSION_MINOR 0
#define BS_THREAD_POOL_UTILS_VERSION_PATCH 1

/**
 * @brief A utility class to allow simple signalling between threads.
 */
class [[nodiscard]] signaller
{
public:
    /**
     * @brief Construct a new signaller.
     */
    signaller() : promise(), future(promise.get_future()) {}

    // The copy constructor and copy assignment operator are deleted. The signaller works using a promise, which cannot be copied.
    signaller(const signaller&) = delete;
    signaller& operator=(const signaller&) = delete;

    // The move constructor and move assignment operator are defaulted.
    signaller(signaller&&) = default;
    signaller& operator=(signaller&&) = default;

    /**
     * @brief Inform any waiting threads that the signaller is ready.
     */
    void ready()
    {
        promise.set_value();
    }

    /**
     * @brief Wait until the signaller is ready.
     */
    void wait()
    {
        future.wait();
    }

private:
    /**
     * @brief A promise used to set the state of the signaller.
     */
    std::promise<void> promise;

    /**
     * @brief A future used to wait for the signaller.
     */
    std::shared_future<void> future;
}; // class signaller

/**
 * @brief A utility class to synchronize printing to an output stream by different threads.
 */
class [[nodiscard]] synced_stream
{
public:
    /**
     * @brief Construct a new synced stream.
     *
     * @param stream The output stream to print to. The default value is `std::cout`.
     */
    explicit synced_stream(std::ostream& stream = std::cout) : out_stream(stream) {}

    // The copy and move constructors and assignment operators are deleted. The synced stream uses a mutex, which cannot be copied or moved.
    synced_stream(const synced_stream&) = delete;
    synced_stream(synced_stream&&) = delete;
    synced_stream& operator=(const synced_stream&) = delete;
    synced_stream& operator=(synced_stream&&) = delete;

    /**
     * @brief Print any number of items into the output stream. Ensures that no other threads print to this stream simultaneously, as long as they all exclusively use the same `synced_stream` object to print.
     *
     * @tparam T The types of the items.
     * @param items The items to print.
     */
    template <typename... T>
    void print(T&&... items)
    {
        const std::scoped_lock stream_lock(stream_mutex);
        (out_stream << ... << std::forward<T>(items));
    }

    /**
     * @brief Print any number of items into the output stream, followed by a newline character. Ensures that no other threads print to this stream simultaneously, as long as they all exclusively use the same `synced_stream` object to print.
     *
     * @tparam T The types of the items.
     * @param items The items to print.
     */
    template <typename... T>
    void println(T&&... items)
    {
        print(std::forward<T>(items)..., '\n');
    }

    /**
     * @brief A stream manipulator to pass to a `synced_stream` (an explicit cast of `std::endl`). Prints a newline character to the stream, and then flushes it. Should only be used if flushing is desired, otherwise a newline character should be used instead.
     */
    inline static std::ostream& (&endl)(std::ostream&) = static_cast<std::ostream& (&)(std::ostream&)>(std::endl);

    /**
     * @brief A stream manipulator to pass to a `synced_stream` (an explicit cast of `std::flush`). Used to flush the stream.
     */
    inline static std::ostream& (&flush)(std::ostream&) = static_cast<std::ostream& (&)(std::ostream&)>(std::flush);

private:
    /**
     * @brief The output stream to print to.
     */
    std::ostream& out_stream;

    /**
     * @brief A mutex to synchronize printing.
     */
    mutable std::mutex stream_mutex = {};
}; // class synced_stream

/**
 * @brief A utility class to measure execution time for benchmarking purposes.
 */
class [[nodiscard]] timer
{
public:
    /**
     * @brief Construct a new timer and immediately start measuring time.
     */
    timer() = default;

    /**
     * @brief Get the number of milliseconds that have elapsed since the object was constructed or since `start()` was last called, but keep the timer ticking.
     *
     * @return The number of milliseconds.
     */
    [[nodiscard]] std::chrono::milliseconds::rep current_ms() const
    {
        return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time)).count();
    }

    /**
     * @brief Start (or restart) measuring time. Note that the timer starts ticking as soon as the object is created, so this is only necessary if we want to restart the clock later.
     */
    void start()
    {
        start_time = std::chrono::steady_clock::now();
    }

    /**
     * @brief Stop measuring time and store the elapsed time since the object was constructed or since `start()` was last called.
     */
    void stop()
    {
        elapsed_time = std::chrono::steady_clock::now() - start_time;
    }

    /**
     * @brief Get the number of milliseconds stored when `stop()` was last called.
     *
     * @return The number of milliseconds.
     */
    [[nodiscard]] std::chrono::milliseconds::rep ms() const
    {
        return (std::chrono::duration_cast<std::chrono::milliseconds>(elapsed_time)).count();
    }

private:
    /**
     * @brief The time point when measuring started.
     */
    std::chrono::time_point<std::chrono::steady_clock> start_time = std::chrono::steady_clock::now();

    /**
     * @brief The duration that has elapsed between `start()` and `stop()`.
     */
    std::chrono::duration<double> elapsed_time = std::chrono::duration<double>::zero();
}; // class timer
} // namespace BS
