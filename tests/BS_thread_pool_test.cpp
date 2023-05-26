/**
 * @file BS_thread_pool_test.cpp
 * @author Barak Shoshany (baraksh@gmail.com) (http://baraksh.com)
 * @version 3.5.0
 * @date 2023-05-25
 * @copyright Copyright (c) 2023 Barak Shoshany. Licensed under the MIT license. If you found this project useful, please consider starring it on GitHub! If you use this library in software of any kind, please provide a link to the GitHub repository https://github.com/bshoshany/thread-pool in the source code and documentation. If you use this library in published research, please cite it as follows: Barak Shoshany, "A C++17 Thread Pool for High-Performance Scientific Computing", doi:10.5281/zenodo.4742687, arXiv:2105.00613 (May 2021)
 *
 * @brief BS::thread_pool: a fast, lightweight, and easy-to-use C++17 thread pool library. This program tests all aspects of the library, but is not needed in order to use the library.
 */

// Get rid of annoying MSVC warning.
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <algorithm>          // std::min, std::min_element, std::sort, std::unique
#include <atomic>             // std::atomic
#include <chrono>             // std::chrono
#include <cmath>              // std::abs, std::cos, std::exp, std::llround, std::log, std::round, std::sin, std::sqrt
#include <condition_variable> // std::condition_variable
#include <cstdlib>            // std::quick_exit
#include <ctime>              // std::localtime, std::strftime, std::time, std::time_t
#include <exception>          // std::exception
#include <fstream>            // std::ofstream
#include <functional>         // std::cref, std::ref
#include <future>             // std::future
#include <iomanip>            // std::setprecision, std::setw
#include <ios>                // std::fixed
#include <iostream>           // std::cout
#include <memory>             // std::make_unique, std::unique_ptr
#include <mutex>              // std::mutex, std::scoped_lock, std::unique_lock
#include <random>             // std::mt19937_64, std::random_device, std::uniform_int_distribution
#include <stdexcept>          // std::runtime_error
#include <string>             // std::string, std::to_string
#include <thread>             // std::this_thread, std::thread
#include <type_traits>        // std::is_same_v
#include <utility>            // std::forward, std::pair
#include <vector>             // std::begin, std::end, std::vector

// Include the header files for the thread pool library.
#include "../include/BS_thread_pool.hpp"
#include "../include/BS_thread_pool_light.hpp"

// ================
// Global variables
// ================

// Whether to output to a log file in addition to the standard output.
constexpr bool output_log = true;

// Whether to perform the tests.
constexpr bool enable_tests = true;

// Whether to perform the benchmarks.
constexpr bool enable_benchmarks = true;

// Whether to perform the long deadlock tests. Defaults to false since they can take much longer than the other tests.
constexpr bool enable_long_deadlock_tests = false;

// A global synced_stream object which prints to std::cout.
BS::synced_stream sync_cout(std::cout);

// A global stream object used to access the log file.
std::ofstream log_file;

// A global synced_stream object which prints to the log file.
BS::synced_stream sync_file(log_file);

// Global thread pool objects to be used throughout the test.
BS::thread_pool pool_full;
BS::thread_pool_light pool_light;

// A global random_device object to be used to seed some random number generators.
std::random_device rd;

// A global variable to measure how many checks succeeded.
size_t tests_succeeded = 0;

// A global variable to measure how many checks failed.
size_t tests_failed = 0;

// ================
// Helper functions
// ================

/**
 * @brief Print any number of items into both std::cout and the log file, syncing both independently.
 *
 * @tparam T The types of the items.
 * @param items The items to print.
 */
template <typename... T>
void dual_print(T&&... items)
{
    sync_cout.print(std::forward<T>(items)...);
    if (output_log)
        sync_file.print(std::forward<T>(items)...);
}

/**
 * @brief Print any number of items into both std::cout and the log file, syncing both independently. Also prints a newline character, and flushes the stream.
 *
 * @tparam T The types of the items.
 * @param items The items to print.
 */
template <typename... T>
void dual_println(T&&... items)
{
    dual_print(std::forward<T>(items)..., BS::synced_stream::endl);
}

/**
 * @brief Print a stylized header.
 *
 * @param text The text of the header. Will appear between two lines.
 * @param symbol The symbol to use for the lines. Default is '='.
 */
void print_header(const std::string& text, const char symbol = '=')
{
    dual_println();
    dual_println(std::string(text.length(), symbol));
    dual_println(text);
    dual_println(std::string(text.length(), symbol));
}

/**
 * @brief Get a string representing the current time.
 *
 * @return The string.
 */
std::string get_time()
{
    const std::time_t t = std::time(nullptr);
    char time_string[32];
    std::strftime(time_string, sizeof(time_string), "%Y-%m-%d_%H.%M.%S", std::localtime(&t));
    return time_string;
}

/**
 * @brief Check if a condition is met, report the result, and keep count of the total number of successes and failures.
 *
 * @param condition The condition to check.
 */
void check(const bool condition)
{
    if (condition)
    {
        dual_println("-> PASSED!");
        ++tests_succeeded;
    }
    else
    {
        dual_println("-> FAILED!");
        ++tests_failed;
    }
}

/**
 * @brief Check if the expected result has been obtained, report the result, and keep count of the total number of successes and failures.
 *
 * @param condition The condition to check.
 */
template <typename T1, typename T2>
void check(const T1 expected, const T2 obtained)
{
    dual_print("Expected: ", expected, ", obtained: ", obtained);
    if (expected == obtained)
    {
        dual_println(" -> PASSED!");
        ++tests_succeeded;
    }
    else
    {
        dual_println(" -> FAILED!");
        ++tests_failed;
    }
}

// =========================================
// Functions to verify the number of threads
// =========================================

/**
 * @brief Count the number of unique threads in the pool. Submits a number of tasks equal to twice the thread count into the pool. Each task stores the ID of the thread running it, and then waits until released by the main thread. This ensures that each thread in the pool runs at least one task. The number of unique thread IDs is then counted from the stored IDs.
 *
 * @tparam P The type of the thread pool to check.
 * @param pool The thread pool to check.
 */
template <typename P>
BS::concurrency_t count_unique_threads(P& pool)
{
    std::condition_variable ID_cv, total_cv;
    std::mutex ID_mutex, total_mutex;
    {
        const BS::concurrency_t num_tasks = pool.get_thread_count() * 2;
        std::vector<std::thread::id> thread_IDs(num_tasks);
        std::unique_lock<std::mutex> total_lock(total_mutex);
        BS::concurrency_t total_count = 0;
        bool ID_release = false;
        pool.wait_for_tasks();
        for (std::thread::id& id : thread_IDs)
            pool.push_task(
                [&total_count, &id, &ID_release, &ID_cv, &total_cv, &ID_mutex, &total_mutex]
                {
                    id = std::this_thread::get_id();
                    {
                        const std::scoped_lock total_lock_local(total_mutex);
                        ++total_count;
                    }
                    total_cv.notify_one();
                    std::unique_lock<std::mutex> ID_lock_local(ID_mutex);
                    ID_cv.wait(ID_lock_local, [&ID_release] { return ID_release; });
                });
        total_cv.wait(total_lock, [&total_count, &pool] { return total_count == pool.get_thread_count(); });
        {
            const std::scoped_lock ID_lock(ID_mutex);
            ID_release = true;
        }
        ID_cv.notify_all();
        total_cv.wait(total_lock, [&total_count, &num_tasks] { return total_count == num_tasks; });
        pool.wait_for_tasks();
        std::sort(thread_IDs.begin(), thread_IDs.end());
        return static_cast<BS::concurrency_t>(std::unique(thread_IDs.begin(), thread_IDs.end()) - thread_IDs.begin());
    }
}

