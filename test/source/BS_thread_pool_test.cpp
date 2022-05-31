/**
 * @file BS_thread_pool_test.cpp
 * @author Barak Shoshany (baraksh@gmail.com) (http://baraksh.com)
 * @version 3.0.0
 * @date 2022-05-30
 * @copyright Copyright (c) 2022 Barak Shoshany. Licensed under the MIT license. If you use this library in software of any kind, please provide a link to the GitHub repository https://github.com/bshoshany/thread-pool in the source code and documentation. If you use this library in published research, please cite it as follows: Barak Shoshany, "A C++17 Thread Pool for High-Performance Scientific Computing", doi:10.5281/zenodo.4742687, arXiv:2105.00613 (May 2021)
 *
 * @brief BS::thread_pool: a fast, lightweight, and easy-to-use C++17 thread pool library. This program tests all aspects of the library, but is not needed in order to use the library.
 */

// Get rid of annoying MSVC warning.
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <algorithm> // std::min, std::min_element, std::sort, std::unique
#include <atomic>    // std::atomic
#include <chrono>    // std::chrono
#include <cmath>     // std::abs, std::llround, std::round, std::sqrt
#include <ctime>     // std::localtime, std::strftime, std::time_t
#include <exception> // std::exception
#include <fstream>   // std::ofstream
#include <future>    // std::future
#include <iomanip>   // std::setprecision, std::setw
#include <ios>       // std::fixed
#include <iostream>  // std::cout
#include <limits>    // std::numeric_limits
#include <random>    // std::mt19937_64, std::random_device, std::uniform_int_distribution, std::uniform_real_distribution
#include <stdexcept> // std::runtime_error
#include <string>    // std::string, std::to_string
#include <thread>    // std::this_thread, std::thread
#include <utility>   // std::pair
#include <vector>    // std::begin, std::end, std::vector

// Include the header file for the thread pool library.
#include "BS_thread_pool.hpp"

// ================
// Global variables
// ================

// Set to false to disable output to a log file.
constexpr bool output_log = true;

// Set to false to disable testing.
constexpr bool enable_tests = true;

// Set to false to disable the benchmarks.
constexpr bool enable_benchmarks = true;

// Two global synced_streams objects. One prints to std::cout, and the other to a file.
BS::synced_stream sync_cout(std::cout);
std::ofstream log_file;
BS::synced_stream sync_file(log_file);

// A global thread pool object to be used throughout the test.
BS::thread_pool pool;

// A global random_device object to be used to seed some random number generators.
std::random_device rd;

// Global variables to measure how many checks succeeded and how many failed.
size_t tests_succeeded = 0;
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
void dual_print(const T&... items)
{
    sync_cout.print(items...);
    if (output_log)
        sync_file.print(items...);
}

/**
 * @brief Print any number of items into both std::cout and the log file, followed by a newline character, syncing both independently.
 *
 * @tparam T The types of the items.
 * @param items The items to print.
 */
