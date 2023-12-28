/**
 * @file BS_thread_pool_test.cpp
 * @author Barak Shoshany (baraksh@gmail.com) (http://baraksh.com)
 * @version 4.0.1
 * @date 2023-12-28
 * @copyright Copyright (c) 2023 Barak Shoshany. Licensed under the MIT license. If you found this project useful, please consider starring it on GitHub! If you use this library in software of any kind, please provide a link to the GitHub repository https://github.com/bshoshany/thread-pool in the source code and documentation. If you use this library in published research, please cite it as follows: Barak Shoshany, "A C++17 Thread Pool for High-Performance Scientific Computing", doi:10.5281/zenodo.4742687, arXiv:2105.00613 (May 2021)
 *
 * @brief BS::thread_pool: a fast, lightweight, and easy-to-use C++17 thread pool library. This program tests all aspects of the library, but is not needed in order to use the library.
 */

#include <algorithm>   // std::min, std::shuffle, std::sort, std::unique
#include <atomic>      // std::atomic
#include <chrono>      // std::chrono
#include <cmath>       // std::abs, std::cos, std::exp, std::llround, std::log, std::round, std::sqrt
#include <cstddef>     // std::size_t
#include <cstdint>     // std::int_fast64_t
#include <cstdlib>     // std::exit, std::quick_exit
#include <ctime>       // std::localtime, std::strftime, std::time, std::time_t, std::tm
#include <fstream>     // std::ofstream
#include <functional>  // std::function
#include <future>      // std::future
#include <iomanip>     // std::setprecision, std::setw
#include <ios>         // std::fixed
#include <limits>      // std::numeric_limits
#include <mutex>       // std::mutex, std::scoped_lock
#include <random>      // std::mt19937_64, std::random_device, std::uniform_int_distribution
#include <sstream>     // std::ostringstream
#include <stdexcept>   // std::runtime_error
#include <string>      // std::string, std::to_string
#include <string_view> // std::string_view
#include <thread>      // std::this_thread, std::thread
#include <tuple>       // std::ignore
#include <utility>     // std::as_const, std::forward, std::move, std::pair
#include <vector>      // std::vector

#if defined(__APPLE__)
#include <exception> // std::terminate
#endif

// By default, the test program enables all the optional features by defining the suitable macros, so it can test them. However, if the macro `BS_THREAD_POOL_LIGHT_TEST` is defined during compilation, the optional features will not be tested.
#ifndef BS_THREAD_POOL_LIGHT_TEST
#define BS_THREAD_POOL_ENABLE_NATIVE_HANDLES
#define BS_THREAD_POOL_ENABLE_PAUSE
#define BS_THREAD_POOL_ENABLE_PRIORITY
#define BS_THREAD_POOL_ENABLE_WAIT_DEADLOCK_CHECK
#endif

// Include the header files for the thread pool library and its utilities.
#include "BS_thread_pool.hpp"
#include "BS_thread_pool_utils.hpp"

// Macros indicating the version of the thread pool test program.
#define BS_THREAD_POOL_TEST_VERSION_MAJOR 4
#define BS_THREAD_POOL_TEST_VERSION_MINOR 0
#define BS_THREAD_POOL_TEST_VERSION_PATCH 1

#if (BS_THREAD_POOL_TEST_VERSION_MAJOR != BS_THREAD_POOL_VERSION_MAJOR || BS_THREAD_POOL_TEST_VERSION_MINOR != BS_THREAD_POOL_VERSION_MINOR || BS_THREAD_POOL_TEST_VERSION_PATCH != BS_THREAD_POOL_VERSION_PATCH)
#error The versions of BS_thread_pool_test.cpp and BS_thread_pool.hpp do not match. Aborting compilation.
#endif

#if (BS_THREAD_POOL_TEST_VERSION_MAJOR != BS_THREAD_POOL_UTILS_VERSION_MAJOR || BS_THREAD_POOL_TEST_VERSION_MINOR != BS_THREAD_POOL_UTILS_VERSION_MINOR || BS_THREAD_POOL_TEST_VERSION_PATCH != BS_THREAD_POOL_UTILS_VERSION_PATCH)
#error The versions of BS_thread_pool_test.cpp and BS_thread_pool_utils.hpp do not match. Aborting compilation.
#endif

using int64 = std::int_fast64_t;
using std::size_t;

// ================
// Global variables
// ================

// Whether to perform the benchmarks.
bool enable_benchmarks = true;

// Whether to output to a log file in addition to the standard output.
bool enable_log = true;

// Whether to perform the long deadlock tests. Defaults to false since they can take much longer than the other tests.
bool enable_long_deadlock_tests = false;

// Whether to perform the tests.
bool enable_tests = true;

// A global synced stream which prints to the standard output.
BS::synced_stream sync_cout;

// A global stream object used to access the log file.
std::ofstream log_file;

// A global synced stream which prints to the log file.
BS::synced_stream sync_file(log_file);

// A global variable to measure how many checks succeeded.
size_t tests_succeeded = 0;

// A global variable to measure how many checks failed.
size_t tests_failed = 0;

// ======================
// Functions for printing
// ======================

/**
 * @brief Print any number of items into the standard output and the log file, syncing both independently.
 *
 * @tparam T The types of the items.
 * @param items The items to print.
 */