/**
 * @brief Check that the constructor works.
 *
 * @tparam P The type of the thread pool to check.
 * @param pool The thread pool to check.
 */
template <typename P>
void check_constructor(P& pool)
{
    dual_println("Checking that the thread pool reports a number of threads equal to the hardware concurrency...");
    check(std::thread::hardware_concurrency(), pool.get_thread_count());
    dual_println("Checking that the manually counted number of unique thread IDs is equal to the reported number of threads...");
    check(pool.get_thread_count(), count_unique_threads(pool));
}

/**
 * @brief Check that reset() works.
 *
 * @param pool The thread pool to check.
 */
void check_reset(BS::thread_pool& pool)
{
    pool.reset(std::thread::hardware_concurrency() / 2);
    dual_println("Checking that after reset() the thread pool reports a number of threads equal to half the hardware concurrency...");
    check(std::thread::hardware_concurrency() / 2, pool.get_thread_count());
    dual_println("Checking that after reset() the manually counted number of unique thread IDs is equal to the reported number of threads...");
    check(pool.get_thread_count(), count_unique_threads(pool));
    pool.reset(std::thread::hardware_concurrency());
    dual_println("Checking that after a second reset() the thread pool reports a number of threads equal to the hardware concurrency...");
    check(std::thread::hardware_concurrency(), pool.get_thread_count());
    dual_println("Checking that after a second reset() the manually counted number of unique thread IDs is equal to the reported number of threads...");
    check(pool.get_thread_count(), count_unique_threads(pool));
}

// ================================
// Functions to check for deadlocks
// ================================

// An auxiliary thread pool used by check_deadlock().
BS::thread_pool check_deadlock_pool;

/**
 * @brief Check that the specified function does not create deadlocks. The function will be run many times to increase the probability of encountering a deadlock as a result of subtle timing issues. Uses an auxiliary pool so the whole test doesn't get stuck if a deadlock is encountered.
 *
 * @tparam F The type of the function.
 * @param task The function to try.
 */
template <typename F>
void check_deadlock(const F&& task)
{
    constexpr uint32_t tries = 10000;
    uint32_t i = 0;
    check_deadlock_pool.push_task(
        [&i, &task]
        {
            do
                task();
            while (++i < tries);
        });
    bool passed = false;
    while (true)
    {
        uint32_t old_i = i;
        check_deadlock_pool.wait_for_tasks_duration(std::chrono::milliseconds(500));
        if (i == tries)
        {
            dual_println("Successfully finished all tries!");
            passed = true;
            break;
        }
        else if (i > old_i)
        {
            dual_println("Finished ", i, " tries out of ", tries, "...");
        }
        else
        {
            dual_println("Error: deadlock detected!");
            passed = false;
            break;
        }
    }
    check(passed);
}

// =======================================
// Functions to verify submission of tasks
// =======================================

/**
 * @brief A class to detect when a copy or move constructor has been invoked.
 */
class detect_copy_move
{
public:
    detect_copy_move() = default;
    detect_copy_move(const detect_copy_move&)
    {
        copied = true;
    }
    detect_copy_move(detect_copy_move&&)
    {
        moved = true;
    }
    bool copied = false;
    bool moved = false;
};

/**
 * @brief Check that push_task() works.
 *
 * @tparam P The type of the thread pool to check.
 * @param pool The thread pool to check.
 */
template <typename P>
void check_push_task(P& pool)
{
    dual_println("Checking that push_task() works for a function with no arguments or return value...");
    {
        bool flag = false;
        pool.push_task([&flag] { flag = true; });
        pool.wait_for_tasks();
        check(flag);
    }
    dual_println("Checking that push_task() works for a function with one argument and no return value...");
    {
        bool flag = false;
        pool.push_task([](bool* flag_) { *flag_ = true; }, &flag);
        pool.wait_for_tasks();
        check(flag);
    }
    dual_println("Checking that push_task() works for a function with two arguments and no return value...");
    {
        bool flag1 = false;
        bool flag2 = false;
        pool.push_task([](bool* flag1_, bool* flag2_) { *flag1_ = *flag2_ = true; }, &flag1, &flag2);
        pool.wait_for_tasks();
        check(flag1 && flag2);
    }
    dual_println("Checking that push_task() does not create unnecessary copies of the function object...");
    {
        bool copied = false;
        bool moved = false;
        pool.push_task(
            [detect = detect_copy_move(), &copied, &moved]
            {
                copied = detect.copied;
                moved = detect.moved;
            });
        pool.wait_for_tasks();
        check(!copied && moved);
    }
    dual_println("Checking that push_task() correctly accepts arguments passed by value, reference, and constant reference...");
    {
        int64_t pass_me_by_value = 0;
        pool.push_task(
            [](int64_t passed_by_value)
            {
                if (++passed_by_value)
                    std::this_thread::yield();
            },
            pass_me_by_value);
        pool.wait_for_tasks();
        check(pass_me_by_value == 0);
        int64_t pass_me_by_ref = 0;
        pool.push_task([](int64_t& passed_by_ref) { ++passed_by_ref; }, std::ref(pass_me_by_ref));
        pool.wait_for_tasks();
        check(pass_me_by_ref == 1);
        int64_t pass_me_by_cref = 0;
        std::atomic<bool> release = false;
        pool.push_task(
            [&release](const int64_t& passed_by_cref)
            {
                while (!release)
                    std::this_thread::yield();
                check(passed_by_cref == 1);
            },
            std::cref(pass_me_by_cref));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ++pass_me_by_cref;
        release = true;
        pool.wait_for_tasks();
    }
}

/**
 * @brief Check that submit() works.
 *
 * @tparam P The type of the thread pool to check.
 * @param pool The thread pool to check.
 */