template <typename... T>
void dual_println(const T&... items)
{
    dual_print(items..., '\n');
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
    return std::string(time_string);
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

// =========================================
// Functions to verify the number of threads
// =========================================

/**
 * @brief Store the ID of the current thread in memory. Waits for a short time to ensure it does not get evaluated by more than one thread.
 *
 * @param location A pointer to the location where the thread ID should be stored.
 */
void store_ID(std::thread::id* location)
{
    *location = std::this_thread::get_id();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

/**
 * @brief Count the number of unique threads in the thread pool to ensure that the correct number of individual threads was created. Pushes a number of tasks equal to four times the thread count into the thread pool, and count the number of unique thread IDs returned by the tasks.
 */
BS::concurrency_t count_unique_threads()
{
    std::vector<std::thread::id> thread_IDs(pool.get_thread_count() * 4);
    BS::multi_future<void> futures;
    for (std::thread::id& id : thread_IDs)
        futures.f.push_back(pool.submit(store_ID, &id));
    futures.wait();
    std::sort(thread_IDs.begin(), thread_IDs.end());
    BS::concurrency_t unique_threads = (BS::concurrency_t)(std::unique(thread_IDs.begin(), thread_IDs.end()) - thread_IDs.begin());
    return unique_threads;
}

/**
 * @brief Check that the constructor works.
 */
void check_constructor()
{
    dual_println("Checking that the thread pool reports a number of threads equal to the hardware concurrency...");
    check(pool.get_thread_count() == std::thread::hardware_concurrency());
    dual_println("Checking that the manually counted number of unique thread IDs is equal to the reported number of threads...");
    check(pool.get_thread_count() == count_unique_threads());
}

/**
 * @brief Check that reset() works.
 */
void check_reset()
{
    pool.reset(std::thread::hardware_concurrency() / 2);
    dual_println("Checking that after reset() the thread pool reports a number of threads equal to half the hardware concurrency...");
    check(pool.get_thread_count() == std::thread::hardware_concurrency() / 2);
    dual_println("Checking that after reset() the manually counted number of unique thread IDs is equal to the reported number of threads...");
    check(pool.get_thread_count() == count_unique_threads());
    pool.reset(std::thread::hardware_concurrency());
    dual_println("Checking that after a second reset() the thread pool reports a number of threads equal to the hardware concurrency...");
    check(pool.get_thread_count() == std::thread::hardware_concurrency());
    dual_println("Checking that after a second reset() the manually counted number of unique thread IDs is equal to the reported number of threads...");
    check(pool.get_thread_count() == count_unique_threads());
}

// =======================================
// Functions to verify submission of tasks
// =======================================

/**
 * @brief Check that push_task() works.
 */
void check_push_task()
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
}

/**
 * @brief Check that submit() works.
 */
void check_submit()
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
        std::future<int> my_future = pool.submit(
            [&flag]
            {
                flag = true;
                return 42;
            });
        check(my_future.get() == 42 && flag);
    }
    dual_println("Checking that submit() works for a function with one argument and a return value...");
    {
        bool flag = false;
        std::future<int> my_future = pool.submit(
            [](bool* flag_)
            {
                *flag_ = true;
                return 42;
            },
            &flag);
        check(my_future.get() == 42 && flag);
    }
    dual_println("Checking that submit() works for a function with two arguments and a return value...");
    {
        bool flag1 = false;
        bool flag2 = false;
        std::future<int> my_future = pool.submit(
            [](bool* flag1_, bool* flag2_)
            {
                *flag1_ = *flag2_ = true;
                return 42;
            },
            &flag1, &flag2);
        check(my_future.get() == 42 && flag1 && flag2);
    }
}

/**
 * @brief Check that wait_for_tasks() works.
 */
void check_wait_for_tasks()
{
    const BS::concurrency_t n = pool.get_thread_count() * 10;
    std::vector<std::atomic<bool>> flags(n);
    for (BS::concurrency_t i = 0; i < n; ++i)
        pool.push_task(
            [&flags, i]
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                flags[i] = true;
            });
    pool.wait_for_tasks();
    bool all_flags = true;
    for (BS::concurrency_t i = 0; i < n; ++i)
        all_flags = all_flags && flags[i];
    check(all_flags);
}

// ========================================
// Functions to verify loop parallelization
// ========================================

/**
 * @brief Check that parallelize_loop() works for a specific number of indices split over a specific number of tasks, with no return value.
 *
 * @param start The first index in the loop.
 * @param end The last index in the loop plus 1.
 * @param num_tasks The number of tasks.
 */
void check_parallelize_loop_no_return(const int64_t random_start, int64_t random_end, const BS::concurrency_t num_tasks)
{
    if (random_start == random_end)
        ++random_end;
    dual_println("Verifying that a loop from ", random_start, " to ", random_end, " with ", num_tasks, num_tasks == 1 ? " task" : " tasks", " modifies all indices...");
    const size_t num_indices = static_cast<size_t>(std::abs(random_end - random_start));
    const int64_t offset = std::min(random_start, random_end);
    std::vector<std::atomic<bool>> flags(num_indices);
    pool.parallelize_loop(
            random_start, random_end,
            [&flags, offset](const int64_t start, const int64_t end)
            {
                for (int64_t i = start; i < end; ++i)
                    flags[(size_t)(i - offset)] = true;
            },
            num_tasks)
        .wait();
    bool all_flags = true;
    for (size_t i = 0; i < num_indices; ++i)
        all_flags = all_flags && flags[i];
    check(all_flags);
}

/**
 * @brief Check that parallelize_loop() works for a specific number of indices split over a specific number of tasks, with a return value.
 *
 * @param start The first index in the loop.
 * @param end The last index in the loop plus 1.
 * @param num_tasks The number of tasks.
 */
