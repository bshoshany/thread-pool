/**
 * @file BS_thread_pool_light_test.cpp
 * @author Barak Shoshany (baraksh@gmail.com) (http://baraksh.com)
 * @version 3.3.0
 * @date 2022-08-03
 * @copyright Copyright (c) 2022 Barak Shoshany. Licensed under the MIT license. If you found this project useful, please consider starring it on GitHub! If you use this library in software of any kind, please provide a link to the GitHub repository https://github.com/bshoshany/thread-pool in the source code and documentation. If you use this library in published research, please cite it as follows: Barak Shoshany, "A C++17 Thread Pool for High-Performance Scientific Computing", doi:10.5281/zenodo.4742687, arXiv:2105.00613 (May 2021)
 *
 * @brief BS::thread_pool_light: a fast, lightweight, and easy-to-use C++17 thread pool library. This program tests all aspects of the light version of the main library, but is not needed in order to use the library.
 */

// Get rid of annoying MSVC warning.
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <algorithm>          // std::min, std::sort, std::unique
#include <atomic>             // std::atomic
#include <chrono>             // std::chrono
#include <cmath>              // std::abs
#include <condition_variable> // std::condition_variable
#include <exception>          // std::exception
#include <future>             // std::future
#include <iostream>           // std::cout, std::endl
#include <memory>             // std::make_unique, std::unique_ptr
#include <mutex>              // std::mutex, std::scoped_lock, std::unique_lock
#include <random>             // std::mt19937_64, std::random_device, std::uniform_int_distribution
#include <stdexcept>          // std::runtime_error
#include <string>             // std::string, std::to_string
#include <thread>             // std::this_thread, std::thread
#include <utility>            // std::forward
#include <vector>             // std::vector

// Include the header file for the thread pool library.
#include "BS_thread_pool_light.hpp"

// ================
// Global variables
// ================

// A global thread pool object to be used throughout the test.
BS::thread_pool_light pool;

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
void print(T&&... items)
{
    (std::cout << ... << std::forward<T>(items));
}

/**
 * @brief Print any number of items into both std::cout and the log file, syncing both independently. Also prints a newline character, and flushes the stream.
 *
 * @tparam T The types of the items.
 * @param items The items to print.
 */
template <typename... T>
void println(T&&... items)
{
    print(std::forward<T>(items)..., static_cast<std::ostream& (&)(std::ostream&)>(std::endl));
}

/**
 * @brief Print a stylized header.
 *
 * @param text The text of the header. Will appear between two lines.
 * @param symbol The symbol to use for the lines. Default is '='.
 */
void print_header(const std::string& text, const char symbol = '=')
{
    println();
    println(std::string(text.length(), symbol));
    println(text);
    println(std::string(text.length(), symbol));
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
        println("-> PASSED!");
        ++tests_succeeded;
    }
    else
    {
        println("-> FAILED!");
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
    print("Expected: ", expected, ", obtained: ", obtained);
    if (expected == obtained)
    {
        println(" -> PASSED!");
        ++tests_succeeded;
    }
    else
    {
        println(" -> FAILED!");
        ++tests_failed;
    }
}

// =========================================
// Functions to verify the number of threads
// =========================================

/**
 * @brief Count the number of unique threads in the pool. Submits a number of tasks equal to twice the thread count into the pool. Each task stores the ID of the thread running it, and then waits until released by the main thread. This ensures that each thread in the pool runs at least one task. The number of unique thread IDs is then counted from the stored IDs.
 */