template <typename P>
void check_submit(P& pool)
{
    dual_println("Checking that submit() works for a function with no arguments or return value...");
    {
        bool flag = false;
        pool.submit([&flag] { flag = true; }).wait();
        check(flag);
    }
    dual_println("Checking that submit() works for a function with one argument and no return value...");
    {
        bool flag = false;
        pool.submit([](bool* flag_) { *flag_ = true; }, &flag).wait();
        check(flag);
    }
    dual_println("Checking that submit() works for a function with two arguments and no return value...");
    {
        bool flag1 = false;
        bool flag2 = false;
        pool.submit([](bool* flag1_, bool* flag2_) { *flag1_ = *flag2_ = true; }, &flag1, &flag2).wait();
        check(flag1 && flag2);
    }
    dual_println("Checking that submit() works for a function with no arguments and a return value...");
    {
        bool flag = false;
        std::future<int> flag_future = pool.submit(
            [&flag]
            {
                flag = true;
                return 42;
            });
        check(flag_future.get() == 42 && flag);
    }
    dual_println("Checking that submit() works for a function with one argument and a return value...");
    {
        bool flag = false;
        std::future<int> flag_future = pool.submit(
            [](bool* flag_)
            {
                *flag_ = true;
                return 42;
            },
            &flag);
        check(flag_future.get() == 42 && flag);
    }
    dual_println("Checking that submit() works for a function with two arguments and a return value...");
    {
        bool flag1 = false;
        bool flag2 = false;
        std::future<int> flag_future = pool.submit(
            [](bool* flag1_, bool* flag2_)
            {
                *flag1_ = *flag2_ = true;
                return 42;
            },
            &flag1, &flag2);
        check(flag_future.get() == 42 && flag1 && flag2);
    }
    dual_println("Checking that submit() does not create unnecessary copies of the function object...");
    {
        std::future<std::pair<bool, bool>> fut = pool.submit([detect = detect_copy_move()] { return std::pair(detect.copied, detect.moved); });
        std::pair<bool, bool> result = fut.get();
        check(!result.first && result.second);
    }
    dual_println("Checking that submit() correctly accepts arguments passed by value, reference, and constant reference...");
    {
        int64_t pass_me_by_value = 0;
        pool.submit(
                [](int64_t passed_by_value)
                {
                    if (++passed_by_value)
                        std::this_thread::yield();
                },
                pass_me_by_value)
            .wait();
        check(pass_me_by_value == 0);
        int64_t pass_me_by_ref = 0;
        pool.submit([](int64_t& passed_by_ref) { ++passed_by_ref; }, std::ref(pass_me_by_ref)).wait();
        check(pass_me_by_ref == 1);
        int64_t pass_me_by_cref = 0;
        std::atomic<bool> release = false;
        std::future<void> fut = pool.submit(
            [&release](const int64_t& passed_by_cref)
            {
                while (!release)
                    std::this_thread::yield();
                check(passed_by_cref == 1);
            },
            std::cref(pass_me_by_cref));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ++pass_me_by_cref;
        release = true;
        fut.wait();
    }
}

/**
 * @brief A class to facilitate checking that member functions of different types have been successfully submitted.
 *
 * @tparam P The type of the thread pool to check.
 */
template <typename P>
class flag_class
{
public:
    flag_class(P& pool_) : pool(pool_) {}

    void set_flag_no_args()
    {
        flag = true;
    }

    void set_flag_one_arg(const bool arg)
    {
        flag = arg;
    }

    int set_flag_no_args_return()
    {
        flag = true;
        return 42;
    }

    int set_flag_one_arg_return(const bool arg)
    {
        flag = arg;
        return 42;
    }

    [[nodiscard]] bool get_flag() const
    {
        return flag;
    }

    void push_test_flag_no_args()
    {
        pool.push_task(&flag_class<P>::set_flag_no_args, this);
        pool.wait_for_tasks();
        check(get_flag());
    }

    void push_test_flag_one_arg()
    {
        pool.push_task(&flag_class<P>::set_flag_one_arg, this, true);
        pool.wait_for_tasks();
        check(get_flag());
    }

    void submit_test_flag_no_args()
    {
        pool.submit(&flag_class<P>::set_flag_no_args, this).wait();
        check(get_flag());
    }

    void submit_test_flag_one_arg()
    {
        pool.submit(&flag_class<P>::set_flag_one_arg, this, true).wait();
        check(get_flag());
    }

    void submit_test_flag_no_args_return()
    {
        std::future<int> flag_future = pool.submit(&flag_class<P>::set_flag_no_args_return, this);
        check(flag_future.get() == 42 && get_flag());
    }

    void submit_test_flag_one_arg_return()
    {
        std::future<int> flag_future = pool.submit(&flag_class<P>::set_flag_one_arg_return, this, true);
        check(flag_future.get() == 42 && get_flag());
    }

private:
    bool flag = false;
    P& pool;
};

/**
 * @brief Check that submitting member functions works.
 *
 * @tparam P The type of the thread pool to check.
 * @param pool The thread pool to check.
 */
template <typename P>
void check_member_function(P& pool)
{
    dual_println("Checking that push_task() works for a member function with no arguments or return value...");
    {
        flag_class<P> flag(pool);
        pool.push_task(&flag_class<P>::set_flag_no_args, &flag);
        pool.wait_for_tasks();
        check(flag.get_flag());
    }
    dual_println("Checking that push_task() works for a member function with one argument and no return value...");
    {
        flag_class<P> flag(pool);
        pool.push_task(&flag_class<P>::set_flag_one_arg, &flag, true);
        pool.wait_for_tasks();
        check(flag.get_flag());
    }
    dual_println("Checking that submit() works for a member function with no arguments or return value...");
    {
        flag_class<P> flag(pool);
        pool.submit(&flag_class<P>::set_flag_no_args, &flag).wait();
        check(flag.get_flag());
    }
    dual_println("Checking that submit() works for a member function with one argument and no return value...");
    {
        flag_class<P> flag(pool);
        pool.submit(&flag_class<P>::set_flag_one_arg, &flag, true).wait();
        check(flag.get_flag());
    }
    dual_println("Checking that submit() works for a member function with no arguments and a return value...");
    {
        flag_class<P> flag(pool);
        std::future<int> flag_future = pool.submit(&flag_class<P>::set_flag_no_args_return, &flag);
        check(flag_future.get() == 42 && flag.get_flag());
    }
    dual_println("Checking that submit() works for a member function with one argument and a return value...");
    {
        flag_class<P> flag(pool);
        std::future<int> flag_future = pool.submit(&flag_class<P>::set_flag_one_arg_return, &flag, true);
        check(flag_future.get() == 42 && flag.get_flag());
    }
}

/**
 * @brief Check that submitting member functions within an object works.
 *
 * @tparam P The type of the thread pool to check.
 * @param pool The thread pool to check.
 */
template <typename P>
void check_member_function_within_object(P& pool)
{
    dual_println("Checking that push_task() works within an object for a member function with no arguments or return value...");
    {
        flag_class<P> flag(pool);
        flag.push_test_flag_no_args();
    }
    dual_println("Checking that push_task() works within an object for a member function with one argument and no return value...");
    {
        flag_class<P> flag(pool);
        flag.push_test_flag_one_arg();
    }
    dual_println("Checking that submit() works within an object for a member function with no arguments or return value...");
    {
        flag_class<P> flag(pool);
        flag.submit_test_flag_no_args();
    }
    dual_println("Checking that submit() works within an object for a member function with one argument and no return value...");
    {
        flag_class<P> flag(pool);
        flag.submit_test_flag_one_arg();
    }
    dual_println("Checking that submit() works within an object for a member function with no arguments and a return value...");
    {
        flag_class<P> flag(pool);
        flag.submit_test_flag_no_args_return();
    }
    dual_println("Checking that submit() works within an object for a member function with one argument and a return value...");
    {
        flag_class<P> flag(pool);
        flag.submit_test_flag_one_arg_return();
    }
}