void check_parallelize_loop_return(const int64_t random_start, int64_t random_end, const BS::concurrency_t num_tasks)
{
    if (random_start == random_end)
        ++random_end;
    dual_println("Verifying that a loop from ", random_start, " to ", random_end, " with ", num_tasks, num_tasks == 1 ? " task" : " tasks", " correctly sums all indices...");
    const std::vector<int64_t> sums_vector = pool.parallelize_loop(
                                                     random_start, random_end,
                                                     [](const int64_t start, const int64_t end)
                                                     {
                                                         int64_t total = 0;
                                                         for (int64_t i = start; i < end; ++i)
                                                             total += i;
                                                         return total;
                                                     },
                                                     num_tasks)
                                                 .get();
    int64_t sum = 0;
    for (const int64_t& s : sums_vector)
        sum += s;
    check(sum * 2 == std::abs(random_start - random_end) * (random_start + random_end - 1));
}

/**
 * @brief Check that parallelize_loop() works using several different random values for the range of indices and number of tasks.
 */
void check_parallelize_loop()
{
    std::mt19937_64 mt(rd());
    std::uniform_int_distribution<int64_t> index_dist(-1000000, 1000000);
    std::uniform_int_distribution<BS::concurrency_t> task_dist(1, pool.get_thread_count());
    constexpr uint64_t n = 10;
    for (uint64_t i = 0; i < n; ++i)
        check_parallelize_loop_no_return(index_dist(mt), index_dist(mt), task_dist(mt));
    for (uint64_t i = 0; i < n; ++i)
        check_parallelize_loop_return(index_dist(mt), index_dist(mt), task_dist(mt));
}

// ===============================================
// Functions to verify task monitoring and control
// ===============================================

/**
 * @brief Check that task monitoring works.
 */
void check_task_monitoring()
{
    BS::concurrency_t n = std::min<BS::concurrency_t>(std::thread::hardware_concurrency(), 4);
    dual_println("Resetting pool to ", n, " threads.");
    pool.reset(n);
    dual_println("Submitting ", n * 3, " tasks.");
    std::vector<std::atomic<bool>> release(n * 3);
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
    check(pool.get_tasks_total() == n * 3 && pool.get_tasks_running() == n && pool.get_tasks_queued() == n * 2);
    for (BS::concurrency_t i = 0; i < n; ++i)
        release[i] = true;
    std::this_thread::sleep_for(sleep_time);
    dual_println("After releasing ", n, " tasks, should have: ", n * 2, " tasks total, ", n, " tasks running, ", n, " tasks queued...");
    check(pool.get_tasks_total() == n * 2 && pool.get_tasks_running() == n && pool.get_tasks_queued() == n);
    for (BS::concurrency_t i = n; i < n * 2; ++i)
        release[i] = true;
    std::this_thread::sleep_for(sleep_time);
    dual_println("After releasing ", n, " more tasks, should have: ", n, " tasks total, ", n, " tasks running, ", 0, " tasks queued...");
    check(pool.get_tasks_total() == n && pool.get_tasks_running() == n && pool.get_tasks_queued() == 0);
    for (BS::concurrency_t i = n * 2; i < n * 3; ++i)
        release[i] = true;
    std::this_thread::sleep_for(sleep_time);
    dual_println("After releasing the final ", n, " tasks, should have: ", 0, " tasks total, ", 0, " tasks running, ", 0, " tasks queued...");
    check(pool.get_tasks_total() == 0 && pool.get_tasks_running() == 0 && pool.get_tasks_queued() == 0);
    dual_println("Resetting pool to ", std::thread::hardware_concurrency(), " threads.");
    pool.reset(std::thread::hardware_concurrency());
}

/**
 * @brief Check that pausing works.
 */