std::condition_variable ID_cv, total_cv;
std::mutex ID_mutex, total_mutex;
BS::concurrency_t count_unique_threads()
{
    const BS::concurrency_t num_tasks = pool.get_thread_count() * 2;
    std::vector<std::thread::id> thread_IDs(num_tasks);
    std::unique_lock<std::mutex> total_lock(total_mutex);
    BS::concurrency_t total_count = 0;
    bool ID_release = false;
    pool.wait_for_tasks();
    for (std::thread::id& id : thread_IDs)
        pool.push_task(
            [&total_count, &id, &ID_release]
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
    total_cv.wait(total_lock, [&total_count] { return total_count == pool.get_thread_count(); });
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

/**
 * @brief Check that the constructor works.
 */
void check_constructor()
{
    println("Checking that the thread pool reports a number of threads equal to the hardware concurrency...");
    check(std::thread::hardware_concurrency(), pool.get_thread_count());
    println("Checking that the manually counted number of unique thread IDs is equal to the reported number of threads...");
    check(pool.get_thread_count(), count_unique_threads());
}

// =======================================
// Functions to verify submission of tasks
// =======================================

/**
 * @brief Check that push_task() works.
 */
void check_push_task()
{
    println("Checking that push_task() works for a function with no arguments or return value...");
    {
        bool flag = false;
        pool.push_task([&flag] { flag = true; });
        pool.wait_for_tasks();
        check(flag);
    }
    println("Checking that push_task() works for a function with one argument and no return value...");
    {
        bool flag = false;
        pool.push_task([](bool* flag_) { *flag_ = true; }, &flag);
        pool.wait_for_tasks();
        check(flag);
    }
    println("Checking that push_task() works for a function with two arguments and no return value...");
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
    println("Checking that submit() works for a function with no arguments or return value...");
    {
        bool flag = false;
        pool.submit([&flag] { flag = true; }).wait();
        check(flag);
    }
    println("Checking that submit() works for a function with one argument and no return value...");
    {
        bool flag = false;
        pool.submit([](bool* flag_) { *flag_ = true; }, &flag).wait();
        check(flag);
    }
    println("Checking that submit() works for a function with two arguments and no return value...");
    {
        bool flag1 = false;
        bool flag2 = false;
        pool.submit([](bool* flag1_, bool* flag2_) { *flag1_ = *flag2_ = true; }, &flag1, &flag2).wait();
        check(flag1 && flag2);
    }
    println("Checking that submit() works for a function with no arguments and a return value...");
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
    println("Checking that submit() works for a function with one argument and a return value...");
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
    println("Checking that submit() works for a function with two arguments and a return value...");
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
}

class flag_class
{
public:
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

    bool get_flag() const
    {
        return flag;
    }

    void push_test_flag_no_args()
    {
        pool.push_task(&flag_class::set_flag_no_args, this);
        pool.wait_for_tasks();
        check(get_flag());
    }

    void push_test_flag_one_arg()
    {
        pool.push_task(&flag_class::set_flag_one_arg, this, true);
        pool.wait_for_tasks();
        check(get_flag());
    }

    void submit_test_flag_no_args()
    {
        pool.submit(&flag_class::set_flag_no_args, this).wait();
        check(get_flag());
    }

    void submit_test_flag_one_arg()
    {
        pool.submit(&flag_class::set_flag_one_arg, this, true).wait();
        check(get_flag());
    }

    void submit_test_flag_no_args_return()
    {
        std::future<int> flag_future = pool.submit(&flag_class::set_flag_no_args_return, this);
        check(flag_future.get() == 42 && get_flag());
    }

    void submit_test_flag_one_arg_return()
    {
        std::future<int> flag_future = pool.submit(&flag_class::set_flag_one_arg_return, this, true);
        check(flag_future.get() == 42 && get_flag());
    }

private:
    bool flag = false;
};

/**
 * @brief Check that submitting member functions works.
 */
void check_member_function()
{
    println("Checking that push_task() works for a member function with no arguments or return value...");
    {
        flag_class flag;
        pool.push_task(&flag_class::set_flag_no_args, &flag);
        pool.wait_for_tasks();
        check(flag.get_flag());
    }
    println("Checking that push_task() works for a member function with one argument and no return value...");
    {
        flag_class flag;
        pool.push_task(&flag_class::set_flag_one_arg, &flag, true);
        pool.wait_for_tasks();
        check(flag.get_flag());
    }
    println("Checking that submit() works for a member function with no arguments or return value...");
    {
        flag_class flag;
        pool.submit(&flag_class::set_flag_no_args, &flag).wait();
        check(flag.get_flag());
    }
    println("Checking that submit() works for a member function with one argument and no return value...");
    {
        flag_class flag;
        pool.submit(&flag_class::set_flag_one_arg, &flag, true).wait();
        check(flag.get_flag());
    }
    println("Checking that submit() works for a member function with no arguments and a return value...");
    {
        flag_class flag;
        std::future<int> flag_future = pool.submit(&flag_class::set_flag_no_args_return, &flag);
        check(flag_future.get() == 42 && flag.get_flag());
    }
    println("Checking that submit() works for a member function with one argument and a return value...");
    {
        flag_class flag;
        std::future<int> flag_future = pool.submit(&flag_class::set_flag_one_arg_return, &flag, true);
        check(flag_future.get() == 42 && flag.get_flag());
    }
}

/**
 * @brief Check that submitting member functions within an object works.
 */
void check_member_function_within_object()
{
    println("Checking that push_task() works within an object for a member function with no arguments or return value...");
    {
        flag_class flag;
        flag.push_test_flag_no_args();
    }
    println("Checking that push_task() works within an object for a member function with one argument and no return value...");
    {
        flag_class flag;
        flag.push_test_flag_one_arg();
    }
    println("Checking that submit() works within an object for a member function with no arguments or return value...");
    {
        flag_class flag;
        flag.submit_test_flag_no_args();
    }
    println("Checking that submit() works within an object for a member function with one argument and no return value...");
    {
        flag_class flag;
        flag.submit_test_flag_one_arg();
    }
    println("Checking that submit() works within an object for a member function with no arguments and a return value...");
    {
        flag_class flag;
        flag.submit_test_flag_no_args_return();
    }
    println("Checking that submit() works within an object for a member function with one argument and a return value...");
    {
        flag_class flag;
        flag.submit_test_flag_one_arg_return();
    }
}

/**
 * @brief Check that wait_for_tasks() works.
 */
void check_wait_for_tasks()
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
    println("Waiting for tasks...");
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
 * @brief Check that push_loop() works for a specific range of indices split over a specific number of tasks, with no return value.
 *
 * @param random_start The first index in the loop.
 * @param random_end The last index in the loop plus 1.
 * @param num_tasks The number of tasks.
 */
template <typename T>
void check_push_loop_no_return(const int64_t random_start, T random_end, const BS::concurrency_t num_tasks)
{
    if (random_start == random_end)
        ++random_end;
    println("Verifying that push_loop() from ", random_start, " to ", random_end, " with ", num_tasks, num_tasks == 1 ? " task" : " tasks", " modifies all indices...");
    const size_t num_indices = static_cast<size_t>(std::abs(random_end - random_start));
    const int64_t offset = std::min(random_start, static_cast<int64_t>(random_end));
    std::unique_ptr<std::atomic<bool>[]> flags = std::make_unique<std::atomic<bool>[]>(num_indices);
    const auto loop = [&flags, offset](const int64_t start, const int64_t end)
    {
        for (int64_t i = start; i < end; ++i)
            flags[static_cast<size_t>(i - offset)] = true;
    };
    if (random_start == 0)
        pool.push_loop(random_end, loop, num_tasks);
    else
        pool.push_loop(random_start, random_end, loop, num_tasks);
    pool.wait_for_tasks();
    bool all_flags = true;
    for (size_t i = 0; i < num_indices; ++i)
        all_flags = all_flags && flags[i];
    check(all_flags);
}

/**
 * @brief Check that push_loop() works using several different random values for the range of indices and number of tasks.
 */
void check_parallelize_loop()
{
    std::mt19937_64 mt(rd());
    std::uniform_int_distribution<int64_t> index_dist(-1000000, 1000000);
    std::uniform_int_distribution<BS::concurrency_t> task_dist(1, pool.get_thread_count());
    constexpr uint64_t n = 10;
    for (uint64_t i = 0; i < n; ++i)
        check_push_loop_no_return(index_dist(mt), index_dist(mt), task_dist(mt));
    println("Verifying that push_loop() with identical start and end indices does nothing...");
    bool flag = true;
    const int64_t index = index_dist(mt);
    pool.push_loop(index, index, [&flag](const int64_t, const int64_t) { flag = false; });
    pool.wait_for_tasks();
    check(flag);
    println("Trying push_loop() with start and end indices of different types:");
    const int64_t start = index_dist(mt);
    const uint32_t end = static_cast<uint32_t>(std::abs(index_dist(mt)));
    check_push_loop_no_return(start, end, task_dist(mt));
    println("Trying the overload for push_loop() for the case where the first index is equal to 0:");
    check_push_loop_no_return(0, index_dist(mt), task_dist(mt));
}

// ======================================
// Functions to verify exception handling
// ======================================

/**
 * @brief Check that exception handling works.
 */
void check_exceptions()
{
    println("Checking that exceptions are forwarded correctly by submit()...");
    bool caught = false;
    auto throws = []
    {
        println("Throwing exception...");
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
    println("Adding two vectors with ", vector_size, " elements using ", num_tasks, " tasks...");
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
 */
void check_vectors()
{
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

    print_header("Checking that push_task() works:");
    check_push_task();

    print_header("Checking that submit() works:");
    check_submit();

    print_header("Checking that submitting member functions works:");
    check_member_function();

    print_header("Checking that submitting member functions from within an object works:");
    check_member_function_within_object();

    print_header("Checking that wait_for_tasks() works...");
    check_wait_for_tasks();

    print_header("Checking that push_loop() works:");
    check_parallelize_loop();

    print_header("Checking that exception handling works:");
    check_exceptions();

    print_header("Testing that vector operations produce the expected results:");
    check_vectors();
}

int main()
{
    println("BS::thread_pool_light: a fast, lightweight, and easy-to-use C++17 thread pool library");
    println("(c) 2022 Barak Shoshany (baraksh@gmail.com) (http://baraksh.com)");
    println("GitHub: https://github.com/bshoshany/thread-pool\n");

    println("Thread pool library version is ", BS_THREAD_POOL_VERSION, ".");
    println("Hardware concurrency is ", std::thread::hardware_concurrency(), ".");

    println("Important: Please do not run any other applications, especially multithreaded applications, in parallel with this test!");

    do_tests();

    if (tests_failed == 0)
    {
        print_header("SUCCESS: Passed all " + std::to_string(tests_succeeded) + " checks!", '+');
        return EXIT_SUCCESS;
    }
    else
    {
        print_header("FAILURE: Passed " + std::to_string(tests_succeeded) + " checks, but failed " + std::to_string(tests_failed) + "!", '+');
        println("\nPlease submit a bug report at https://github.com/bshoshany/thread-pool/issues including the exact specifications of your system (OS, CPU, compiler, etc.) and the generated log file.");
        return EXIT_FAILURE;
    }
}