/**
 * @brief Check that wait_for_tasks() works.
 *
 * @tparam P The type of the thread pool to check.
 * @param pool The thread pool to check.
 */
template <typename P>
void check_wait_for_tasks(P& pool)
{
    const BS::concurrency_t n = pool.get_thread_count() * 10;
    std::unique_ptr<std::atomic<bool>[]> flags = std::make_unique<std::atomic<bool>[]>(n);
    for (BS::concurrency_t i = 0; i < n; ++i)
        pool.push_task(
            [&flags, i]
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                flags[i] = true;
            });
    dual_println("Waiting for tasks...");
    pool.wait_for_tasks();
    bool all_flags = true;
    for (BS::concurrency_t i = 0; i < n; ++i)
        all_flags = all_flags && flags[i];
    check(all_flags);
}

/**
 * @brief Check that wait_for_tasks() correctly blocks all external threads that call it.
 *
 * @tparam P The type of the thread pool to check.
 * @param pool The thread pool to check.
 */
template <typename P>
void check_wait_for_tasks_blocks(P& pool)
{
    pool.wait_for_tasks();
    dual_println("Checking that wait_for_tasks() correctly blocks all external threads that call it...");
    std::atomic<bool> release = false;
    pool.push_task(
        [&release]
        {
            dual_println("Task submitted to pool 1 and waiting to be released...");
            while (!release)
                std::this_thread::yield();
            dual_println("Task released.");
        });
    constexpr BS::concurrency_t num_waiting_tasks = 4;
    P temp_pool(num_waiting_tasks);
    std::unique_ptr<std::atomic<bool>[]> flags = std::make_unique<std::atomic<bool>[]>(num_waiting_tasks);
    auto waiting_task = [&flags, &pool](const BS::concurrency_t i)
    {
        dual_println("Task ", i, " submitted to pool 2 and waiting for pool 1's task to finish...");
        pool.wait_for_tasks();
        dual_println("Task ", i, " finished waiting.");
        flags[i] = true;
    };
    for (BS::concurrency_t i = 0; i < num_waiting_tasks; ++i)
        temp_pool.push_task(waiting_task, i);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    bool any_flag_set = false;
    for (BS::concurrency_t i = 0; i < num_waiting_tasks; ++i)
        any_flag_set = any_flag_set || flags[i];
    check(!any_flag_set);
    release = true;
    temp_pool.wait_for_tasks();
    bool all_flags_set = true;
    for (size_t i = 0; i < num_waiting_tasks; ++i)
        all_flags_set = all_flags_set && flags[i];
    check(all_flags_set);
}

/**
 * @brief Check that calling wait_for_tasks() more than once doesn't create a deadlock. Uses a mechanism similar to check_deadlock().
 *
 * @tparam P The type of the thread pool to check.
 */
template <typename P>
void check_wait_for_tasks_deadlock()
{
    dual_println("Checking for deadlocks when waiting for tasks...");
    P temp_pool(1);
    temp_pool.push_task([] { std::this_thread::sleep_for(std::chrono::milliseconds(500)); });
    constexpr uint32_t n_waiting_tasks = 1000;
    std::atomic<uint32_t> count = 0;
    for (uint32_t j = 0; j < n_waiting_tasks; ++j)
    {
        check_deadlock_pool.push_task(
            [&temp_pool, &count]
            {
                temp_pool.wait_for_tasks();
                ++count;
            });
    }
    bool passed = false;
    while (true)
    {
        uint32_t old_count = count;
        check_deadlock_pool.wait_for_tasks_duration(std::chrono::milliseconds(1000));
        if (count == n_waiting_tasks)
        {
            dual_println("All waiting tasks successfully finished!");
            passed = true;
            break;
        }
        else if (count > old_count)
        {
            dual_println(count, " tasks out of ", n_waiting_tasks, " finished waiting...");
        }
        else
        {
            dual_println("Error: deadlock detected!");
            passed = false;
            break;
        }
    }
    check(passed);
}

/**
 * @brief Check that wait_for_tasks_duration() works.
 *
 * @param pool The thread pool to check.
 */
void check_wait_for_tasks_duration(BS::thread_pool& pool)
{
    dual_println("Checking that wait_for_tasks_duration() works...");
    pool.reset();
    std::atomic<bool> done = false;
    pool.push_task(
        [&done]
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            done = true;
        });
    dual_println("Task submitted. Waiting for 10ms...");
    pool.wait_for_tasks_duration(std::chrono::milliseconds(10));
    check(!done);
    dual_println("Waiting for 500ms...");
    pool.wait_for_tasks_duration(std::chrono::milliseconds(500));
    check(done);
}

/**
 * @brief Check that wait_for_tasks_until() works.
 *
 * @param pool The thread pool to check.
 */
void check_wait_for_tasks_until(BS::thread_pool& pool)
{
    dual_println("Checking that wait_for_tasks_until() works...");
    pool.reset();
    std::atomic<bool> done = false;
    pool.push_task(
        [&done]
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            done = true;
        });
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    dual_println("Task submitted. Waiting until 10ms from submission time...");
    pool.wait_for_tasks_until(now + std::chrono::milliseconds(10));
    check(!done);
    dual_println("Waiting until 500ms from submission time...");
    pool.wait_for_tasks_until(now + std::chrono::milliseconds(500));
    check(done);
}

// ========================================
// Functions to verify loop parallelization
// ========================================

/**
 * @brief Check that push_loop() or parallelize_loop() work for a specific range of indices split over a specific number of tasks, with no return value.
 *
 * @tparam P The type of the thread pool to check.
 * @tparam T The type of the last index in the loop plus 1.
 * @param pool The thread pool to check.
 * @param random_start The first index in the loop.
 * @param random_end The last index in the loop plus 1.
 * @param num_tasks The number of tasks.
 * @param use_push Whether to check push_loop() instead of parallelize_loop().
 */
template <typename P, typename T>
void check_parallelize_loop_no_return(P& pool, const int64_t random_start, T random_end, const BS::concurrency_t num_tasks, const bool use_push = false)
{
    if (random_start == random_end)
        ++random_end;
    dual_println("Verifying that ", use_push ? "push_loop()" : "parallelize_loop()", " from ", random_start, " to ", random_end, " with ", num_tasks, num_tasks == 1 ? " task" : " tasks", " modifies all indices...");
    const size_t num_indices = static_cast<size_t>(std::abs(random_end - random_start));
    const int64_t offset = std::min<int64_t>(random_start, static_cast<int64_t>(random_end));
    std::unique_ptr<std::atomic<bool>[]> flags = std::make_unique<std::atomic<bool>[]>(num_indices);
    const auto loop = [&flags, offset](const int64_t start, const int64_t end)
    {
        for (int64_t i = start; i < end; ++i)
            flags[static_cast<size_t>(i - offset)] = true;
    };
    if (use_push)
    {
        if (random_start == 0)
            pool.push_loop(random_end, loop, num_tasks);
        else
            pool.push_loop(random_start, random_end, loop, num_tasks);
        pool.wait_for_tasks();
    }
    else if constexpr (std::is_same_v<P, BS::thread_pool>)
    {
        if (random_start == 0)
            pool.parallelize_loop(random_end, loop, num_tasks).wait();
        else
            pool.parallelize_loop(random_start, random_end, loop, num_tasks).wait();
    }
    bool all_flags = true;
    for (size_t i = 0; i < num_indices; ++i)
        all_flags = all_flags && flags[i];
    check(all_flags);
}