void check_pausing()
{
    BS::concurrency_t n = std::min<BS::concurrency_t>(std::thread::hardware_concurrency(), 4);
    dual_println("Resetting pool to ", n, " threads.");
    pool.reset(n);
    dual_println("Pausing pool.");
    pool.paused = true;
    dual_println("Submitting ", n * 3, " tasks, each one waiting for 200ms.");
    for (BS::concurrency_t i = 0; i < n * 3; ++i)
        pool.push_task(
            [i]
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                dual_println("Task ", i, " done.");
            });
    dual_println("Immediately after submission, should have: ", n * 3, " tasks total, ", 0, " tasks running, ", n * 3, " tasks queued...");
    check(pool.get_tasks_total() == n * 3 && pool.get_tasks_running() == 0 && pool.get_tasks_queued() == n * 3);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    dual_println("300ms later, should still have: ", n * 3, " tasks total, ", 0, " tasks running, ", n * 3, " tasks queued...");
    check(pool.get_tasks_total() == n * 3 && pool.get_tasks_running() == 0 && pool.get_tasks_queued() == n * 3);
    dual_println("Unpausing pool.");
    pool.paused = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    dual_println("300ms later, should have: ", n * 2, " tasks total, ", n, " tasks running, ", n, " tasks queued...");
    check(pool.get_tasks_total() == n * 2 && pool.get_tasks_running() == n && pool.get_tasks_queued() == n);
    dual_println("Pausing pool and using wait_for_tasks() to wait for the running tasks.");
    pool.paused = true;
    pool.wait_for_tasks();
    dual_println("After waiting, should have: ", n, " tasks total, ", 0, " tasks running, ", n, " tasks queued...");
    check(pool.get_tasks_total() == n && pool.get_tasks_running() == 0 && pool.get_tasks_queued() == n);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    dual_println("200ms later, should still have: ", n, " tasks total, ", 0, " tasks running, ", n, " tasks queued...");
    check(pool.get_tasks_total() == n && pool.get_tasks_running() == 0 && pool.get_tasks_queued() == n);
    dual_println("Unpausing pool and using wait_for_tasks() to wait for all tasks.");
    pool.paused = false;
    pool.wait_for_tasks();
    dual_println("After waiting, should have: ", 0, " tasks total, ", 0, " tasks running, ", 0, " tasks queued...");
    check(pool.get_tasks_total() == 0 && pool.get_tasks_running() == 0 && pool.get_tasks_queued() == 0);
    dual_println("Resetting pool to ", std::thread::hardware_concurrency(), " threads.");
    pool.reset(std::thread::hardware_concurrency());
}

// ======================================
// Functions to verify exception handling
// ======================================

/**
 * @brief Check that exception handling work.
 */
void check_exceptions()
{
    bool caught = false;
    std::future<void> my_future = pool.submit([] { throw std::runtime_error("Exception thrown!"); });
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

// =====================================
// Functions to verify vector operations
// =====================================

/**
 * @brief Check that parallelized vector operations work as expected by calculating the sum of two randomized vectors of a specific size in two ways, single-threaded and multithreaded, and comparing the results.
 */
void check_vector_of_size(const size_t vector_size, const BS::concurrency_t num_tasks)
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
    pool.parallelize_loop(
            0, vector_size,
            [&sum_multi, &vector_1, &vector_2](const size_t start, const size_t end)
            {
                for (size_t i = start; i < end; ++i)
                    sum_multi[i] = vector_1[i] + vector_2[i];
            },
            num_tasks)
        .wait();
    bool vectors_equal = true;
    for (size_t i = 0; i < vector_size; ++i)
        vectors_equal = vectors_equal && (sum_single[i] == sum_multi[i]);
    check(vectors_equal);
}

/**
 * @brief Check that parallelized vector operations work as expected by calculating the sum of two randomized vectors in two ways, single-threaded and multithreaded, and comparing the results.
 */
void check_vectors()
{
    pool.reset();
    std::mt19937_64 mt(rd());
    std::uniform_int_distribution<size_t> size_dist(0, 1000000);
    std::uniform_int_distribution<BS::concurrency_t> task_dist(1, pool.get_thread_count());
    for (size_t i = 0; i < 10; ++i)
        check_vector_of_size(size_dist(mt), task_dist(mt));
}

// ==================
// Main test function
// ==================

/**
 * @brief Test that various aspects of the library are working as expected.
 */
void do_tests()
{
    print_header("Checking that the constructor works:");
    check_constructor();

    print_header("Checking that reset() works:");
    check_reset();

    print_header("Checking that push_task() works:");
    check_push_task();

    print_header("Checking that submit() works:");
    check_submit();

    print_header("Checking that wait_for_tasks() works...");
    check_wait_for_tasks();

    print_header("Checking that parallelize_loop() works:");
    check_parallelize_loop();

    print_header("Checking that task monitoring works:");
    check_task_monitoring();

    print_header("Checking that pausing works:");
    check_pausing();

    print_header("Checking that exception handling works:");
    check_exceptions();

    print_header("Testing that vector operations produce the expected results:");
    check_vectors();
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
    return std::pair(mean, sd);
}

/**
 * @brief Generate a seed. The std::mt19937_64 in each task will be seeded using this function in order to avoid depleting the entropy of the random_device.
 *
 * @return A random unsigned 64-bit integer.
 */