template <typename... T>
void dual_print(T&&... items)
{
    sync_cout.print(std::forward<T>(items)...);
    if (enable_log)
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
void print_header(const std::string_view text, const char symbol = '=')
{
    dual_println();
    dual_println(std::string(text.length(), symbol));
    dual_println(text);
    dual_println(std::string(text.length(), symbol));
}

// =================================
// Functions for checking conditions
// =================================

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

/**
 * @brief Check if all of the flags in a container are set.
 *
 * @param flags The container.
 * @return `true` if all flags are set, or `false` otherwise.
 */
template <typename T>
bool all_flags_set(const T& flags)
{
    for (size_t i = 0; i < flags.size(); ++i)
    {
        if (!flags[i])
            return false;
    }
    return true;
}

/**
 * @brief Check if none of the flags in a container are set.
 *
 * @param flags The container.
 * @return `true` if no flags are set, or `false` otherwise.
 */
template <typename T>
bool no_flags_set(const T& flags)
{
    for (size_t i = 0; i < flags.size(); ++i)
    {
        if (flags[i])
            return false;
    }
    return true;
}

// =======================================
// Functions for generating random numbers
// =======================================

/**
 * @brief Obtain a random number in a specified range.
 *
 * @tparam T The type of the values in the range.
 * @param min The minimum value of the range.
 * @param max The maximum value of the range.
 * @return The random number.
 */
template <typename T>
T random(const T min, const T max)
{
    static std::random_device rand_device;
    static std::mt19937_64 twister(rand_device());
    std::uniform_int_distribution<T> dist(min, max);
    return dist(twister);
}

/**
 * @brief Obtain an ordered pair of two distinct random numbers in a specified range.
 *
 * @tparam T The type of the values in the range.
 * @param min The minimum value of the range.
 * @param max The maximum value of the range. Must be larger than `min`.
 * @return The random numbers.
 */
template <typename T>
std::pair<T, T> random_pair(const T min, const T max)
{
    static std::random_device rand_device;
    static std::mt19937_64 twister(rand_device());
    std::uniform_int_distribution<T> dist(min, max);
    T first = dist(twister);
    T second;
    do
    {
        second = dist(twister);
    } while (second == first);
    if (second < first)
        return {second, first};
    return {first, second};
}

// =======================
// Miscellaneous functions
// =======================

/**
 * @brief Make a string out of items of various types.
 *
 * @tparam T The types of the items.
 * @param items The items.
 * @return The generated string.
 */
template <typename... T>
std::string make_string(T&&... items)
{
    std::ostringstream out; // NOLINT(misc-const-correctness)
    (out << ... << std::forward<T>(items));
    return out.str();
}

/**
 * @brief Detect the compiler used to compile this program.
 *
 * @return A string describing the compiler.
 */
std::string detect_compiler()
{
#if defined(__clang__)
    return make_string("Clang v", __clang_major__, '.', __clang_minor__, '.', __clang_patchlevel__);
#elif defined(__GNUC__)
    return make_string("GCC v", __GNUC__, '.', __GNUC_MINOR__, '.', __GNUC_PATCHLEVEL__);
#elif defined(_MSC_VER)
    return make_string("MSVC v", _MSC_FULL_VER);
#else
    return "Other";
#endif
}

/**
 * @brief Detect the operating system used to compile this program.
 *
 * @return A string describing the operating system.
 */
std::string detect_os()
{
#if defined(__linux__)
    return "Linux";
#elif defined(_WIN32)
    return "Windows";
#elif defined(__APPLE__)
    return "macOS";
#else
    return "Other";
#endif
}

/**
 * @brief Get a string representing the current time. If MSVC is detected, `localtime_s` will be used to avoid warning C4996.
 *
 * @return The string.
 */
std::string get_time()
{
    constexpr std::string_view error = "Error obtaining current time!";
    std::string time_string = "YYYY-MM-DD_HH.MM.SS";
    std::tm local_tm = {};
    const std::time_t epoch = std::time(nullptr);
#if defined(_MSC_VER)
    if (localtime_s(&local_tm, &epoch) != 0)
    {
        dual_println(error);
        return "";
    }
#else
    local_tm = *std::localtime(&epoch);
#endif
    const size_t bytes = std::strftime(time_string.data(), time_string.length() + 1, "%Y-%m-%d_%H.%M.%S", &local_tm);
    if (bytes != time_string.length())
    {
        dual_println(error);
        return "";
    }
    return time_string;
}

// =========================================
// Functions to verify the number of threads
// =========================================

/**
 * @brief Obtain a list of unique thread IDs in the pool. Submits a number of tasks equal to twice the thread count into the pool. Each task stores the ID of the thread running it, and then waits until as many tasks as the thread count are finished. This ensures that each thread in the pool runs at least one task, as the pool gets filled completely.
 *
 * @param pool The thread pool to check.
 */
std::vector<std::thread::id> obtain_unique_threads(BS::thread_pool& pool)
{
    const BS::concurrency_t num_tasks = pool.get_thread_count() * 2;
    std::vector<std::thread::id> thread_ids(num_tasks);
    std::atomic<BS::concurrency_t> total_count = 0;
    BS::signaller signal;
    for (std::thread::id& tid : thread_ids)
    {
        pool.detach_task(
            [&total_count, &tid, &signal, thread_count = pool.get_thread_count()]
            {
                tid = std::this_thread::get_id();
                if (++total_count == thread_count)
                    signal.ready();
                signal.wait();
            });
    }
    pool.wait();
    std::sort(thread_ids.begin(), thread_ids.end());
    const std::vector<std::thread::id>::iterator last = std::unique(thread_ids.begin(), thread_ids.end());
    thread_ids.erase(last, thread_ids.end());
    return thread_ids;
}

/**
 * @brief Check that the constructor works. Also checks that get_thread_ids() reports the correct IDs.
 */
void check_constructor()
{
    BS::thread_pool pool;
    dual_println("Checking that the thread pool reports a number of threads equal to the hardware concurrency...");
    check(std::thread::hardware_concurrency(), pool.get_thread_count());
    dual_println("Checking that the manually counted number of unique thread IDs is equal to the reported number of threads...");
    const std::vector<std::thread::id> unique_threads = obtain_unique_threads(pool);
    check(pool.get_thread_count(), unique_threads.size());
    dual_println("Checking that the unique thread IDs obtained are the same as those reported by get_thread_ids()...");
    std::vector<std::thread::id> threads_from_pool = pool.get_thread_ids();
    std::sort(threads_from_pool.begin(), threads_from_pool.end());
    check(threads_from_pool == unique_threads);
}

/**
 * @brief Check that reset() works.
 */
void check_reset()
{
    BS::thread_pool pool;
    pool.reset(std::thread::hardware_concurrency() * 2);
    dual_println("Checking that after reset() the thread pool reports a number of threads equal to double the hardware concurrency...");
    check(std::thread::hardware_concurrency() * 2, pool.get_thread_count());
    dual_println("Checking that after reset() the manually counted number of unique thread IDs is equal to the reported number of threads...");
    check(pool.get_thread_count(), obtain_unique_threads(pool).size());
    pool.reset(std::thread::hardware_concurrency());
    dual_println("Checking that after a second reset() the thread pool reports a number of threads equal to the hardware concurrency...");
    check(std::thread::hardware_concurrency(), pool.get_thread_count());
    dual_println("Checking that after a second reset() the manually counted number of unique thread IDs is equal to the reported number of threads...");
    check(pool.get_thread_count(), obtain_unique_threads(pool).size());
}

// =======================================
// Functions to verify submission of tasks
// =======================================

/**
 * @brief A class to detect when a copy or move constructor has been invoked.
 */
class [[nodiscard]] detect_copy_move
{
public:
    detect_copy_move() = default;

    detect_copy_move(const detect_copy_move&) : copied(true) {}

    detect_copy_move(detect_copy_move&&) noexcept : moved(true) {}

    detect_copy_move& operator=(const detect_copy_move&) = delete;

    detect_copy_move& operator=(detect_copy_move&&) = delete;

    [[nodiscard]] bool get_copied() const
    {
        return copied;
    };

    [[nodiscard]] bool get_moved() const
    {
        return moved;
    };

private:
    bool copied = false;
    bool moved = false;
}; // class detect_copy_move

/**
 * @brief Check that detach_task() or submit_task() work.
 *
 * @param which_func A string naming the function to check.
 */
void check_task(const std::string_view which_func)
{
    BS::thread_pool pool;
    dual_println("Checking that ", which_func, " works for a function with no arguments or return value...");
    {
        bool flag = false;
        const std::function<void()> func = [&flag]
        {
            flag = true;
        };
        if (which_func == "detach_task()")
        {
            pool.detach_task(func);
            pool.wait();
        }
        else
        {
            pool.submit_task(func).wait();
        }
        check(flag);
    }
    dual_println("Checking that ", which_func, " works for a function with one argument and no return value...");
    {
        bool flag = false;
        const std::function<void(bool&)> func = [](bool& flag_)
        {
            flag_ = true;
        };
        if (which_func == "detach_task()")
        {
            pool.detach_task(
                [&func, &flag]
                {
                    func(flag);
                });
            pool.wait();
        }
        else
        {
            pool.submit_task(
                    [&func, &flag]
                    {
                        func(flag);
                    })
                .wait();
        }
        check(flag);
    }
    dual_println("Checking that ", which_func, " works for a function with two arguments and no return value...");
    {
        bool flag1 = false;
        bool flag2 = false;
        const std::function<void(bool&, bool&)> func = [](bool& flag1_, bool& flag2_)
        {
            flag1_ = flag2_ = true;
        };
        if (which_func == "detach_task()")
        {
            pool.detach_task(
                [&func, &flag1, &flag2]
                {
                    func(flag1, flag2);
                });
            pool.wait();
        }
        else
        {
            pool.submit_task(
                    [&func, &flag1, &flag2]
                    {
                        func(flag1, flag2);
                    })
                .wait();
        }
        check(flag1 && flag2);
    }
    if (which_func == "submit_task()")
    {
        dual_println("Checking that submit_task() works for a function with no arguments and a return value...");
        {
            bool flag = false;
            const std::function<bool()> func = [&flag]
            {
                return (flag = true);
            };
            std::future<bool> flag_future = pool.submit_task(func);
            check(flag_future.get() && flag);
        }
        dual_println("Checking that submit_task() works for a function with one argument and a return value...");
        {
            bool flag = false;
            const std::function<bool(bool&)> func = [](bool& flag_)
            {
                return (flag_ = true);
            };
            std::future<bool> flag_future = pool.submit_task(
                [&func, &flag]
                {
                    return func(flag);
                });
            check(flag_future.get() && flag);
        }
        dual_println("Checking that submit_task() works for a function with two arguments and a return value...");
        {
            bool flag1 = false;
            bool flag2 = false;
            const std::function<bool(bool&, bool&)> func = [](bool& flag1_, bool& flag2_)
            {
                return (flag1_ = flag2_ = true);
            };
            std::future<bool> flag_future = pool.submit_task(
                [&func, &flag1, &flag2]
                {
                    return func(flag1, flag2);
                });
            check(flag_future.get() && flag1 && flag2);
        }
    }
    dual_println("Checking that ", which_func, " does not create unnecessary copies of the function object...");
    {
        bool copied = false;
        bool moved = false;
        std::function<void()> test_copy = [detect = detect_copy_move(), &copied, &moved]
        {
            copied = detect.get_copied();
            moved = detect.get_moved();
        };
        if (which_func == "detach_task()")
        {
            pool.detach_task(std::move(test_copy));
            pool.wait();
        }
        else
        {
            pool.submit_task(std::move(test_copy)).wait();
        }
        check(!copied && moved);
    }
    dual_println("Checking that ", which_func, " correctly accepts arguments passed by value, reference, and constant reference...");
    {
        {
            dual_println("Value:");
            const int64 pass_me_by_value = 0;
            const std::function<void(int64)> func_value = [](int64 passed_by_value)
            {
                if (++passed_by_value != 1)
                    static_cast<void>(0);
            };
            if (which_func == "detach_task()")
            {
                pool.detach_task(
                    [&func_value, pbv = pass_me_by_value]
                    {
                        func_value(pbv);
                    });
                pool.wait();
            }
            else
            {
                pool.submit_task(
                        [&func_value, pbv = pass_me_by_value]
                        {
                            func_value(pbv);
                        })
                    .wait();
            }
            check(pass_me_by_value == 0);
        }
        {
            dual_println("Reference:");
            int64 pass_me_by_ref = 0;
            const std::function<void(int64&)> func_ref = [](int64& passed_by_ref)
            {
                ++passed_by_ref;
            };
            if (which_func == "detach_task()")
            {
                pool.detach_task(
                    [&func_ref, &pass_me_by_ref]
                    {
                        func_ref(pass_me_by_ref);
                    });
                pool.wait();
            }
            else
            {
                pool.submit_task(
                        [&func_ref, &pass_me_by_ref]
                        {
                            func_ref(pass_me_by_ref);
                        })
                    .wait();
            }
            check(pass_me_by_ref == 1);
        }
        {
            dual_println("Constant reference:");
            int64 pass_me_by_cref = 0;
            BS::signaller signal;
            const std::function<void(const int64&)> func_cref = [&signal](const int64& passed_by_cref)
            {
                signal.wait();
                check(passed_by_cref == 1);
            };
            if (which_func == "detach_task()")
            {
                pool.detach_task(
                    [&func_cref, &pass_me_by_cref = std::as_const(pass_me_by_cref)]
                    {
                        func_cref(pass_me_by_cref);
                    });
                ++pass_me_by_cref;
                signal.ready();
                pool.wait();
            }
            else
            {
                const std::future<void> future = pool.submit_task(
                    [&func_cref, &pass_me_by_cref = std::as_const(pass_me_by_cref)]
                    {
                        func_cref(pass_me_by_cref);
                    });
                ++pass_me_by_cref;
                signal.ready();
                future.wait();
            }
        }
    }
}

/**
 * @brief A class to facilitate checking that member functions of different types have been successfully submitted.
 */
class [[nodiscard]] flag_class
{
public:
    explicit flag_class(BS::thread_pool& pool_) : pool(&pool_) {}

    void set_flag_no_args()
    {
        flag = true;
    }

    void set_flag_one_arg(const bool arg)
    {
        flag = arg;
    }

    [[nodiscard]] bool set_flag_no_args_return()
    {
        return (flag = true);
    }

    [[nodiscard]] bool set_flag_one_arg_return(const bool arg)
    {
        return (flag = arg);
    }

    [[nodiscard]] bool get_flag() const
    {
        return flag;
    }

    void detach_test_flag_no_args()
    {
        pool->detach_task(
            [this]
            {
                set_flag_no_args();
            });
        pool->wait();
        check(get_flag());
    }

    void detach_test_flag_one_arg()
    {
        pool->detach_task(
            [this]
            {
                set_flag_one_arg(true);
            });
        pool->wait();
        check(get_flag());
    }

    void submit_test_flag_no_args()
    {
        pool->submit_task(
                [this]
                {
                    set_flag_no_args();
                })
            .wait();
        check(get_flag());
    }

    void submit_test_flag_one_arg()
    {
        pool->submit_task(
                [this]
                {
                    set_flag_one_arg(true);
                })
            .wait();
        check(get_flag());
    }

    void submit_test_flag_no_args_return()
    {
        std::future<bool> flag_future = pool->submit_task(
            [this]
            {
                return set_flag_no_args_return();
            });
        check(flag_future.get() && get_flag());
    }

    void submit_test_flag_one_arg_return()
    {
        std::future<bool> flag_future = pool->submit_task(
            [this]
            {
                return set_flag_one_arg_return(true);
            });
        check(flag_future.get() && get_flag());
    }

private:
    bool flag = false;
    BS::thread_pool* pool;
}; // class flag_class

/**
 * @brief Check that submitting member functions works.
 */
void check_member_function()
{
    BS::thread_pool pool;
    dual_println("Checking that detach_task() works for a member function with no arguments or return value...");
    {
        flag_class flag(pool);
        pool.detach_task(
            [&flag]
            {
                flag.set_flag_no_args();
            });
        pool.wait();
        check(flag.get_flag());
    }
    dual_println("Checking that detach_task() works for a member function with one argument and no return value...");
    {
        flag_class flag(pool);
        pool.detach_task(
            [&flag]
            {
                flag.set_flag_one_arg(true);
            });
        pool.wait();
        check(flag.get_flag());
    }
    dual_println("Checking that submit_task() works for a member function with no arguments or return value...");
    {
        flag_class flag(pool);
        pool.submit_task(
                [&flag]
                {
                    flag.set_flag_no_args();
                })
            .wait();
        check(flag.get_flag());
    }
    dual_println("Checking that submit_task() works for a member function with one argument and no return value...");
    {
        flag_class flag(pool);
        pool.submit_task(
                [&flag]
                {
                    flag.set_flag_one_arg(true);
                })
            .wait();
        check(flag.get_flag());
    }
    dual_println("Checking that submit_task() works for a member function with no arguments and a return value...");
    {
        flag_class flag(pool);
        std::future<bool> flag_future = pool.submit_task(
            [&flag]
            {
                return flag.set_flag_no_args_return();
            });
        check(flag_future.get() && flag.get_flag());
    }
    dual_println("Checking that submit_task() works for a member function with one argument and a return value...");
    {
        flag_class flag(pool);
        std::future<bool> flag_future = pool.submit_task(
            [&flag]
            {
                return flag.set_flag_one_arg_return(true);
            });
        check(flag_future.get() && flag.get_flag());
    }
}

/**
 * @brief Check that submitting member functions within an object works.
 */
void check_member_function_within_object()
{
    BS::thread_pool pool;
    dual_println("Checking that detach_task() works within an object for a member function with no arguments or return value...");
    {
        flag_class flag(pool);
        flag.detach_test_flag_no_args();
    }
    dual_println("Checking that detach_task() works within an object for a member function with one argument and no return value...");
    {
        flag_class flag(pool);
        flag.detach_test_flag_one_arg();
    }
    dual_println("Checking that submit_task() works within an object for a member function with no arguments or return value...");
    {
        flag_class flag(pool);
        flag.submit_test_flag_no_args();
    }
    dual_println("Checking that submit_task() works within an object for a member function with one argument and no return value...");
    {
        flag_class flag(pool);
        flag.submit_test_flag_one_arg();
    }
    dual_println("Checking that submit_task() works within an object for a member function with no arguments and a return value...");
    {
        flag_class flag(pool);
        flag.submit_test_flag_no_args_return();
    }
    dual_println("Checking that submit_task() works within an object for a member function with one argument and a return value...");
    {
        flag_class flag(pool);
        flag.submit_test_flag_one_arg_return();
    }
}

// =====================================
// Functions to verify waiting for tasks
// =====================================

/**
 * @brief Check that wait() works.
 */
void check_wait()
{
    constexpr std::chrono::milliseconds sleep_time(10);
    BS::thread_pool pool;
    const BS::concurrency_t num_tasks = pool.get_thread_count() * 10;
    std::vector<std::atomic<bool>> flags(num_tasks);
    for (BS::concurrency_t i = 0; i < num_tasks; ++i)
    {
        pool.detach_task(
            [&flags, i, sleep_time]
            {
                std::this_thread::sleep_for(sleep_time);
                flags[i] = true;
            });
    }
    dual_println("Waiting for tasks...");
    pool.wait();
    check(all_flags_set(flags));
}

/**
 * @brief Check that wait() correctly blocks all external threads that call it.
 */
void check_wait_blocks()
{
    constexpr std::chrono::milliseconds sleep_time(100);
    constexpr BS::concurrency_t num_waiting_tasks = 4;
    BS::thread_pool pool;
    BS::signaller signal;
    dual_println("Checking that wait() correctly blocks all external threads that call it...");
    pool.detach_task(
        [&signal]
        {
            dual_println("Task submitted to pool 1 and waiting to be released...");
            signal.wait();
            dual_println("Task released.");
        });
    BS::thread_pool temp_pool(num_waiting_tasks);
    std::vector<std::atomic<bool>> flags(num_waiting_tasks);
    const std::function<void(BS::concurrency_t)> waiting_task = [&flags, &pool](const BS::concurrency_t task_num)
    {
        dual_println("Task ", task_num, " submitted to pool 2 and waiting for pool 1's task to finish...");
        pool.wait();
        dual_println("Task ", task_num, " finished waiting.");
        flags[task_num] = true;
    };
    for (BS::concurrency_t i = 0; i < num_waiting_tasks; ++i)
        temp_pool.detach_task(
            [&waiting_task, i]
            {
                waiting_task(i);
            });
    std::this_thread::sleep_for(sleep_time);
    check(no_flags_set(flags));
    signal.ready();
    temp_pool.wait();
    check(all_flags_set(flags));
}

/**
 * @brief Check that wait_for() works.
 */
void check_wait_for()
{
    constexpr std::chrono::milliseconds long_sleep_time(250);
    constexpr std::chrono::milliseconds short_sleep_time(10);
    BS::thread_pool pool;
    dual_println("Checking that wait_for() works...");
    std::atomic<bool> done = false;
    pool.detach_task(
        [&done, long_sleep_time]
        {
            std::this_thread::sleep_for(long_sleep_time);
            done = true;
        });
    dual_println("Task that lasts ", long_sleep_time.count(), "ms submitted. Waiting for ", short_sleep_time.count(), "ms...");
    pool.wait_for(short_sleep_time);
    check(!done);
    dual_println("Waiting for ", long_sleep_time.count() * 2, "ms...");
    pool.wait_for(long_sleep_time * 2);
    check(done);
}

/**
 * @brief Check that wait_until() works.
 */
void check_wait_until()
{
    constexpr std::chrono::milliseconds long_sleep_time(250);
    constexpr std::chrono::milliseconds short_sleep_time(10);
    BS::thread_pool pool;
    dual_println("Checking that wait_until() works...");
    std::atomic<bool> done = false;
    pool.detach_task(
        [&done, long_sleep_time]
        {
            std::this_thread::sleep_for(long_sleep_time);
            done = true;
        });
    const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    dual_println("Task that lasts ", long_sleep_time.count(), "ms submitted. Waiting until ", short_sleep_time.count(), "ms from submission time...");
    pool.wait_until(now + short_sleep_time);
    check(!done);
    dual_println("Waiting until ", long_sleep_time.count() * 2, "ms from submission time...");
    pool.wait_until(now + long_sleep_time * 2);
    check(done);
}

// An auxiliary thread pool used by check_wait_multiple_deadlock(). It's a global variable so that the program will not get stuck upon destruction of this pool if a deadlock actually occurs.
BS::thread_pool check_wait_multiple_deadlock_pool;

/**
 * @brief Check that calling wait() more than once doesn't create a deadlock.
 */
void check_wait_multiple_deadlock()
{
    constexpr std::chrono::milliseconds sleep_time(500);
    constexpr size_t n_waiting_tasks = 1000;
    dual_println("Checking for deadlocks when waiting for tasks...");
    BS::thread_pool pool(1);
    pool.detach_task(
        [sleep_time]
        {
            std::this_thread::sleep_for(sleep_time);
        });
    std::atomic<size_t> count = 0;
    for (size_t j = 0; j < n_waiting_tasks; ++j)
    {
        check_wait_multiple_deadlock_pool.detach_task(
            [&pool, &count]
            {
                pool.wait();
                ++count;
            });
    }
    bool passed = false;
    while (true)
    {
        const size_t old_count = count;
        check_wait_multiple_deadlock_pool.wait_for(sleep_time * 2);
        if (count == n_waiting_tasks)
        {
            dual_println("All waiting tasks successfully finished!");
            passed = true;
            break;
        }
        if (count == old_count)
        {
            dual_println("Error: deadlock detected!");
            passed = false;
            break;
        }
        dual_println(count, " tasks out of ", n_waiting_tasks, " finished waiting...");
    }
    check(passed);
}

#ifdef BS_THREAD_POOL_ENABLE_WAIT_DEADLOCK_CHECK
// An auxiliary thread pool used by check_wait_self_deadlock(). It's a global variable so that the program will not get stuck upon destruction of this pool if a deadlock actually occurs.
BS::thread_pool check_wait_self_deadlock_pool;

/**
 * @brief Check that calling wait() from within a thread of the same pool throws an exception instead of creating a deadlock.
 */
void check_wait_self_deadlock()
{
    constexpr std::chrono::milliseconds sleep_time(100);
    dual_println("Checking for deadlocks when waiting from within a thread of the same pool...");
    std::atomic<bool> passed = false;
    check_wait_self_deadlock_pool.detach_task(
        [&passed]
        {
            try
            {
                check_wait_self_deadlock_pool.wait();
            }
            catch (const BS::thread_pool::wait_deadlock&)
            {
                passed = true;
            }
        });
    check_wait_self_deadlock_pool.wait_for(sleep_time);
    check(passed);
}
#endif

// ========================================
// Functions to verify loop parallelization
// ========================================

/**
 * @brief Check that detach_loop() or submit_loop() work for a specific range of indices split over a specific number of tasks, with no return value.
 *
 * @param pool The thread pool to check.
 * @param random_start The first index in the loop.
 * @param random_end The last index in the loop plus 1.
 * @param num_tasks The number of tasks.
 * @param which_func A string naming the function to check.
 * @return `true` if the check succeeded, `false` otherwise.
 */
bool check_loop_no_return(BS::thread_pool& pool, const int64 random_start, const int64 random_end, const BS::concurrency_t num_tasks, const std::string_view which_func)
{
    dual_println("Verifying that ", which_func, " from ", random_start, " to ", random_end, " with ", num_tasks, num_tasks == 1 ? " task" : " tasks", " modifies all indices exactly once...");
    const size_t num_indices = static_cast<size_t>(random_end - random_start);
    std::vector<std::atomic<int64>> flags(num_indices);
    std::atomic<bool> indices_out_of_range = false;
    const std::function<void(int64)> loop = [&flags, random_start, random_end, &indices_out_of_range](const int64 idx)
    {
        if (idx < random_start || idx > random_end)
            indices_out_of_range = true;
        else
            ++flags[static_cast<size_t>(idx - random_start)];
    };
    if (which_func == "detach_loop()")
    {
        pool.detach_loop(random_start, random_end, loop, num_tasks);
        pool.wait();
    }
    else
    {
        pool.submit_loop(random_start, random_end, loop, num_tasks).wait();
    }
    if (indices_out_of_range)
    {
        dual_println("Error: Loop indices out of range!");
        return false;
    }
    for (size_t i = 0; i < num_indices; ++i)
    {
        if (flags[i] != 1)
            return false;
    }
    return true;
}

/**
 * @brief Check that detach_loop() and submit_loop() work using several different random values for the range of indices and number of tasks.
 */
void check_loop()
{
    constexpr int64 range = 1000000;
    constexpr size_t repeats = 10;
    BS::thread_pool pool;
    for (size_t i = 0; i < repeats; ++i)
    {
        const std::pair<int64, int64> indices = random_pair(-range, range);
        check(check_loop_no_return(pool, indices.first, indices.second, random<BS::concurrency_t>(1, pool.get_thread_count()), "detach_loop()"));
    }
    for (size_t i = 0; i < repeats; ++i)
    {
        const std::pair<int64, int64> indices = random_pair(-range, range);
        check(check_loop_no_return(pool, indices.first, indices.second, random<BS::concurrency_t>(1, pool.get_thread_count()), "submit_loop()"));
    }
    dual_println("Verifying that detach_loop() with identical start and end indices does nothing...");
    {
        std::atomic<size_t> count = 0;
        const int64 index = random(-range, range);
        dual_println("Range: ", index, " to ", index);
        pool.detach_loop(index, index,
            [&count](const int64)
            {
                ++count;
            });
        pool.wait();
        check(count == 0);
    }
    dual_println("Verifying that submit_loop() with identical start and end indices does nothing...");
    {
        std::atomic<size_t> count = 0;
        const int64 index = random(-range, range);
        dual_println("Range: ", index, " to ", index);
        pool.submit_loop(index, index,
                [&count](const int64)
                {
                    ++count;
                })
            .wait();
        check(count == 0);
    }
    dual_println("Verifying that detach_loop() with end index smaller than the start index does nothing...");
    {
        std::atomic<size_t> count = 0;
        const std::pair<int64, int64> indices = random_pair(-range, range);
        dual_println("Range: ", indices.second, " to ", indices.first);
        pool.detach_loop(indices.second, indices.first,
            [&count](const int64)
            {
                ++count;
            });
        pool.wait();
        check(count == 0);
    }
    dual_println("Verifying that submit_loop() with end index smaller than the start index does nothing...");
    {
        std::atomic<size_t> count = 0;
        const std::pair<int64, int64> indices = random_pair(-range, range);
        dual_println("Range: ", indices.second, " to ", indices.first);
        pool.submit_loop(indices.second, indices.first,
                [&count](const int64)
                {
                    ++count;
                })
            .wait();
        check(count == 0);
    }
    dual_println("Trying detach_loop() with a number of tasks larger than the number of indices:");
    {
        const int64 start = random(-range, range);
        check(check_loop_no_return(pool, start, start + random<BS::concurrency_t>(0, pool.get_thread_count() * 2), random<BS::concurrency_t>(pool.get_thread_count() * 2, pool.get_thread_count() * 4), "detach_loop()"));
    }
    dual_println("Trying submit_loop() with a number of tasks larger than the number of indices:");
    {
        const int64 start = random(-range, range);
        check(check_loop_no_return(pool, start, start + random<BS::concurrency_t>(0, pool.get_thread_count() * 2), random<BS::concurrency_t>(pool.get_thread_count() * 2, pool.get_thread_count() * 4), "submit_loop()"));
    }
}

/**
 * @brief Check that detach_blocks() or submit_blocks() work for a specific range of indices split over a specific number of tasks, with no return value.
 *
 * @param pool The thread pool to check.
 * @param random_start The first index in the loop.
 * @param random_end The last index in the loop plus 1.
 * @param num_tasks The number of tasks.
 * @param which_func A string naming the function to check.
 * @return `true` if the check succeeded, `false` otherwise.
 */
bool check_blocks_no_return(BS::thread_pool& pool, const int64 random_start, const int64 random_end, const BS::concurrency_t num_tasks, const std::string_view which_func)
{
    dual_println("Verifying that ", which_func, " from ", random_start, " to ", random_end, " with ", num_tasks, num_tasks == 1 ? " task" : " tasks", " modifies all indices exactly once...");
    const size_t num_indices = static_cast<size_t>(random_end - random_start);
    std::vector<std::atomic<int64>> flags(num_indices);
    std::atomic<bool> indices_out_of_range = false;
    const std::function<void(int64, int64)> loop = [&flags, random_start, random_end, &indices_out_of_range](const int64 start, const int64 end)
    {
        if (start < random_start || end > random_end)
        {
            indices_out_of_range = true;
        }
        else
        {
            for (int64 i = start; i < end; ++i)
                ++flags[static_cast<size_t>(i - random_start)];
        }
    };
    if (which_func == "detach_blocks()")
    {
        pool.detach_blocks(random_start, random_end, loop, num_tasks);
        pool.wait();
    }
    else
    {
        pool.submit_blocks(random_start, random_end, loop, num_tasks).wait();
    }
    if (indices_out_of_range)
    {
        dual_println("Error: Loop indices out of range!");
        return false;
    }
    for (size_t i = 0; i < num_indices; ++i)
    {
        if (flags[i] != 1)
            return false;
    }
    return true;
}

/**
 * @brief Check that submit_blocks() works for a specific range of indices split over a specific number of tasks, with a return value.
 *
 * @param pool The thread pool to check.
 * @param random_start The first index in the loop.
 * @param random_end The last index in the loop plus 1.
 * @param num_tasks The number of tasks.
 */
void check_blocks_return(BS::thread_pool& pool, const int64 random_start, const int64 random_end, const BS::concurrency_t num_tasks)
{
    dual_println("Verifying that submit_blocks() from ", random_start, " to ", random_end, " with ", num_tasks, num_tasks == 1 ? " task" : " tasks", " correctly sums all indices...");
    const std::function<int64(int64, int64)> loop = [](const int64 start, const int64 end)
    {
        int64 total = 0;
        for (int64 i = start; i < end; ++i)
            total += i;
        return total;
    };
    const std::vector<int64> sums_vector = pool.submit_blocks(random_start, random_end, loop, num_tasks).get();
    int64 sum = 0;
    for (const int64 partial_sum : sums_vector)
        sum += partial_sum;
    check(std::abs(random_start - random_end) * (random_start + random_end - 1) / 2, sum);
}

/**
 * @brief Check that detach_blocks() and submit_blocks() work using several different random values for the range of indices and number of tasks.
 */
void check_blocks()
{
    constexpr int64 range = 1000000;
    constexpr size_t repeats = 10;
    BS::thread_pool pool;
    for (size_t i = 0; i < repeats; ++i)
    {
        const std::pair<int64, int64> indices = random_pair(-range, range);
        check(check_blocks_no_return(pool, indices.first, indices.second, random<BS::concurrency_t>(1, pool.get_thread_count()), "detach_blocks()"));
    }
    for (size_t i = 0; i < repeats; ++i)
    {
        const std::pair<int64, int64> indices = random_pair(-range, range);
        check(check_blocks_no_return(pool, indices.first, indices.second, random<BS::concurrency_t>(1, pool.get_thread_count()), "submit_blocks()"));
    }
    for (size_t i = 0; i < repeats; ++i)
    {
        const std::pair<int64, int64> indices = random_pair(-range, range);
        check_blocks_return(pool, indices.first, indices.second, random<BS::concurrency_t>(1, pool.get_thread_count()));
    }
    dual_println("Verifying that detach_blocks() with identical start and end indices does nothing...");
    {
        std::atomic<size_t> count = 0;
        const int64 index = random(-range, range);
        dual_println("Range: ", index, " to ", index);
        pool.detach_blocks(index, index,
            [&count](const int64, const int64)
            {
                ++count;
            });
        pool.wait();
        check(count == 0);
    }
    dual_println("Verifying that submit_blocks() with identical start and end indices does nothing...");
    {
        std::atomic<size_t> count = 0;
        const int64 index = random(-range, range);
        dual_println("Range: ", index, " to ", index);
        pool.submit_blocks(index, index,
                [&count](const int64, const int64)
                {
                    ++count;
                })
            .wait();
        check(count == 0);
    }
    dual_println("Verifying that detach_blocks() with end index smaller than the start index does nothing...");
    {
        std::atomic<size_t> count = 0;
        const std::pair<int64, int64> indices = random_pair(-range, range);
        dual_println("Range: ", indices.second, " to ", indices.first);
        pool.detach_blocks(indices.second, indices.first,
            [&count](const int64, const int64)
            {
                ++count;
            });
        pool.wait();
        check(count == 0);
    }
    dual_println("Verifying that submit_blocks() with end index smaller than the start index does nothing...");
    {
        std::atomic<size_t> count = 0;
        const std::pair<int64, int64> indices = random_pair(-range, range);
        dual_println("Range: ", indices.second, " to ", indices.first);
        pool.submit_blocks(indices.second, indices.first,
                [&count](const int64, const int64)
                {
                    ++count;
                })
            .wait();
        check(count == 0);
    }
    dual_println("Trying detach_blocks() with a number of tasks larger than the number of indices:");
    {
        const int64 start = random(-range, range);
        check(check_blocks_no_return(pool, start, start + random<BS::concurrency_t>(0, pool.get_thread_count() * 2), random<BS::concurrency_t>(pool.get_thread_count() * 2, pool.get_thread_count() * 4), "detach_blocks()"));
    }
    dual_println("Trying submit_blocks() with a number of tasks larger than the number of indices:");
    {
        const int64 start = random(-range, range);
        check(check_blocks_no_return(pool, start, start + random<BS::concurrency_t>(0, pool.get_thread_count() * 2), random<BS::concurrency_t>(pool.get_thread_count() * 2, pool.get_thread_count() * 4), "submit_blocks()"));
    }
}

// ============================================
// Functions to verify sequence parallelization
// ============================================

/**
 * @brief Check that detach_sequence() or submit_sequence() work for a specific range of indices, with no return value.
 *
 * @param pool The thread pool to check.
 * @param random_start The first index in the sequence.
 * @param random_end The last index in the sequence plus 1.
 * @param which_func A string naming the function to check.
 * @return `true` if the check succeeded, `false` otherwise.
 */
bool check_sequence_no_return(BS::thread_pool& pool, const int64 random_start, const int64 random_end, const std::string_view which_func)
{
    dual_println("Verifying that ", which_func, " from ", random_start, " to ", random_end, " modifies all indices exactly once...");
    const size_t num_indices = static_cast<size_t>(random_end - random_start);
    std::vector<std::atomic<int64>> flags(num_indices);
    std::atomic<bool> indices_out_of_range = false;
    const std::function<void(int64)> sequence = [&flags, random_start, random_end, &indices_out_of_range](const int64 index)
    {
        if (index < random_start || index > random_end)
            indices_out_of_range = true;
        else
            ++flags[static_cast<size_t>(index - random_start)];
    };
    if (which_func == "detach_sequence()")
    {
        pool.detach_sequence(random_start, random_end, sequence);
        pool.wait();
    }
    else
    {
        pool.submit_sequence(random_start, random_end, sequence).wait();
    }
    if (indices_out_of_range)
    {
        dual_println("Error: Sequence indices out of range!");
        return false;
    }
    for (size_t i = 0; i < num_indices; ++i)
    {
        if (flags[i] != 1)
            return false;
    }
    return true;
}

/**
 * @brief Check that submit_sequence() works for a specific range of indices, with a return value.
 *
 * @param pool The thread pool to check.
 * @param random_start The first index in the sequence.
 * @param random_end The last index in the sequence plus 1.
 */
void check_sequence_return(BS::thread_pool& pool, const int64 random_start, const int64 random_end)
{
    dual_println("Verifying that submit_sequence() from ", random_start, " to ", random_end, " correctly sums all squares of indices...");
    const std::function<int64(int64)> sequence = [](const int64 index)
    {
        return index * index;
    };
    const std::vector<int64> sums_vector = pool.submit_sequence(random_start, random_end, sequence).get();
    int64 sum = 0;
    for (const int64 partial_sum : sums_vector)
        sum += partial_sum;
    int64 correct_sum = 0;
    for (int64 i = random_start; i < random_end; i++)
        correct_sum += i * i;
    check(correct_sum, sum);
}

/**
 * @brief Check that detach_sequence() and submit_sequence() work using several different random values for the range of indices.
 */
void check_sequence()
{
    constexpr int64 range = 1000;
    constexpr size_t repeats = 10;
    BS::thread_pool pool;
    for (size_t i = 0; i < repeats; ++i)
    {
        const std::pair<int64, int64> indices = random_pair(-range, range);
        check(check_sequence_no_return(pool, indices.first, indices.second, "detach_sequence()"));
    }
    for (size_t i = 0; i < repeats; ++i)
    {
        const std::pair<int64, int64> indices = random_pair(-range, range);
        check(check_sequence_no_return(pool, indices.first, indices.second, "submit_sequence()"));
    }
    for (size_t i = 0; i < repeats; ++i)
    {
        const std::pair<int64, int64> indices = random_pair(-range, range);
        check_sequence_return(pool, indices.first, indices.second);
    }
    dual_println("Verifying that detach_sequence() with identical start and end indices does nothing...");
    {
        std::atomic<size_t> count = 0;
        const int64 index = random(-range, range);
        dual_println("Range: ", index, " to ", index);
        pool.detach_sequence(index, index,
            [&count](const int64)
            {
                ++count;
            });
        pool.wait();
        check(count == 0);
    }
    dual_println("Verifying that submit_sequence() with identical start and end indices does nothing...");
    {
        std::atomic<size_t> count = 0;
        const int64 index = random(-range, range);
        dual_println("Range: ", index, " to ", index);
        pool.submit_sequence(index, index,
                [&count](const int64)
                {
                    ++count;
                })
            .wait();
        check(count == 0);
    }
    dual_println("Verifying that detach_sequence() with end index smaller than the start index does nothing...");
    {
        std::atomic<size_t> count = 0;
        const std::pair<int64, int64> indices = random_pair(-range, range);
        dual_println("Range: ", indices.second, " to ", indices.first);
        pool.detach_sequence(indices.second, indices.first,
            [&count](const int64)
            {
                ++count;
            });
        pool.wait();
        check(count == 0);
    }
    dual_println("Verifying that submit_sequence() with end index smaller than the start index does nothing...");
    {
        std::atomic<size_t> count = 0;
        const std::pair<int64, int64> indices = random_pair(-range, range);
        dual_println("Range: ", indices.second, " to ", indices.first);
        pool.submit_sequence(indices.second, indices.first,
                [&count](const int64)
                {
                    ++count;
                })
            .wait();
        check(count == 0);
    }
}

// ===============================================
// Functions to verify task monitoring and control
// ===============================================

/**
 * @brief Check that task monitoring works.
 */
void check_task_monitoring()
{
    constexpr std::chrono::milliseconds sleep_time(300);
    const size_t num_threads = std::min<BS::concurrency_t>(std::thread::hardware_concurrency(), 4);
    dual_println("Creating pool with ", num_threads, " threads.");
    BS::thread_pool pool(static_cast<BS::concurrency_t>(num_threads));
    dual_println("Submitting ", num_threads * 3, " tasks.");
    std::vector<BS::signaller> signals(num_threads * 3);
    for (size_t i = 0; i < num_threads * 3; ++i)
        pool.detach_task(
            [i, &signal = signals[i]]
            {
                signal.wait();
                dual_println("Task ", i, " released.");
            }
#ifdef BS_THREAD_POOL_ENABLE_PRIORITY
            ,
            static_cast<BS::priority_t>(-static_cast<BS::priority_t>(i))
#endif
        );
    std::this_thread::sleep_for(sleep_time);
    dual_println("After submission, should have: ", num_threads * 3, " tasks total, ", num_threads, " tasks running, ", num_threads * 2, " tasks queued...");
    dual_print("Result: ", pool.get_tasks_total(), " tasks total, ", pool.get_tasks_running(), " tasks running, ", pool.get_tasks_queued(), " tasks queued ");
    check(pool.get_tasks_total() == num_threads * 3 && pool.get_tasks_running() == num_threads && pool.get_tasks_queued() == num_threads * 2);
    for (size_t i = 0; i < num_threads; ++i)
        signals[i].ready();
    std::this_thread::sleep_for(sleep_time);
    dual_println("After releasing ", num_threads, " tasks, should have: ", num_threads * 2, " tasks total, ", num_threads, " tasks running, ", num_threads, " tasks queued...");
    dual_print("Result: ", pool.get_tasks_total(), " tasks total, ", pool.get_tasks_running(), " tasks running, ", pool.get_tasks_queued(), " tasks queued ");
    check(pool.get_tasks_total() == num_threads * 2 && pool.get_tasks_running() == num_threads && pool.get_tasks_queued() == num_threads);
    for (size_t i = num_threads; i < num_threads * 2; ++i)
        signals[i].ready();
    std::this_thread::sleep_for(sleep_time);
    dual_println("After releasing ", num_threads, " more tasks, should have: ", num_threads, " tasks total, ", num_threads, " tasks running, ", 0, " tasks queued...");
    dual_print("Result: ", pool.get_tasks_total(), " tasks total, ", pool.get_tasks_running(), " tasks running, ", pool.get_tasks_queued(), " tasks queued ");
    check(pool.get_tasks_total() == num_threads && pool.get_tasks_running() == num_threads && pool.get_tasks_queued() == 0);
    for (size_t i = num_threads * 2; i < num_threads * 3; ++i)
        signals[i].ready();
    std::this_thread::sleep_for(sleep_time);
    dual_println("After releasing the final ", num_threads, " tasks, should have: ", 0, " tasks total, ", 0, " tasks running, ", 0, " tasks queued...");
    dual_print("Result: ", pool.get_tasks_total(), " tasks total, ", pool.get_tasks_running(), " tasks running, ", pool.get_tasks_queued(), " tasks queued ");
    check(pool.get_tasks_total() == 0 && pool.get_tasks_running() == 0 && pool.get_tasks_queued() == 0);
}

#ifdef BS_THREAD_POOL_ENABLE_PAUSE
/**
 * @brief Check that pausing works.
 */
void check_pausing()
{
    constexpr std::chrono::milliseconds sleep_time(200);
    BS::thread_pool pool;
    dual_println("Checking that the pool correctly reports that it is not paused after construction...");
    check(!pool.is_paused());
    dual_println("Pausing pool.");
    pool.pause();
    dual_println("Checking that the pool correctly reports that it is paused...");
    check(pool.is_paused());
    dual_println("Submitting task and waiting.");
    std::atomic<bool> flag = false;
    pool.detach_task(
        [&flag]
        {
            flag = true;
            dual_println("Task executed.");
        });
    std::this_thread::sleep_for(sleep_time);
    dual_println("Verifying that the task has not been executed...");
    check(!flag);
    dual_println("Unpausing pool and waiting.");
    pool.unpause();
    std::this_thread::sleep_for(sleep_time);
    dual_println("Verifying that the task has been executed...");
    check(flag);
    dual_println("Checking that the pool correctly reports that it is not paused...");
    check(!pool.is_paused());
}
#endif

/**
 * @brief Check that purge() works.
 */
void check_purge()
{
    constexpr std::chrono::milliseconds long_sleep_time(200);
    constexpr std::chrono::milliseconds short_sleep_time(100);
    constexpr size_t num_tasks = 10;
    BS::thread_pool pool(1);
    dual_println("Submitting ", num_tasks, " tasks to the pool.");
    std::vector<std::atomic<bool>> flags(num_tasks);
    for (size_t i = 0; i < num_tasks; ++i)
        pool.detach_task(
            [&flags, i, long_sleep_time]
            {
                std::this_thread::sleep_for(long_sleep_time);
                dual_println("Task ", i, " done.");
                flags[i] = true;
            });
    std::this_thread::sleep_for(short_sleep_time);
    dual_println("Purging the pool and waiting for tasks...");
    pool.purge();
    pool.wait();
    dual_println("Checking that only the first task was executed...");
    flags[0] = !flags[0];
    check(no_flags_set(flags));
}

// ======================================
// Functions to verify exception handling
// ======================================

/**
 * @brief An exception class to be thrown when testing exception handling.
 */
struct test_exception : public std::runtime_error
{
    test_exception() : std::runtime_error("Exception thrown!"){};
};

/**
 * @brief A function that throws a `test_exception`.
 */
void throws()
{
    dual_println("Throwing exception...");
    throw test_exception();
};

/**
 * @brief Check that exceptions are forwarded correctly by submit_task().
 */
void check_exceptions_submit()
{
    BS::thread_pool pool;
    dual_println("Checking that exceptions are forwarded correctly by submit_task()...");
    bool caught = false;
    std::future<void> future = pool.submit_task(throws);
    try
    {
        future.get();
    }
    catch (const test_exception&)
    {
        caught = true;
    }
    check(caught);
}

/**
 * @brief Check that exceptions are forwarded correctly by `BS::multi_future`.
 */
void check_exceptions_multi_future()
{
    BS::thread_pool pool;
    dual_println("Checking that exceptions are forwarded correctly by BS::multi_future...");
    bool caught = false;
    BS::multi_future<void> future;
    future.push_back(pool.submit_task(throws));
    future.push_back(pool.submit_task(throws));
    try
    {
        future.get();
    }
    catch (const test_exception&)
    {
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
 * @param pool The thread pool to check.
 * @param vector_size The size of the vectors.
 * @param num_tasks The number of tasks to split the calculation into.
 * @return `true` if the single-threaded and multithreaded results are equal, `false` otherwise.
 */
bool check_vector_of_size(BS::thread_pool& pool, const size_t vector_size, const BS::concurrency_t num_tasks)
{
    constexpr int64 value_range = 1000000;
    std::vector<int64> vector_1(vector_size);
    std::vector<int64> vector_2(vector_size);
    for (size_t i = 0; i < vector_size; ++i)
    {
        vector_1[i] = random(-value_range, value_range);
        vector_2[i] = random(-value_range, value_range);
    }
    dual_println("Adding two vectors with ", vector_size, " elements using ", num_tasks, " tasks...");
    std::vector<int64> sum_single(vector_size);
    for (size_t i = 0; i < vector_size; ++i)
        sum_single[i] = vector_1[i] + vector_2[i];
    std::vector<int64> sum_multi(vector_size);
    pool.submit_blocks<size_t>(
            0, vector_size,
            [&sum_multi, &vector_1, &vector_2](const size_t start, const size_t end)
            {
                for (size_t i = start; i < end; ++i)
                    sum_multi[i] = vector_1[i] + vector_2[i];
            },
            num_tasks)
        .wait();
    for (size_t i = 0; i < vector_size; ++i)
    {
        if (sum_single[i] != sum_multi[i])
            return false;
    }
    return true;
}

/**
 * @brief Check that parallelized vector operations work as expected.
 */
void check_vectors()
{
    constexpr size_t size_range = 1000000;
    constexpr size_t repeats = 10;
    BS::thread_pool pool;
    for (size_t i = 0; i < repeats; ++i)
        check(check_vector_of_size(pool, random<size_t>(0, size_range), random<BS::concurrency_t>(1, pool.get_thread_count())));
}

// =================================
// Functions to verify task priority
// =================================

#ifdef BS_THREAD_POOL_ENABLE_PRIORITY
/**
 * @brief Check that task priority works as expected with all task submission methods.
 */
void check_priority()
{
    constexpr std::chrono::milliseconds sleep_time(200);
    constexpr size_t num_tasks = 10;
    // Set the pool to have only 1 thread, so it can only run 1 task at a time. This will ensure the tasks will be executed in priority order. Also initialize the pool with a task that waits until released, to simulate pausing in case it is disabled.
    BS::signaller signal;
    BS::thread_pool pool(1,
        [&signal]
        {
            signal.wait();
        });

    // Create a shuffled lists of priorities.
    std::vector<BS::priority_t> priorities;
    for (size_t i = 0; i < num_tasks - 1; ++i)
        priorities.push_back((i % 2 == 0) ? random<BS::priority_t>(0, BS::pr::highest) : random<BS::priority_t>(BS::pr::lowest, 0));
    priorities.push_back(0);
    std::shuffle(priorities.begin(), priorities.end(), std::mt19937_64(std::random_device()()));

    // Submit tasks using various methods in random priority order.
    std::vector<BS::priority_t> execution_order;
    std::mutex exec_mutex;
    std::function<void(BS::priority_t)> execute_task_priority = [&execution_order, &exec_mutex](const BS::priority_t priority)
    {
        const std::scoped_lock lock(exec_mutex);
        dual_println("Task with priority ", priority, " executed.");
        execution_order.push_back(priority);
    };
    const std::vector<std::string_view> functions = {"detach_task", "submit_task", "detach_sequence", "submit_sequence", "detach_loop", "submit_loop", "detach_blocks", "submit_blocks"};
    for (const BS::priority_t priority : priorities)
    {
        const std::string_view func = functions[random<size_t>(0, functions.size() - 1)];
        dual_println("Launching ", func, "() with priority ", priority, "...");
        if (func == "detach_task")
            pool.detach_task(
                [priority, &execute_task_priority]
                {
                    execute_task_priority(priority);
                },
                priority);
        else if (func == "submit_task")
            std::ignore = pool.submit_task(
                [priority, &execute_task_priority]
                {
                    execute_task_priority(priority);
                },
                priority);
        else if (func == "detach_sequence")
            pool.detach_sequence(
                0, 1,
                [priority, &execute_task_priority](int64)
                {
                    execute_task_priority(priority);
                },
                priority);
        else if (func == "submit_sequence")
            std::ignore = pool.submit_sequence(
                0, 1,
                [priority, &execute_task_priority](int64)
                {
                    execute_task_priority(priority);
                },
                priority);
        else if (func == "detach_loop")
            pool.detach_loop(
                0, 1,
                [priority, &execute_task_priority](int64)
                {
                    execute_task_priority(priority);
                },
                0, priority);
        else if (func == "submit_loop")
            std::ignore = pool.submit_loop(
                0, 1,
                [priority, &execute_task_priority](int64)
                {
                    execute_task_priority(priority);
                },
                0, priority);
        else if (func == "detach_blocks")
            pool.detach_blocks(
                0, 1,
                [priority, &execute_task_priority](int64, int64)
                {
                    execute_task_priority(priority);
                },
                0, priority);
        else if (func == "submit_blocks")
            std::ignore = pool.submit_blocks(
                0, 1,
                [priority, &execute_task_priority](int64, int64)
                {
                    execute_task_priority(priority);
                },
                0, priority);
    }

    // Release the waiting task so the tasks can be executed, then check that they were executed in the correct order.
    dual_println("Checking execution order...");
    std::this_thread::sleep_for(sleep_time);
    signal.ready();
    pool.wait();
    std::sort(priorities.rbegin(), priorities.rend());
    check(execution_order == priorities);
}
#endif

// =========================================================
// Functions to verify thread initialization and this_thread
// =========================================================

/**
 * @brief Check that thread initialization functions and get_index() work.
 */
void check_init()
{
    dual_println("Comparing thread indices with a function passed to the constructor...");
    std::vector<std::atomic<size_t>> thread_indices(std::thread::hardware_concurrency());
    BS::thread_pool pool(
        [&thread_indices]
        {
            BS::this_thread::optional_index idx = BS::this_thread::get_index();
            if (idx.has_value())
                thread_indices[idx.value()] = idx.value();
            else
                check(false);
        });
    pool.wait();
    bool correct = true;
    for (size_t i = 0; i < thread_indices.size(); ++i)
    {
        if (thread_indices[i] != i)
        {
            correct = false;
            break;
        }
    }
    check(correct);
    dual_println("Comparing thread indices with a function passed to reset()...");
    pool.reset(
        [&thread_indices]
        {
            BS::this_thread::optional_index idx = BS::this_thread::get_index();
            if (idx.has_value())
                thread_indices[idx.value()] = std::thread::hardware_concurrency() - idx.value();
            else
                check(false);
        });
    pool.wait();
    correct = true;
    for (size_t i = 0; i < thread_indices.size(); ++i)
    {
        if (thread_indices[i] != std::thread::hardware_concurrency() - i)
        {
            correct = false;
            break;
        }
    }
    check(correct);
    {
        dual_println("Verifying that the index of the main thread has no value...");
        const BS::this_thread::optional_index idx = BS::this_thread::get_index();
        check(!idx.has_value());
    }
    {
        dual_println("Verifying that the index of an independent thread has no value...");
        std::thread test_thread(
            []
            {
                const BS::this_thread::optional_index idx = BS::this_thread::get_index();
                check(!idx.has_value());
            });
        test_thread.join();
    }
}

/**
 * @brief Check that get_pool() works.
 */
void check_get_pool()
{
    dual_println("Checking that all threads report the correct pool...");
    std::vector<std::atomic<BS::thread_pool*>> thread_pool_ptrs1(std::thread::hardware_concurrency());
    std::vector<std::atomic<BS::thread_pool*>> thread_pool_ptrs2(std::thread::hardware_concurrency());
    auto store_pointers = [](std::vector<std::atomic<BS::thread_pool*>>& ptrs)
    {
        BS::this_thread::optional_pool ptr = BS::this_thread::get_pool();
        if (ptr.has_value())
            ptrs[BS::this_thread::get_index().value()] = ptr.value();
        else
            check(false);
    };
    BS::thread_pool pool1(
        [&thread_pool_ptrs1, &store_pointers]
        {
            store_pointers(thread_pool_ptrs1);
        });
    BS::thread_pool pool2(
        [&thread_pool_ptrs2, &store_pointers]
        {
            store_pointers(thread_pool_ptrs2);
        });
    pool1.wait();
    pool2.wait();
    auto check_pointers = [](const std::vector<std::atomic<BS::thread_pool*>>& ptrs, const BS::thread_pool& pool)
    {
        bool correct = true;
        for (const std::atomic<BS::thread_pool*>& ptr : ptrs)
        {
            if (ptr != &pool)
            {
                correct = false;
                break;
            }
        }
        check(correct);
    };
    check_pointers(thread_pool_ptrs1, pool1);
    check_pointers(thread_pool_ptrs2, pool2);
    {
        dual_println("Verifying that the pool pointer of the main thread has no value...");
        const BS::this_thread::optional_pool ptr = BS::this_thread::get_pool();
        check(!ptr.has_value());
    }
    {
        dual_println("Verifying that the pool pointer of an independent thread has no value...");
        std::thread test_thread(
            []
            {
                const BS::this_thread::optional_pool ptr = BS::this_thread::get_pool();
                check(!ptr.has_value());
            });
        test_thread.join();
    }
}

// ================================
// Functions to check for deadlocks
// ================================

// An auxiliary thread pool used by check_deadlock(). It's a global variable so that the program will not get stuck upon destruction of this pool if a deadlock actually occurs.
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
    constexpr std::chrono::milliseconds sleep_time(200);
    constexpr size_t tries = 10000;
    size_t try_n = 0;
    check_deadlock_pool.detach_task(
        [&try_n, &task]
        {
            do
                task();
            while (++try_n < tries);
        });
    bool passed = false;
    while (true)
    {
        const size_t old_try_n = try_n;
        check_deadlock_pool.wait_for(sleep_time);
        if (try_n == tries)
        {
            dual_println("Successfully finished all tries!");
            passed = true;
            break;
        }
        if (try_n == old_try_n)
        {
            dual_println("Error: deadlock detected!");
            passed = false;
            break;
        }
        dual_println("Finished ", try_n, " tries out of ", tries, "...");
    }
    check(passed);
}

// ==========================
// Functions for benchmarking
// ==========================

/**
 * @brief Print the timing of a specific test.
 *
 * @param num_tasks The number of tasks.
 * @param mean_sd An `std::pair` containing the mean as the first member and standard deviation as the second member.
 */
void print_timing(const BS::concurrency_t num_tasks, const std::pair<double, double>& mean_sd)
{
    constexpr int width_tasks = 4;
    constexpr int width_mean = 6;
    constexpr int width_sd = 4;
    if (num_tasks == 0)
        dual_print("Single-threaded");
    else
        dual_print("With ", std::setw(width_tasks), num_tasks, " task", (num_tasks > 1) ? "s" : "");
    dual_println(", mean execution time was ", std::setw(width_mean), mean_sd.first, " ms with standard deviation ", std::setw(width_sd), mean_sd.second, " ms.");
}

/**
 * @brief Find the minimum element in a vector.
 *
 * @tparam T The type of elements in the vector.
 * @param vec The vector.
 * @return The index of the smallest element in the vector.
 */
template <typename T>
size_t min_element(const std::vector<T>& vec)
{
    size_t smallest = 0;
    for (size_t i = 1; i < vec.size(); ++i)
    {
        if (vec[i] < vec[smallest])
            smallest = i;
    }
    return smallest;
}

/**
 * @brief Calculate and print the speedup obtained by multithreading.
 *
 * @param timings A vector of the timings corresponding to different numbers of tasks.
 */
void print_speedup(const std::vector<double>& timings, const std::vector<BS::concurrency_t>& try_tasks)
{
    const size_t min_el = min_element(timings);
    const double max_speedup = std::round((timings[0] / timings[min_el]) * 10) / 10;
    const BS::concurrency_t num_tasks = try_tasks[min_el];
    dual_println("Maximum speedup obtained by multithreading vs. single-threading: ", max_speedup, "x, using ", num_tasks, " tasks.");
}

/**
 * @brief Calculate the mean and standard deviation of a set of integers.
 *
 * @param timings The integers.
 * @return An `std::pair` containing the mean as the first member and standard deviation as the second member.
 */
std::pair<double, double> analyze(const std::vector<std::chrono::milliseconds::rep>& timings)
{
    double mean = 0;
    for (size_t i = 0; i < timings.size(); ++i)
        mean += static_cast<double>(timings[i]) / static_cast<double>(timings.size());
    double variance = 0;
    for (size_t i = 0; i < timings.size(); ++i)
        variance += (static_cast<double>(timings[i]) - mean) * (static_cast<double>(timings[i]) - mean) / static_cast<double>(timings.size());
    return {mean, std::sqrt(variance)};
}

/**
 * @brief A function to generate vector elements. Chosen arbitrarily to simulate a typical numerical calculation.
 *
 * @param idx The element index.
 * @return The value of the element.
 */
double generate_element(const size_t idx)
{
    return std::log(std::sqrt(std::exp(std::cos(idx))));
}

/**
 * @brief Benchmark multithreaded performance.
 */
void check_performance()
{
    BS::thread_pool pool;

    // Set the formatting of floating point numbers.
    dual_print(std::fixed, std::setprecision(1));

    // Initialize a timer object to measure execution time.
    BS::timer tmr;

    // Store the number of available hardware threads for easy access.
    const BS::concurrency_t thread_count = pool.get_thread_count();
    dual_println("Using ", thread_count, " threads.");

    // The target execution time, in milliseconds, of the multithreaded test with the number of blocks equal to the number of threads. The total time spent on that test will be approximately equal to repeat * target_ms.
    constexpr std::chrono::milliseconds::rep target_ms = 50;

    // Test how many vectors we need to generate, and of what size, to roughly achieve the target execution time.
    dual_println("Determining the number of elements to generate in order to achieve an approximate mean execution time of ", target_ms, " ms with ", thread_count, " tasks...");
    size_t vector_size = thread_count;
    std::vector<double> vector;
    const std::function<void(size_t, size_t)> loop = [&vector](const size_t start, const size_t end)
    {
        for (size_t i = start; i < end; ++i)
            vector[i] = generate_element(i);
    };
    do
    {
        vector_size *= 2;
        vector = std::vector<double>(vector_size);
        tmr.start();
        pool.detach_blocks<size_t>(0, vector_size, loop);
        pool.wait();
        tmr.stop();
    } while (tmr.ms() < target_ms);
    vector_size = thread_count * static_cast<size_t>(std::llround(static_cast<double>(vector_size) * static_cast<double>(target_ms) / static_cast<double>(tmr.ms()) / thread_count));

    // Define vectors to store statistics.
    std::vector<double> different_n_timings;
    std::vector<std::chrono::milliseconds::rep> same_n_timings;

    // The maximum number of overall tests.
    constexpr size_t max_tests = 20;

    // The maximum number, in milliseconds, that a single test can last.
    constexpr std::chrono::milliseconds::rep max_time_ms = 5000;

    // The minimum number of repeats for each test. At least this many repeats will be performed even if `max_time_ms` is exceeded.
    constexpr size_t min_repeats = 5;

    // The maximum number of times to repeat each run of the test in order to collect reliable statistics.
    constexpr size_t max_repeats = 30;
    dual_println("Each test will be repeated up to ", max_repeats, " times to collect reliable statistics.");

    // Perform the test.
    std::vector<BS::concurrency_t> try_tasks;
    BS::concurrency_t num_tasks = 0;
    double last_timing = std::numeric_limits<double>::max();
    dual_println("Generating ", vector_size, " elements:");
    while (try_tasks.size() <= max_tests)
    {
        try_tasks.push_back(num_tasks);
        dual_print('[');
        std::chrono::milliseconds::rep total_time = 0;
        for (size_t i = 0; i < max_repeats; ++i)
        {
            vector = std::vector<double>(vector_size);
            tmr.start();
            if (num_tasks > 0)
            {
                pool.detach_blocks<size_t>(0, vector_size, loop, num_tasks);
                pool.wait();
            }
            else
            {
                loop(0, vector_size);
            }
            tmr.stop();
            same_n_timings.push_back(tmr.ms());
            dual_print('.');
            total_time += tmr.ms();
            if (total_time > max_time_ms && same_n_timings.size() > min_repeats)
                break;
        }
        dual_println(']');
        const std::pair<double, double> mean_sd = analyze(same_n_timings);
        same_n_timings.clear();
        print_timing(num_tasks, mean_sd);
        different_n_timings.push_back(mean_sd.first);
        if (num_tasks == 0)
        {
            num_tasks = 2;
        }
        else
        {
            if ((num_tasks > thread_count) && (mean_sd.first > last_timing))
                break;
            last_timing = mean_sd.first;
            num_tasks *= 2;
        }
    }
    print_speedup(different_n_timings, try_tasks);
}

// ===============================================
// Functions related to the command line interface
// ===============================================

/**
 * @brief Show basic information about the program.
 */
void show_intro()
{
    dual_println("BS::thread_pool: a fast, lightweight, and easy-to-use C++17 thread pool library");
    dual_println("(c) 2023 Barak Shoshany (baraksh@gmail.com) (http://baraksh.com)");
    dual_println("GitHub: https://github.com/bshoshany/thread-pool");
    dual_println();

    dual_println("Thread pool library version is ", BS_THREAD_POOL_VERSION_MAJOR, '.', BS_THREAD_POOL_VERSION_MINOR, '.', BS_THREAD_POOL_VERSION_PATCH, '.');
    dual_println("Thread pool utilities library version is ", BS_THREAD_POOL_UTILS_VERSION_MAJOR, '.', BS_THREAD_POOL_UTILS_VERSION_MINOR, '.', BS_THREAD_POOL_UTILS_VERSION_PATCH, '.');
    dual_println("Hardware concurrency is ", std::thread::hardware_concurrency(), '.');
    dual_println();

    dual_print("Native handles are ");
#ifdef BS_THREAD_POOL_ENABLE_NATIVE_HANDLES
    dual_println("enabled.");
#else
    dual_println("disabled.");
#endif
    dual_print("Pausing is ");
#ifdef BS_THREAD_POOL_ENABLE_PAUSE
    dual_println("enabled.");
#else
    dual_println("disabled.");
#endif
    dual_print("Priority is ");
#ifdef BS_THREAD_POOL_ENABLE_PRIORITY
    dual_println("enabled.");
#else
    dual_println("disabled.");
#endif
    dual_print("Wait deadlock checks are ");
#ifdef BS_THREAD_POOL_ENABLE_WAIT_DEADLOCK_CHECK
    dual_println("enabled.");
#else
    dual_println("disabled.");
#endif

    dual_println();

    dual_println("Detected OS: ", detect_os(), '.');
    dual_println("Detected compiler: ", detect_compiler(), '.');
    dual_println();

    dual_println("Important: Please do not run any other applications, especially multithreaded applications, in parallel with this test!");
}

/**
 * @brief Show the available command line options.
 */
void show_help()
{
    dual_println();
    dual_println("Available options:");
    dual_println("    help          Show this help message and exit.");
    dual_println("    log           Create a log file.");
    dual_println("    tests         Perform standard tests.");
    dual_println("    deadlock      Perform long deadlock tests.");
    dual_println("    benchmarks    Perform benchmarks.");
    dual_println("If no options are entered, the default is:");
    dual_println("    ", enable_log ? "log " : "", enable_tests ? "tests " : "", enable_long_deadlock_tests ? "deadlock " : "", enable_benchmarks ? "benchmarks " : "");
}

/**
 * @brief A class to parse command line arguments.
 */
class [[nodiscard]] arg_parser
{
public:
    /**
     * @brief Convert the command line arguments passed to the `main()` function into an `std::vector`.
     *
     * @param argc The number of arguments.
     * @param argv An array containing the arguments.
     */
    arg_parser(int argc, char* argv[]) : args(argv, argv + argc){};

    /**
     * @brief Check if a specific command line argument has been passed to the program.
     *
     * @param arg The argument to check for.
     * @return `true` if the argument exists, `false` otherwise.
     */
    [[nodiscard]] bool exists(const std::string_view arg) const
    {
        for (const std::string_view str : args)
        {
            if (str == arg)
                return true;
        }
        return false;
    }

    /**
     * @brief Access a command line argument at a specific index.
     *
     * @param index The index to access.
     * @return The argument.
     */
    [[nodiscard]] const std::string_view operator[](const size_t index) const
    {
        return args[index];
    }

    /**
     * @brief Get the number of command line arguments.
     *
     * @return The number of arguments.
     */
    [[nodiscard]] size_t size() const
    {
        return args.size();
    }

private:
    /**
     * @brief A vector containing string views of the command line arguments.
     */
    std::vector<std::string_view> args;
}; // class arg_parser

// =================
// The main function
// =================

int main(int argc, char* argv[])
{
    // Parse the command line arguments.
    const arg_parser args(argc, argv);
    if (args.size() > 1)
    {
        if (args.exists("help"))
        {
            show_intro();
            show_help();
            std::exit(0);
        }
        enable_tests = args.exists("tests");
        enable_long_deadlock_tests = args.exists("deadlock");
        enable_benchmarks = args.exists("benchmarks");
        enable_log = args.exists("log");
        if (!enable_tests && !enable_long_deadlock_tests && !enable_benchmarks)
        {
            show_intro();
            dual_println("Error: No tests requested! Aborting.");
            std::exit(0);
        }
    }

    if (enable_log)
    {
        // Extract the name of the executable file, or use a default value if it is not found.
        const size_t last_slash = args[0].find_last_of("/\\") + 1;
        std::string exe_file(args[0].substr(last_slash, args[0].find('.', last_slash) - last_slash));
        if (exe_file.empty())
            exe_file = "BS_thread_pool_test";
        // Create a log file using the name of the executable, followed by the current date and time.
        const std::string log_filename = exe_file + "-" + get_time() + ".log";
        log_file.open(log_filename);
        if (log_file.is_open())
        {
            dual_println("Generating log file: ", log_filename, ".\n");
        }
        else
        {
            enable_log = false;
            dual_println("Could not create log file: ", log_filename, ". Proceeding without it.\n");
        }
    }

    show_intro();

    if (enable_tests)
    {
        print_header("Checking the constructor:");
        check_constructor();

        print_header("Checking reset():");
        check_reset();

        print_header("Checking detach_task() and submit_task():");
        check_task("detach_task()");
        check_task("submit_task()");

        print_header("Checking submission of member functions as tasks:");
        check_member_function();
        check_member_function_within_object();

        print_header("Checking wait(), wait_for(), and wait_until():");
        check_wait();
        check_wait_blocks();
        check_wait_for();
        check_wait_until();
        check_wait_multiple_deadlock();
#ifdef BS_THREAD_POOL_ENABLE_WAIT_DEADLOCK_CHECK
        check_wait_self_deadlock();
#else
        print_header("NOTE: Wait deadlock checks disabled, skipping test.");
#endif

        print_header("Checking detach_loop() and submit_loop():");
        check_loop();

        print_header("Checking detach_blocks() and submit_blocks():");
        check_blocks();

        print_header("Checking detach_sequence() and submit_sequence():");
        check_sequence();

        print_header("Checking task monitoring:");
        check_task_monitoring();

#ifdef BS_THREAD_POOL_ENABLE_PAUSE
        print_header("Checking pausing:");
        check_pausing();
#else
        print_header("NOTE: Pausing disabled, skipping test.");
#endif

        print_header("Checking purge():");
        check_purge();

        print_header("Checking exception handling:");
        check_exceptions_submit();
        check_exceptions_multi_future();

        print_header("Checking parallelized vector operations:");
        check_vectors();

#ifdef BS_THREAD_POOL_ENABLE_PRIORITY
        print_header("Checking task priority:");
        check_priority();
#else
        print_header("NOTE: Task priority disabled, skipping test.");
#endif

        print_header("Checking thread initialization functions and get_index():");
        check_init();

        print_header("Checking get_pool():");
        check_get_pool();
    }

    if (enable_long_deadlock_tests)
    {
        print_header("Checking for deadlocks:");
        dual_println("Checking for destruction deadlocks...");
        check_deadlock(
            []
            {
                const BS::thread_pool temp_pool;
            });
        dual_println("Checking for reset deadlocks...");
        BS::thread_pool temp_pool;
        check_deadlock(
            [&temp_pool]
            {
                temp_pool.reset();
            });
    }

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
        std::exit(0);
    }
    else
    {
        print_header("FAILURE: Passed " + std::to_string(tests_succeeded) + " checks, but failed " + std::to_string(tests_failed) + "!", '+');
        dual_println("\nPlease submit a bug report at https://github.com/bshoshany/thread-pool/issues including the exact specifications of your system (OS, CPU, compiler, etc.) and the generated log file.");
#if defined(__APPLE__)
        // macOS does not implement `std::quick_exit` for some reason. `std::exit()` cannot be used here, as it might get stuck if a deadlock occurs.
        std::terminate();
#else
        std::quick_exit(static_cast<int>(tests_failed));
#endif
    }
}