/**
 * @brief Check that parallelize_loop() works for a specific range of indices split over a specific number of tasks, with a return value.
 *
 * @param pool The thread pool to check.
 * @param random_start The first index in the loop.
 * @param random_end The last index in the loop plus 1.
 * @param num_tasks The number of tasks.
 */
void check_parallelize_loop_return(BS::thread_pool& pool, const int64_t random_start, int64_t random_end, const BS::concurrency_t num_tasks)
{
    if (random_start == random_end)
        ++random_end;
    dual_println("Verifying that parallelize_loop() from ", random_start, " to ", random_end, " with ", num_tasks, num_tasks == 1 ? " task" : " tasks", " correctly sums all indices...");
    const auto loop = [](const int64_t start, const int64_t end)
    {
        int64_t total = 0;
        for (int64_t i = start; i < end; ++i)
            total += i;
        return total;
    };
    const std::vector<int64_t> sums_vector = (random_start == 0) ? pool.parallelize_loop(random_end, loop, num_tasks).get() : pool.parallelize_loop(random_start, random_end, loop, num_tasks).get();
    int64_t sum = 0;
    for (const int64_t& s : sums_vector)
        sum += s;
    check(std::abs(random_start - random_end) * (random_start + random_end - 1), sum * 2);
}

/**
 * @brief Check that push_loop() and parallelize_loop() work using several different random values for the range of indices and number of tasks.
 *
 * @tparam P The type of the thread pool to check.
 * @param pool The thread pool to check.
 */
template <typename P>
void check_loop(P& pool)
{
    std::mt19937_64 mt(rd());
    std::uniform_int_distribution<int64_t> index_dist(-1000000, 1000000);
    std::uniform_int_distribution<BS::concurrency_t> task_dist(1, pool.get_thread_count());
    constexpr uint64_t n = 10;
    if constexpr (std::is_same_v<P, BS::thread_pool>)
    {
        for (uint64_t i = 0; i < n; ++i)
            check_parallelize_loop_no_return(pool, index_dist(mt), index_dist(mt), task_dist(mt));
        for (uint64_t i = 0; i < n; ++i)
            check_parallelize_loop_return(pool, index_dist(mt), index_dist(mt), task_dist(mt));
        dual_println("Verifying that parallelize_loop() with identical start and end indices does nothing...");
        bool flag = true;
        const int64_t index = index_dist(mt);
        pool.parallelize_loop(index, index, [&flag](const int64_t, const int64_t) { flag = false; }).wait();
        check(flag);
        dual_println("Trying parallelize_loop() with start and end indices of different types:");
        const int64_t start = index_dist(mt);
        const uint32_t end = static_cast<uint32_t>(std::abs(index_dist(mt)));
        check_parallelize_loop_no_return(pool, start, end, task_dist(mt));
        dual_println("Trying the overload of parallelize_loop() for the case where the first index is equal to 0:");
        check_parallelize_loop_no_return(pool, 0, index_dist(mt), task_dist(mt));
        check_parallelize_loop_return(pool, 0, index_dist(mt), task_dist(mt));
    }
    for (uint64_t i = 0; i < n; ++i)
        check_parallelize_loop_no_return(pool, index_dist(mt), index_dist(mt), task_dist(mt), true);
    dual_println("Verifying that push_loop() with identical start and end indices does nothing...");
    bool flag = true;
    const int64_t index = index_dist(mt);
    pool.push_loop(index, index, [&flag](const int64_t, const int64_t) { flag = false; });
    pool.wait_for_tasks();
    check(flag);
    dual_println("Trying push_loop() with start and end indices of different types:");
    const int64_t start = index_dist(mt);
    const uint32_t end = static_cast<uint32_t>(std::abs(index_dist(mt)));
    check_parallelize_loop_no_return(pool, start, end, task_dist(mt), true);
    dual_println("Trying the overload of push_loop() for the case where the first index is equal to 0:");
    check_parallelize_loop_no_return(pool, 0, index_dist(mt), task_dist(mt), true);
}

// ===============================================
// Functions to verify task monitoring and control
// ===============================================

/**
 * @brief Check that task monitoring works.
 *
 * @param pool The thread pool to check.
 */
void check_task_monitoring(BS::thread_pool& pool)
{
    BS::concurrency_t n = std::min<BS::concurrency_t>(std::thread::hardware_concurrency(), 4);
    dual_println("Resetting pool to ", n, " threads.");
    pool.reset(n);
    dual_println("Submitting ", n * 3, " tasks.");
    std::unique_ptr<std::atomic<bool>[]> release = std::make_unique<std::atomic<bool>[]>(n * 3);
    for (BS::concurrency_t i = 0; i < n * 3; ++i)
        pool.push_task(
            [&release, i]
            {
                while (!release[i])
                    std::this_thread::yield();
                dual_println("Task ", i, " released.");
            });
    constexpr std::chrono::milliseconds sleep_time(300);
    std::this_thread::sleep_for(sleep_time);

    dual_println("After submission, should have: ", n * 3, " tasks total, ", n, " tasks running, ", n * 2, " tasks queued...");
    dual_print("Result: ", pool.get_tasks_total(), " tasks total, ", pool.get_tasks_running(), " tasks running, ", pool.get_tasks_queued(), " tasks queued ");
    check(pool.get_tasks_total() == n * 3 && pool.get_tasks_running() == n && pool.get_tasks_queued() == n * 2);
    for (BS::concurrency_t i = 0; i < n; ++i)
        release[i] = true;
    std::this_thread::sleep_for(sleep_time);

    dual_println("After releasing ", n, " tasks, should have: ", n * 2, " tasks total, ", n, " tasks running, ", n, " tasks queued...");
    dual_print("Result: ", pool.get_tasks_total(), " tasks total, ", pool.get_tasks_running(), " tasks running, ", pool.get_tasks_queued(), " tasks queued ");
    check(pool.get_tasks_total() == n * 2 && pool.get_tasks_running() == n && pool.get_tasks_queued() == n);
    for (BS::concurrency_t i = n; i < n * 2; ++i)
        release[i] = true;
    std::this_thread::sleep_for(sleep_time);

    dual_println("After releasing ", n, " more tasks, should have: ", n, " tasks total, ", n, " tasks running, ", 0, " tasks queued...");
    dual_print("Result: ", pool.get_tasks_total(), " tasks total, ", pool.get_tasks_running(), " tasks running, ", pool.get_tasks_queued(), " tasks queued ");
    check(pool.get_tasks_total() == n && pool.get_tasks_running() == n && pool.get_tasks_queued() == 0);
    for (BS::concurrency_t i = n * 2; i < n * 3; ++i)
        release[i] = true;
    std::this_thread::sleep_for(sleep_time);

    dual_println("After releasing the final ", n, " tasks, should have: ", 0, " tasks total, ", 0, " tasks running, ", 0, " tasks queued...");
    dual_print("Result: ", pool.get_tasks_total(), " tasks total, ", pool.get_tasks_running(), " tasks running, ", pool.get_tasks_queued(), " tasks queued ");
    check(pool.get_tasks_total() == 0 && pool.get_tasks_running() == 0 && pool.get_tasks_queued() == 0);

    dual_println("Resetting pool to ", std::thread::hardware_concurrency(), " threads.");
    pool.reset(std::thread::hardware_concurrency());
}