uint64_t generate_seed()
{
    static std::mt19937_64 mt(rd());
    return mt();
}

/**
 * @brief Benchmark multithreaded performance by generating random vectors.
 */
void check_performance()
{
    // Reset the pool to ensure that we have a fresh start.
    pool.reset();

    // Set the formatting of floating point numbers.
    dual_print(std::fixed, std::setprecision(1));

    // Initialize a random distribution to randomize vectors with arbitrary floating point values.
    const double range = std::sqrt(std::numeric_limits<double>::max());
    std::uniform_real_distribution<double> vector_dist(-range, range);

    // Initialize a timer object to measure execution time.
    BS::timer tmr;

    // Store the number of available hardware threads for easy access.
    const BS::concurrency_t thread_count = pool.get_thread_count();
    dual_println("Using ", thread_count, " threads.");

    // Define the number of tasks to try in each run of the test (0 = single-threaded).
    const BS::concurrency_t try_tasks[] = {0, thread_count / 4, thread_count / 2, thread_count, thread_count * 2, thread_count * 4};

    // The size of the vectors to use for the test.
    constexpr size_t vector_size = 500;

    // How many times to repeat each run of the test in order to collect reliable statistics.
    constexpr size_t repeat = 20;
    dual_println("Each test will be repeated ", repeat, " times to collect reliable statistics.");

    // The target duration of the single-threaded test in milliseconds. The total time spent on the test in the single-threaded case will be approximately equal to repeat * target_ms.
    constexpr std::chrono::milliseconds::rep target_ms = 300;

    // Vectors to store statistics.
    std::vector<double> different_n_timings;
    std::vector<std::chrono::milliseconds::rep> same_n_timings;

    // Test how many vectors we need to generate to roughly achieve the target duration.
    size_t num_vectors = 1;
    do
    {
        num_vectors *= 2;
        std::vector<std::vector<double>> vectors(num_vectors, std::vector<double>(vector_size));
        std::mt19937_64 test_mt(rd());
        tmr.start();
        for (size_t i = 0; i < num_vectors; ++i)
        {
            for (size_t j = 0; j < vector_size; ++j)
                vectors[i][j] = vector_dist(test_mt);
        }
        tmr.stop();
    } while (tmr.ms() < target_ms);
    num_vectors = static_cast<size_t>(std::llround(static_cast<double>(num_vectors) * static_cast<double>(target_ms) / static_cast<double>(tmr.ms())));

    // Initialize the desired number of vectors.
    std::vector<std::vector<double>> vectors(num_vectors, std::vector<double>(vector_size));

    // Perform the test.
    dual_println("\nGenerating ", num_vectors, " random vectors with ", vector_size, " elements each:");
    for (BS::concurrency_t n : try_tasks)
    {
        for (size_t r = 0; r < repeat; ++r)
        {
            tmr.start();
            if (n > 1)
            {
                pool.parallelize_loop(
                        0, num_vectors,
                        [&vector_dist, &vectors](const size_t start, const size_t end)
                        {
                            std::mt19937_64 multi_mt(generate_seed());
                            for (size_t i = start; i < end; ++i)
                            {
                                for (size_t j = 0; j < vector_size; ++j)
                                    vectors[i][j] = vector_dist(multi_mt);
                            }
                        },
                        n)
                    .wait();
            }
            else
            {
                std::mt19937_64 single_mt(generate_seed());
                for (size_t i = 0; i < num_vectors; ++i)
                {
                    for (size_t j = 0; j < vector_size; ++j)
                        vectors[i][j] = vector_dist(single_mt);
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

    dual_println("A C++17 Thread Pool for High-Performance Scientific Computing");
    dual_println("(c) 2022 Barak Shoshany (baraksh@gmail.com) (http://baraksh.com)");
    dual_println("GitHub: https://github.com/bshoshany/thread-pool\n");

    dual_println("Thread pool library version is ", BS_THREAD_POOL_VERSION, ".");
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
        return EXIT_SUCCESS;
    }
    else
    {
        print_header("FAILURE: Passed " + std::to_string(tests_succeeded) + " checks, but failed " + std::to_string(tests_failed) + "!", '+');
        dual_println("\nPlease submit a bug report at https://github.com/bshoshany/thread-pool/issues including the exact specifications of your system (OS, CPU, compiler, etc.) and the generated log file.");
        return EXIT_FAILURE;
    }
}