/**
 * @brief Check that pausing works.
 *
 * @param pool The thread pool to check.
 */
void check_pausing(BS::thread_pool& pool)
{
    BS::concurrency_t n = std::min<BS::concurrency_t>(std::thread::hardware_concurrency(), 4);
    dual_println("Resetting pool to ", n, " threads.");
    pool.reset(n);
    dual_println("Checking that the pool correctly reports that it is not paused.");
    check(pool.is_paused() == false);
    dual_println("Pausing pool.");
    pool.pause();
    dual_println("Checking that the pool correctly reports that it is paused.");
    check(pool.is_paused() == true);
    dual_println("Submitting ", n * 3, " tasks, each one waiting for 200ms.");
    for (BS::concurrency_t i = 0; i < n * 3; ++i)
        pool.push_task(
            [i]
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                dual_println("Task ", i, " done.");
            });

    dual_println("Immediately after submission, should have: ", n * 3, " tasks total, ", 0, " tasks running, ", n * 3, " tasks queued...");
    dual_print("Result: ", pool.get_tasks_total(), " tasks total, ", pool.get_tasks_running(), " tasks running, ", pool.get_tasks_queued(), " tasks queued ");
    check(pool.get_tasks_total() == n * 3 && pool.get_tasks_running() == 0 && pool.get_tasks_queued() == n * 3);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    dual_println("300ms later, should still have: ", n * 3, " tasks total, ", 0, " tasks running, ", n * 3, " tasks queued...");
    dual_print("Result: ", pool.get_tasks_total(), " tasks total, ", pool.get_tasks_running(), " tasks running, ", pool.get_tasks_queued(), " tasks queued ");
    check(pool.get_tasks_total() == n * 3 && pool.get_tasks_running() == 0 && pool.get_tasks_queued() == n * 3);
    dual_println("Unpausing pool.");
    pool.unpause();
    dual_println("Checking that the pool correctly reports that it is not paused.");
    check(pool.is_paused() == false);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    dual_println("300ms later, should have: ", n * 2, " tasks total, ", n, " tasks running, ", n, " tasks queued...");
    dual_print("Result: ", pool.get_tasks_total(), " tasks total, ", pool.get_tasks_running(), " tasks running, ", pool.get_tasks_queued(), " tasks queued ");
    check(pool.get_tasks_total() == n * 2 && pool.get_tasks_running() == n && pool.get_tasks_queued() == n);
    dual_println("Pausing pool and using wait_for_tasks() to wait for the running tasks.");
    pool.pause();
    pool.wait_for_tasks();

    dual_println("After waiting, should have: ", n, " tasks total, ", 0, " tasks running, ", n, " tasks queued...");
    dual_print("Result: ", pool.get_tasks_total(), " tasks total, ", pool.get_tasks_running(), " tasks running, ", pool.get_tasks_queued(), " tasks queued ");
    check(pool.get_tasks_total() == n && pool.get_tasks_running() == 0 && pool.get_tasks_queued() == n);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    dual_println("200ms later, should still have: ", n, " tasks total, ", 0, " tasks running, ", n, " tasks queued...");
    dual_print("Result: ", pool.get_tasks_total(), " tasks total, ", pool.get_tasks_running(), " tasks running, ", pool.get_tasks_queued(), " tasks queued ");
    check(pool.get_tasks_total() == n && pool.get_tasks_running() == 0 && pool.get_tasks_queued() == n);
    dual_println("Unpausing pool and using wait_for_tasks() to wait for all tasks.");
    pool.unpause();
    pool.wait_for_tasks();

    dual_println("After waiting, should have: ", 0, " tasks total, ", 0, " tasks running, ", 0, " tasks queued...");
    dual_print("Result: ", pool.get_tasks_total(), " tasks total, ", pool.get_tasks_running(), " tasks running, ", pool.get_tasks_queued(), " tasks queued ");
    check(pool.get_tasks_total() == 0 && pool.get_tasks_running() == 0 && pool.get_tasks_queued() == 0);

    dual_println("Resetting pool to ", std::thread::hardware_concurrency(), " threads.");
    pool.reset(std::thread::hardware_concurrency());
}

/**
 * @brief Check that purge() works.
 *
 * @param pool The thread pool to check.
 */
void check_purge(BS::thread_pool& pool)
{
    dual_println("Resetting pool to 1 thread.");
    pool.reset(1);
    constexpr size_t num_tasks = 10;
    dual_println("Submitting ", num_tasks, " tasks to the pool.");
    std::unique_ptr<std::atomic<bool>[]> done = std::make_unique<std::atomic<bool>[]>(num_tasks);
    for (size_t i = 0; i < num_tasks; ++i)
        pool.push_task(
            [&done, i]
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                dual_println("Task ", i, " done.");
                done[i] = true;
            });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    dual_println("Purging the pool and waiting for tasks...");
    pool.purge();
    pool.wait_for_tasks();
    dual_println("Checking that only the first task was executed...");
    check(done[0]);
    bool any_flag_set = false;
    for (BS::concurrency_t i = 1; i < num_tasks; ++i)
        any_flag_set = any_flag_set || done[i];
    check(!any_flag_set);

    dual_println("Resetting pool to ", std::thread::hardware_concurrency(), " threads.");
    pool.reset(std::thread::hardware_concurrency());
}

// ======================================
// Functions to verify exception handling
// ======================================

/**
 * @brief Check that exceptions are forwarded correctly by submit().
 *
 * @tparam P The type of the thread pool to check.
 * @param pool The thread pool to check.
 */
template <typename P>
void check_exceptions_submit(P& pool)
{
    dual_println("Checking that exceptions are forwarded correctly by submit()...");
    bool caught = false;
    auto throws = []
    {
        dual_println("Throwing exception...");
        throw std::runtime_error("Exception thrown!");
    };
    std::future<void> my_future = pool.submit(throws);
    try
    {
        my_future.get();
    }
    catch (const std::exception& e)
    {
        if (e.what() == std::string("Exception thrown!"))
            caught = true;
    }
    check(caught);
}

/**
 * @brief Check that exceptions are forwarded correctly by BS::multi_future.
 *
 * @param pool The thread pool to check.
 */
void check_exceptions_multi_future(BS::thread_pool& pool)
{
    dual_println("Checking that exceptions are forwarded correctly by BS::multi_future...");
    bool caught = false;
    auto throws = []
    {
        dual_println("Throwing exception...");
        throw std::runtime_error("Exception thrown!");
    };
    BS::multi_future<void> my_future;
    my_future.push_back(pool.submit(throws));
    my_future.push_back(pool.submit(throws));
    try
    {
        void(my_future.get());
    }
    catch (const std::exception& e)
    {
        if (e.what() == std::string("Exception thrown!"))
            caught = true;
    }
    check(caught);
}

// =====================================
// Functions to verify vector operations
// =====================================

/**
 * @brief Check that parallelized vector operations work as expected by calculating the sum of two randomized vectors of a specific size in two ways, single-threaded and multithreaded, and comparing the results.
 *
 * @tparam P The type of the thread pool to check.
 * @param pool The thread pool to check.
 * @param vector_size The size of the vectors.
 * @param num_tasks The number of tasks to split the calculation into.
 */
template <typename P>
void check_vector_of_size(P& pool, const size_t vector_size, const BS::concurrency_t num_tasks)
{
    std::vector<int64_t> vector_1(vector_size);
    std::vector<int64_t> vector_2(vector_size);
    std::mt19937_64 mt(rd());
    std::uniform_int_distribution<int64_t> vector_dist(-1000000, 1000000);
    for (size_t i = 0; i < vector_size; ++i)
    {
        vector_1[i] = vector_dist(mt);
        vector_2[i] = vector_dist(mt);
    }
    dual_println("Adding two vectors with ", vector_size, " elements using ", num_tasks, " tasks...");
    std::vector<int64_t> sum_single(vector_size);
    for (size_t i = 0; i < vector_size; ++i)
        sum_single[i] = vector_1[i] + vector_2[i];
    std::vector<int64_t> sum_multi(vector_size);
    pool.push_loop(
        0, vector_size,
        [&sum_multi, &vector_1, &vector_2](const size_t start, const size_t end)
        {
            for (size_t i = start; i < end; ++i)
                sum_multi[i] = vector_1[i] + vector_2[i];
        },
        num_tasks);
    pool.wait_for_tasks();
    bool vectors_equal = true;
    for (size_t i = 0; i < vector_size; ++i)
        vectors_equal = vectors_equal && (sum_single[i] == sum_multi[i]);
    check(vectors_equal);
}

/**
 * @brief Check that parallelized vector operations work as expected by calculating the sum of two randomized vectors in two ways, single-threaded and multithreaded, and comparing the results.
 *
 * @tparam P The type of the thread pool to check.
 * @param pool The thread pool to check.
 */
template <typename P>
void check_vectors(P& pool)
{
    std::mt19937_64 mt(rd());
    std::uniform_int_distribution<size_t> size_dist(0, 1000000);
    std::uniform_int_distribution<BS::concurrency_t> task_dist(1, pool.get_thread_count());
    for (size_t i = 0; i < 10; ++i)
        check_vector_of_size(pool, size_dist(mt), task_dist(mt));
}

// ==================
// Main test function
// ==================

/**
 * @brief Test that various aspects of the library are working as expected.
 */
void do_tests()
{
    print_header("Checking that the constructor works in the full thread pool:");
    check_constructor(pool_full);
    print_header("Checking that the constructor works in the light thread pool:");
    check_constructor(pool_light);

    print_header("Checking that reset() works in the full thread pool:");
    check_reset(pool_full);

    print_header("Checking that push_task() works in the full thread pool:");
    check_push_task(pool_full);
    print_header("Checking that push_task() works in the light thread pool:");
    check_push_task(pool_light);

    print_header("Checking that submit() works in the full thread pool:");
    check_submit(pool_full);
    print_header("Checking that submit() works in the light thread pool:");
    check_submit(pool_light);

    print_header("Checking that submitting member functions works in the full thread pool:");
    check_member_function(pool_full);
    print_header("Checking that submitting member functions works in the light thread pool:");
    check_member_function(pool_light);

    print_header("Checking that submitting member functions from within an object works in the full thread pool:");
    check_member_function_within_object(pool_full);
    print_header("Checking that submitting member functions from within an object works in the light thread pool:");
    check_member_function_within_object(pool_light);

    print_header("Checking that wait_for_tasks() works in the full thread pool...");
    check_wait_for_tasks(pool_full);
    check_wait_for_tasks_blocks(pool_full);
    check_wait_for_tasks_deadlock<BS::thread_pool>();
    check_wait_for_tasks_duration(pool_full);
    check_wait_for_tasks_until(pool_full);
    print_header("Checking that wait_for_tasks() works in the light thread pool...");
    check_wait_for_tasks(pool_light);
    check_wait_for_tasks_blocks(pool_full);
    check_wait_for_tasks_deadlock<BS::thread_pool_light>();

    print_header("Checking that push_loop() and parallelize_loop() work in the full thread pool:");
    check_loop(pool_full);
    print_header("Checking that push_loop() works in the light thread pool:");
    check_loop(pool_light);

    print_header("Checking that task monitoring works in the full thread pool:");
    check_task_monitoring(pool_full);

    print_header("Checking that pausing works in the full thread pool:");
    check_pausing(pool_full);

    print_header("Checking that purge() works in the full thread pool:");
    check_purge(pool_full);

    print_header("Checking that exception handling works in the full thread pool:");
    check_exceptions_submit(pool_full);
    check_exceptions_multi_future(pool_full);
    print_header("Checking that exception handling works in the light thread pool:");
    check_exceptions_submit(pool_light);

    print_header("Testing that vector operations produce the expected results in the full thread pool:");
    check_vectors(pool_full);
    print_header("Testing that vector operations produce the expected results in the light thread pool:");
    check_vectors(pool_light);

    if (enable_long_deadlock_tests)
    {
        print_header("Checking for deadlocks:");
        dual_println("Checking for destruction deadlocks in the full thread pool...");
        check_deadlock([] { BS::thread_pool temp_pool; });
        dual_println("Checking for destruction deadlocks in the light thread pool...");
        check_deadlock([] { BS::thread_pool_light temp_pool; });
        dual_println("Checking for reset deadlocks in the full thread pool...");
        BS::thread_pool temp_pool;
        check_deadlock([&temp_pool] { temp_pool.reset(); });
    }
}

// ==========================
// Functions for benchmarking
// ==========================

/**
 * @brief Print the timing of a specific test.
 *
 * @param num_tasks The number of tasks.
 * @param mean_sd An std::pair containing the mean as the first member and standard deviation as the second member.
 */
void print_timing(const BS::concurrency_t num_tasks, const std::pair<double, double>& mean_sd)
{
    if (num_tasks == 0)
        dual_print("Single-threaded");
    else if (num_tasks == 1)
        dual_print("With    1  task");
    else
        dual_print("With ", std::setw(4), num_tasks, " tasks");
    dual_println(", mean execution time was ", std::setw(6), mean_sd.first, " ms with standard deviation ", std::setw(4), mean_sd.second, " ms.");
}

/**
 * @brief Calculate and print the speedup obtained by multithreading.
 *
 * @param timings A vector of the timings corresponding to different numbers of tasks.
 */
void print_speedup(const std::vector<double>& timings, const BS::concurrency_t try_tasks[])
{
    const std::vector<double>::const_iterator min_el = std::min_element(std::begin(timings), std::end(timings));
    const double max_speedup = std::round(timings[0] / *min_el * 10) / 10;
    const BS::concurrency_t num_tasks = try_tasks[min_el - std::begin(timings)];
    dual_println("Maximum speedup obtained by multithreading vs. single-threading: ", max_speedup, "x, using ", num_tasks, " tasks.");
}

/**
 * @brief Calculate the mean and standard deviation of a set of integers.
 *
 * @param timings The integers.
 * @return An std::pair containing the mean as the first member and standard deviation as the second member.
 */
std::pair<double, double> analyze(const std::vector<std::chrono::milliseconds::rep>& timings)
{
    double mean = 0;
    for (size_t i = 0; i < timings.size(); ++i)
        mean += static_cast<double>(timings[i]) / static_cast<double>(timings.size());
    double variance = 0;
    for (size_t i = 0; i < timings.size(); ++i)
        variance += (static_cast<double>(timings[i]) - mean) * (static_cast<double>(timings[i]) - mean) / static_cast<double>(timings.size());
    const double sd = std::sqrt(variance);
    return {mean, sd};
}

/**
 * @brief A function to generate vector elements. Chosen arbitrarily to simulate a typical numerical calculation.
 *
 * @param i The vector number.
 * @param j The element index.
 * @return The value of the element.
 */
double generate_element(const size_t i, const size_t j)
{
    return std::log(std::sqrt(std::exp(std::sin(i) + std::cos(j))));
}

/**
 * @brief Benchmark multithreaded performance.
 */
void check_performance()
{
    // Create a new pool to ensure that we have a fresh start. We use the light version here since we don't need any of the advanced features of the full version.
    BS::thread_pool_light pool;

    // Set the formatting of floating point numbers.
    dual_print(std::fixed, std::setprecision(1));

    // Initialize a timer object to measure execution time.
    BS::timer tmr;

    // Store the number of available hardware threads for easy access.
    const BS::concurrency_t thread_count = pool.get_thread_count();
    dual_println("Using ", thread_count, " threads.");

    // Define the number of tasks to try in each run of the test (0 = single-threaded).
    const BS::concurrency_t try_tasks[] = {0, thread_count / 4, thread_count / 2, thread_count, thread_count * 2, thread_count * 4};

    // How many times to repeat each run of the test in order to collect reliable statistics.
    constexpr size_t repeat = 20;
    dual_println("Each test will be repeated ", repeat, " times to collect reliable statistics.");

    // The target execution time, in milliseconds, of the multi-threaded test with the number of blocks equal to the number of threads. The total time spent on that test will be approximately equal to repeat * target_ms.
    constexpr std::chrono::milliseconds::rep target_ms = 50;

    // Test how many vectors we need to generate, and of what size, to roughly achieve the target execution time.
    dual_println("Determining the number and size of vectors to generate in order to achieve an approximate mean execution time of ", target_ms, " ms with ", thread_count, " tasks...");
    size_t num_vectors = thread_count;
    size_t vector_size = thread_count;
    std::vector<std::vector<double>> vectors;
    auto loop = [&vectors, &vector_size](const size_t start, const size_t end)
    {
        for (size_t i = start; i < end; ++i)
        {
            for (size_t j = 0; j < vector_size; ++j)
                vectors[i][j] = generate_element(i, j);
        }
    };
    do
    {
        num_vectors *= 2;
        vector_size *= 2;
        vectors = std::vector<std::vector<double>>(num_vectors, std::vector<double>(vector_size));
        tmr.start();
        pool.push_loop(num_vectors, loop);
        pool.wait_for_tasks();
        tmr.stop();
    } while (tmr.ms() < target_ms);
    num_vectors = thread_count * static_cast<size_t>(std::llround(static_cast<double>(num_vectors) * static_cast<double>(target_ms) / static_cast<double>(tmr.ms()) / thread_count));

    // Initialize the desired number of vectors.
    vectors = std::vector<std::vector<double>>(num_vectors, std::vector<double>(vector_size));

    // Define vectors to store statistics.
    std::vector<double> different_n_timings;
    std::vector<std::chrono::milliseconds::rep> same_n_timings;

    // Perform the test.
    dual_println("Generating ", num_vectors, " vectors with ", vector_size, " elements each:");
    for (BS::concurrency_t n : try_tasks)
    {
        for (size_t r = 0; r < repeat; ++r)
        {
            tmr.start();
            if (n > 1)
            {
                pool.push_loop(num_vectors, loop, n);
                pool.wait_for_tasks();
            }
            else
            {
                for (size_t i = 0; i < num_vectors; ++i)
                {
                    for (size_t j = 0; j < vector_size; ++j)
                        vectors[i][j] = generate_element(i, j);
                }
            }
            tmr.stop();
            same_n_timings.push_back(tmr.ms());
        }
        std::pair<double, double> mean_sd = analyze(same_n_timings);
        print_timing(n, mean_sd);
        different_n_timings.push_back(mean_sd.first);
        same_n_timings.clear();
    }
    print_speedup(different_n_timings, try_tasks);
}

int main()
{
    const std::string log_filename = "BS_thread_pool_test-" + get_time() + ".log";
    if (output_log)
        log_file.open(log_filename);

    dual_println("BS::thread_pool: a fast, lightweight, and easy-to-use C++17 thread pool library");
    dual_println("(c) 2023 Barak Shoshany (baraksh@gmail.com) (http://baraksh.com)");
    dual_println("GitHub: https://github.com/bshoshany/thread-pool\n");

    dual_println("Thread pool library version is ", BS_THREAD_POOL_VERSION, ".");
    dual_println("Light thread pool library version is ", BS_THREAD_POOL_LIGHT_VERSION, ".");
    dual_println("Hardware concurrency is ", std::thread::hardware_concurrency(), ".");
    if (output_log)
        dual_println("Generating log file: ", log_filename, ".\n");

    dual_println("Important: Please do not run any other applications, especially multithreaded applications, in parallel with this test!");

    if (enable_tests)
        do_tests();

    if (tests_failed == 0)
    {
        if (enable_tests)
            print_header("SUCCESS: Passed all " + std::to_string(tests_succeeded) + " checks!", '+');
        if (enable_benchmarks)
        {
            print_header("Performing benchmarks:");
            check_performance();
            print_header("Thread pool performance test completed!", '+');
        }
        return 0;
    }
    else
    {
        print_header("FAILURE: Passed " + std::to_string(tests_succeeded) + " checks, but failed " + std::to_string(tests_failed) + "!", '+');
        dual_println("\nPlease submit a bug report at https://github.com/bshoshany/thread-pool/issues including the exact specifications of your system (OS, CPU, compiler, etc.) and the generated log file.");
        std::quick_exit(static_cast<int>(tests_failed));
    }
}
