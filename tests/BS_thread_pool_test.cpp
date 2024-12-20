/**
 * ██████  ███████       ████████ ██   ██ ██████  ███████  █████  ██████          ██████   ██████   ██████  ██
 * ██   ██ ██      ██ ██    ██    ██   ██ ██   ██ ██      ██   ██ ██   ██         ██   ██ ██    ██ ██    ██ ██
 * ██████  ███████          ██    ███████ ██████  █████   ███████ ██   ██         ██████  ██    ██ ██    ██ ██
 * ██   ██      ██ ██ ██    ██    ██   ██ ██   ██ ██      ██   ██ ██   ██         ██      ██    ██ ██    ██ ██
 * ██████  ███████          ██    ██   ██ ██   ██ ███████ ██   ██ ██████  ███████ ██       ██████   ██████  ███████
 *
 * @file BS_thread_pool_test.cpp
 * @author Barak Shoshany (baraksh@gmail.com) (https://baraksh.com/)
 * @version 5.0.0
 * @date 2024-12-19
 * @copyright Copyright (c) 2024 Barak Shoshany. Licensed under the MIT license. If you found this project useful, please consider starring it on GitHub! If you use this library in software of any kind, please provide a link to the GitHub repository https://github.com/bshoshany/thread-pool in the source code and documentation. If you use this library in published research, please cite it as follows: Barak Shoshany, "A C++17 Thread Pool for High-Performance Scientific Computing", doi:10.1016/j.softx.2024.101687, SoftwareX 26 (2024) 101687, arXiv:2105.00613
 *
 * @brief `BS::thread_pool`: a fast, lightweight, modern, and easy-to-use C++17/C++20/C++23 thread pool library. This program tests all aspects of the library, but is not needed in order to use the library.
 */

// We need to include <version> since if we're using `import std` it will not define any feature-test macros, including `__cpp_lib_modules`, which we need to check if `import std` is supported in the first place.
#ifdef __has_include
    #if __has_include(<version>)
        #include <version> // NOLINT(misc-include-cleaner)
    #endif
#endif

// If the macro `BS_THREAD_POOL_IMPORT_STD` is defined, import the C++ Standard Library as a module. Otherwise, include the relevant Standard Library header files. This is currently only officially supported by MSVC with Microsoft STL and LLVM Clang (NOT Apple Clang) with LLVM libc++. It is not supported by GCC with any standard library, or any compiler with GNU libstdc++. We also check that the feature is enabled by checking `__cpp_lib_modules`. However, MSVC defines this macro even in C++20 mode, which is not standards-compliant, so we check that we are in C++23 mode; MSVC currently reports `__cplusplus` as `202004L` for C++23 mode, so we use that value.
#if defined(BS_THREAD_POOL_IMPORT_STD) && defined(__cpp_lib_modules) && (__cplusplus >= 202004L) && (defined(_MSC_VER) || (defined(__clang__) && defined(_LIBCPP_VERSION) && !defined(__apple_build_version__)))
import std;
constexpr bool using_import_std = true;
#else
    #include <algorithm>
    #include <atomic>
    #include <chrono>
    #include <cmath>
    #include <complex>
    #include <cstddef>
    #include <cstdint>
    #include <cstdlib>
    #include <ctime>
    #include <fstream>
    #include <functional>
    #include <future>
    #include <initializer_list>
    #include <iomanip>
    #include <ios>
    #include <iostream>
    #include <limits>
    #include <map>
    #include <memory>
    #include <mutex>
    #include <numeric>
    #include <optional>
    #include <random>
    #include <set>
    #include <sstream>
    #include <string>
    #include <string_view>
    #include <thread>
    #include <tuple>
    #include <type_traits>
    #include <utility>
    #include <vector>

    #ifdef __cpp_exceptions
        #include <exception>
        #include <stdexcept>
    #endif
    #ifdef __cpp_lib_format
        #include <format>
    #endif
constexpr bool using_import_std = false;
#endif

// If the macro `BS_THREAD_POOL_TEST_IMPORT_MODULE` is defined, import the thread pool library as a module. Otherwise, include the header file. We also check that we are in C++20 or later. We can't use `__cpp_modules` to check if modules are supported, because Clang does not define it even in C++20 mode; its support for C++20 modules is only partial, but it does seem to work for this particular library.
#define BS_THREAD_POOL_TEST_VERSION 5, 0, 0
#if defined(BS_THREAD_POOL_TEST_IMPORT_MODULE) && (__cplusplus >= 202002L)
import BS.thread_pool;
static_assert(BS::thread_pool_module, "The flag BS::thread_pool_module is set to false, but the library was imported as a module. Aborting compilation.");
static_assert(BS::thread_pool_version == BS::version(BS_THREAD_POOL_TEST_VERSION), "The versions of BS_thread_pool_test.cpp and the BS.thread_pool module do not match. Aborting compilation.");
#else
    #include "BS_thread_pool.hpp"
static_assert(!BS::thread_pool_module, "The flag BS::thread_pool_module is set to true, but the library was not imported as a module. Aborting compilation.");
static_assert(BS::thread_pool_version == BS::version(BS_THREAD_POOL_TEST_VERSION), "The versions of BS_thread_pool_test.cpp and BS_thread_pool.hpp do not match. Aborting compilation.");
#endif

#ifdef BS_THREAD_POOL_NATIVE_EXTENSIONS
static_assert(BS::thread_pool_native_extensions, "Cannot test the native extensions, as the thread pool module was compiled without enabling them using the macro BS_THREAD_POOL_NATIVE_EXTENSIONS. Aborting compilation.");
#endif

// A global synced stream which prints to the standard output and/or the log file.
BS::synced_stream sync_out;

// ======================
// Functions for printing
// ======================

/**
 * @brief Print a stylized header.
 *
 * @param text The text of the header. Will appear between two lines.
 * @param symbol The symbol to use for the lines. Default is '='.
 */
void print_header(const std::string_view text, const char symbol = '=')
{
    sync_out.println(BS::synced_stream::flush);
    sync_out.println(std::string(text.length(), symbol));
    sync_out.println(text);
    sync_out.println(std::string(text.length(), symbol));
}

// =================================
// Functions for checking conditions
// =================================

/**
 * @brief A struct to keep count of the number of tests that succeeded and failed.
 */
struct test_results
{
    inline static std::size_t tests_succeeded = 0;
    inline static std::size_t tests_failed = 0;
};

/**
 * @brief Check if a condition is met, report the result, and keep count of the total number of successes and failures.
 *
 * @param condition The condition to check.
 */
void check(const bool condition)
{
    if (condition)
    {
        sync_out.println("-> passed.");
        ++test_results::tests_succeeded;
    }
    else
    {
        sync_out.println("-> FAILED!");
        ++test_results::tests_failed;
    }
}

/**
 * @brief Check if the expected result has been obtained, report the result, and keep count of the total number of successes and failures.
 *
 * @param condition The condition to check.
 */
template <typename T1, typename T2>
void check(const T1& expected, const T2& obtained)
{
    sync_out.print("- Expected: ", expected, ", obtained: ", obtained, ' ');
    check(expected == static_cast<T1>(obtained));
}

/**
 * @brief Check if all of the flags in a container are equal to the desired value.
 *
 * @param flags The container.
 * @param value The desired value.
 * @return `true` if all flags are equal to the desired value, or `false` otherwise.
 */
template <typename T, typename V>
bool all_flags_equal(const T& flags, const V& value)
{
    return std::all_of(flags.begin(), flags.end(),
        [&value](const auto& flag)
        {
            return flag == value;
        });
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
    return std::all_of(flags.begin(), flags.end(),
        [](const bool flag)
        {
            return flag;
        });
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
    return std::none_of(flags.begin(), flags.end(),
        [](const bool flag)
        {
            return flag;
        });
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
        second = dist(twister);
    while (second == first);
    if (second < first)
        return {second, first};
    return {first, second};
}

// ========================================
// Functions for detecting various features
// ========================================

/**
 * @brief Make a string out of items of various types.
 *
 * @tparam T The types of the items.
 * @param items The items.
 * @return The generated string.
 */
template <typename... T>
std::string make_string(const T&... items)
{
    std::ostringstream out;
    (out << ... << items);
    return out.str();
}

/**
 * @brief Detect the compiler used to compile this program.
 *
 * @return A string describing the compiler.
 */
std::string detect_compiler()
{
#if defined(__apple_build_version__)
    return make_string("Apple Clang v", __clang_major__, '.', __clang_minor__, '.', __clang_patchlevel__);
#elif defined(__clang__)
    return make_string("Clang v", __clang_major__, '.', __clang_minor__, '.', __clang_patchlevel__);
#elif defined(__GNUC__)
    return make_string("GCC v", __GNUC__, '.', __GNUC_MINOR__, '.', __GNUC_PATCHLEVEL__);
#elif defined(_MSC_FULL_VER)
    std::string msvc_full_ver = make_string(_MSC_FULL_VER);
    return make_string("MSVC v", msvc_full_ver.substr(0, 2), '.', msvc_full_ver.substr(2, 2), '.', msvc_full_ver.substr(4));
#else
    return "Other";
#endif
}

/**
 * @brief Detect the C++ standard used to compile this program.
 *
 * @return A string describing the C++ standard.
 */
std::string detect_cpp_standard()
{
#if __cplusplus == 201703L
    return "C++17";
#elif __cplusplus == 202002L
    return "C++20";
#elif (__cplusplus == 202302L) || (__cplusplus == 202004L) // MSVC currently uses 202004L for /std:c++latest.
    return "C++23";
#else
    return make_string("Other (__cplusplus = ", __cplusplus, ")");
#endif
}

/**
 * @brief Detect the C++ standard library used to compile this program.
 *
 * @return A string describing the C++ standard library.
 */
std::string detect_lib()
{
#if defined(_LIBCPP_VERSION)
    return make_string("LLVM libc++ v", _LIBCPP_VERSION / 10000, '.', (_LIBCPP_VERSION / 100) % 100, '.', _LIBCPP_VERSION % 100); // NOLINT(readability-magic-numbers)
#elif defined(_GLIBCXX_RELEASE)
    std::string out = make_string("GNU libstdc++ v", _GLIBCXX_RELEASE);
    #ifdef __GLIBCXX__
    out += make_string(" (", __GLIBCXX__, ')');
    #endif
    return out;
#elif defined(_MSVC_STL_VERSION)
    std::string out = make_string("Microsoft STL v", _MSVC_STL_VERSION);
    #ifdef _MSVC_STL_UPDATE
    out += make_string(" (", _MSVC_STL_UPDATE, ')');
    #endif
    return out;
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
 * @brief Detect available C++ features and print them out.
 */
void print_features()
{
    constexpr int width = 33;
    sync_out.print(std::left);

    sync_out.print(std::setw(width), "__cpp_concepts:");
#ifdef __cpp_concepts
    sync_out.println(__cpp_concepts);
#else
    sync_out.println("N/A");
#endif

    sync_out.print(std::setw(width), "__cpp_exceptions:");
#ifdef __cpp_exceptions
    sync_out.println(__cpp_exceptions);
#else
    sync_out.println("N/A");
#endif

    sync_out.print(std::setw(width), "__cpp_impl_three_way_comparison:");
#ifdef __cpp_impl_three_way_comparison
    sync_out.println(__cpp_impl_three_way_comparison);
#else
    sync_out.println("N/A");
#endif

    sync_out.print(std::setw(width), "__cpp_lib_format:");
#ifdef __cpp_lib_format
    sync_out.println(__cpp_lib_format);
#else
    sync_out.println("N/A");
#endif

    sync_out.print(std::setw(width), "__cpp_lib_int_pow2:");
#ifdef __cpp_lib_int_pow2
    sync_out.println(__cpp_lib_int_pow2);
#else
    sync_out.println("N/A");
#endif

    sync_out.print(std::setw(width), "__cpp_lib_jthread:");
#ifdef __cpp_lib_jthread
    sync_out.println(__cpp_lib_jthread);
#else
    sync_out.println("N/A");
#endif

    sync_out.print(std::setw(width), "__cpp_lib_modules:");
#ifdef __cpp_lib_modules
    sync_out.println(__cpp_lib_modules);
#else
    sync_out.println("N/A");
#endif

    sync_out.print(std::setw(width), "__cpp_lib_move_only_function:");
#ifdef __cpp_lib_move_only_function
    sync_out.println(__cpp_lib_move_only_function);
#else
    sync_out.println("N/A");
#endif

    sync_out.print(std::setw(width), "__cpp_lib_semaphore:");
#ifdef __cpp_lib_semaphore
    sync_out.println(__cpp_lib_semaphore);
#else
    sync_out.println("N/A");
#endif

    sync_out.print(std::setw(width), "__cpp_modules:");
#ifdef __cpp_modules
    sync_out.println(__cpp_modules);
#else
    sync_out.println("N/A");
#endif

    sync_out.print(std::setw(width), "__has_include(<version>):");
#if __has_include(<version>)
    sync_out.println("true");
#else
    sync_out.println("false");
#endif

    sync_out.println(std::right);
}

// =========================================
// Functions to verify the number of threads
// =========================================

/**
 * @brief Obtain a list of unique thread IDs in the pool. Submits a number of tasks equal to twice the thread count into the pool. Each task stores the ID of the thread running it, and then waits until as many tasks as the thread count are finished. This ensures that each thread in the pool runs at least one task, as the pool gets filled completely.
 *
 * @param pool The thread pool to check.
 */
std::vector<std::thread::id> obtain_unique_threads(BS::thread_pool<>& pool)
{
    const std::size_t num_tasks = pool.get_thread_count() * 2;
    std::vector<std::thread::id> thread_ids(num_tasks);
    std::atomic<std::size_t> total_count = 0;
    BS::counting_semaphore sem(0);
    for (std::thread::id& tid : thread_ids)
    {
        pool.detach_task(
            [&total_count, &tid, &sem, thread_count = pool.get_thread_count(), num_tasks]
            {
                tid = std::this_thread::get_id();
                if (++total_count == thread_count)
                    sem.release(static_cast<std::ptrdiff_t>(num_tasks));
                sem.acquire();
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
    sync_out.println("Checking that the thread pool reports a number of threads equal to the hardware concurrency...");
    check(std::thread::hardware_concurrency(), pool.get_thread_count());
    sync_out.println("Checking that the manually counted number of unique thread IDs is equal to the reported number of threads...");
    const std::vector<std::thread::id> unique_threads = obtain_unique_threads(pool);
    check(pool.get_thread_count(), unique_threads.size());
    sync_out.println("Checking that the unique thread IDs obtained are the same as those reported by get_thread_ids()...");
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
    pool.reset(static_cast<std::size_t>(std::thread::hardware_concurrency()) * 2);
    sync_out.println("Checking that after reset() the thread pool reports a number of threads equal to double the hardware concurrency...");
    check(std::thread::hardware_concurrency() * 2, pool.get_thread_count());
    sync_out.println("Checking that after reset() the manually counted number of unique thread IDs is equal to the reported number of threads...");
    check(pool.get_thread_count(), obtain_unique_threads(pool).size());
    pool.reset(std::thread::hardware_concurrency());
    sync_out.println("Checking that after a second reset() the thread pool reports a number of threads equal to the hardware concurrency...");
    check(std::thread::hardware_concurrency(), pool.get_thread_count());
    sync_out.println("Checking that after a second reset() the manually counted number of unique thread IDs is equal to the reported number of threads...");
    check(pool.get_thread_count(), obtain_unique_threads(pool).size());
}

// =======================================
// Functions to verify submission of tasks
// =======================================

/**
 * @brief A class used to detect when a copy or move constructor has been invoked.
 */
class [[nodiscard]] detect_copy_move
{
public:
    detect_copy_move() = default;

    detect_copy_move(const detect_copy_move& /*other*/) : copied(true) {}

    detect_copy_move(detect_copy_move&& /*other*/) noexcept : moved(true) {}

    detect_copy_move& operator=(const detect_copy_move&) = delete;
    detect_copy_move& operator=(detect_copy_move&&) = delete;
    ~detect_copy_move() = default;

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
    sync_out.println("Checking that ", which_func, " works for a function with no arguments or return value...");
    {
        bool flag = false;
        const auto func = [&flag]
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
    sync_out.println("Checking that ", which_func, " works for a function with one argument and no return value...");
    {
        bool flag = false;
        const auto func = [](bool& flag_)
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
    sync_out.println("Checking that ", which_func, " works for a function with two arguments and no return value...");
    {
        bool flag1 = false;
        bool flag2 = false;
        const auto func = [](bool& flag1_, bool& flag2_)
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
        sync_out.println("Checking that submit_task() works for a function with no arguments and a return value...");
        {
            bool flag = false;
            const auto func = [&flag]
            {
                return (flag = true);
            };
            std::future<bool> flag_future = pool.submit_task(func);
            check(flag_future.get() && flag);
        }
        sync_out.println("Checking that submit_task() works for a function with one argument and a return value...");
        {
            bool flag = false;
            const auto func = [](bool& flag_)
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
        sync_out.println("Checking that submit_task() works for a function with two arguments and a return value...");
        {
            bool flag1 = false;
            bool flag2 = false;
            const auto func = [](bool& flag1_, bool& flag2_)
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
    sync_out.println("Checking that ", which_func, " does not create unnecessary copies of the function object...");
    {
        bool copied = false;
        bool moved = false;
        auto test_copy = [detect = detect_copy_move(), &copied, &moved]
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
    sync_out.println("Checking that ", which_func, " correctly accepts arguments passed by value, reference, and constant reference...");
    {
        {
            sync_out.println("Value:");
            const std::int64_t pass_me_by_value = 0;
            const auto func_value = [](std::int64_t passed_by_value)
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
            sync_out.println("Reference:");
            std::int64_t pass_me_by_ref = 0;
            const auto func_ref = [](std::int64_t& passed_by_ref)
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
            sync_out.println("Constant reference:");
            std::int64_t pass_me_by_cref = 0;
            BS::binary_semaphore sem(0);
            const auto func_cref = [&sem](const std::int64_t& passed_by_cref)
            {
                sem.acquire();
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
                sem.release();
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
                sem.release();
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
    explicit flag_class(BS::thread_pool<>& pool_) : pool(&pool_) {}

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
    BS::thread_pool<>* pool;
}; // class flag_class

/**
 * @brief Check that submitting member functions works.
 */
void check_member_function()
{
    BS::thread_pool pool;
    sync_out.println("Checking that detach_task() works for a member function with no arguments or return value...");
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
    sync_out.println("Checking that detach_task() works for a member function with one argument and no return value...");
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
    sync_out.println("Checking that submit_task() works for a member function with no arguments or return value...");
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
    sync_out.println("Checking that submit_task() works for a member function with one argument and no return value...");
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
    sync_out.println("Checking that submit_task() works for a member function with no arguments and a return value...");
    {
        flag_class flag(pool);
        std::future<bool> flag_future = pool.submit_task(
            [&flag]
            {
                return flag.set_flag_no_args_return();
            });
        check(flag_future.get() && flag.get_flag());
    }
    sync_out.println("Checking that submit_task() works for a member function with one argument and a return value...");
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
    sync_out.println("Checking that detach_task() works within an object for a member function with no arguments or return value...");
    {
        flag_class flag(pool);
        flag.detach_test_flag_no_args();
    }
    sync_out.println("Checking that detach_task() works within an object for a member function with one argument and no return value...");
    {
        flag_class flag(pool);
        flag.detach_test_flag_one_arg();
    }
    sync_out.println("Checking that submit_task() works within an object for a member function with no arguments or return value...");
    {
        flag_class flag(pool);
        flag.submit_test_flag_no_args();
    }
    sync_out.println("Checking that submit_task() works within an object for a member function with one argument and no return value...");
    {
        flag_class flag(pool);
        flag.submit_test_flag_one_arg();
    }
    sync_out.println("Checking that submit_task() works within an object for a member function with no arguments and a return value...");
    {
        flag_class flag(pool);
        flag.submit_test_flag_no_args_return();
    }
    sync_out.println("Checking that submit_task() works within an object for a member function with one argument and a return value...");
    {
        flag_class flag(pool);
        flag.submit_test_flag_one_arg_return();
    }
}

std::atomic<bool> check_callables_flag = false;

void normal_func()
{
    check_callables_flag = true;
}

struct functor
{
    void operator()()
    {
        check_callables_flag = true;
    }
};

struct has_member_function
{
    static void member_function()
    {
        check_callables_flag = true;
    }
};

/**
 * @brief Check that different callable types are accepted by the thread pool.
 */
void check_callables()
{
    BS::thread_pool pool;

    sync_out.println("Checking normal function...");
    pool.submit_task(normal_func).wait();
    check(check_callables_flag);

    sync_out.println("Checking function pointer...");
    check_callables_flag = false;
    void (*const func_ptr)() = normal_func;
    pool.submit_task(func_ptr).wait();
    check(check_callables_flag);

    sync_out.println("Checking pointer to static member function...");
    check_callables_flag = false;
    auto member_func_ptr = has_member_function::member_function;
    pool.submit_task(member_func_ptr).wait();
    check(check_callables_flag);

    sync_out.println("Checking lambda expression...");
    check_callables_flag = false;
    const auto lambda = []
    {
        check_callables_flag = true;
    };
    pool.submit_task(lambda).wait();
    check(check_callables_flag);

    sync_out.println("Checking std::function...");
    check_callables_flag = false;
    const std::function<void()> function = []
    {
        check_callables_flag = true;
    };
    pool.submit_task(function).wait();
    check(check_callables_flag);

#ifdef __cpp_lib_move_only_function
    sync_out.println("Checking std::move_only_function...");
    check_callables_flag = false;
    std::move_only_function<void()> move_only_function = []
    {
        check_callables_flag = true;
    };
    pool.submit_task(std::move(move_only_function)).wait();
    check(check_callables_flag);
#else
    sync_out.println("Note: std::move_only_function not available, skipping the corresponding test.");
#endif

    sync_out.println("Checking std::bind...");
    check_callables_flag = false;
    const auto lambda_for_bind = [](std::atomic<bool>& flag)
    {
        flag = true;
    };
    pool.submit_task(std::bind(lambda_for_bind, std::ref(check_callables_flag))).wait();
    check(check_callables_flag);

    sync_out.println("Checking functor...");
    check_callables_flag = false;
    const functor functor_instance;
    pool.submit_task(functor_instance).wait();
    check(check_callables_flag);
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
    const std::size_t num_tasks = pool.get_thread_count() * 10;
    std::vector<std::atomic<bool>> flags(num_tasks);
    for (std::size_t i = 0; i < num_tasks; ++i)
    {
        pool.detach_task(
            [&flags, i, sleep_time]
            {
                std::this_thread::sleep_for(sleep_time);
                flags[i] = true;
            });
    }
    sync_out.println("Waiting for tasks...");
    pool.wait();
    check(all_flags_set(flags));
}

/**
 * @brief Check that wait() correctly blocks all external threads that call it.
 */
void check_wait_blocks()
{
    constexpr std::chrono::milliseconds sleep_time(100);
    constexpr std::size_t num_waiting_tasks = 4;
    BS::thread_pool pool;
    BS::binary_semaphore sem(0);
    sync_out.println("Checking that wait() correctly blocks all external threads that call it...");
    pool.detach_task(
        [&sem]
        {
            sync_out.println("Task submitted to pool 1 and waiting to be released...");
            sem.acquire();
            sync_out.println("Task released.");
        });
    BS::thread_pool temp_pool(num_waiting_tasks);
    std::vector<std::atomic<bool>> flags(num_waiting_tasks);
    const auto waiting_task = [&flags, &pool](const std::size_t task_num)
    {
        sync_out.println("Task ", task_num, " submitted to pool 2 and waiting for pool 1's task to finish...");
        pool.wait();
        sync_out.println("Task ", task_num, " finished waiting.");
        flags[task_num] = true;
    };
    for (std::size_t i = 0; i < num_waiting_tasks; ++i)
    {
        temp_pool.detach_task(
            [&waiting_task, i]
            {
                waiting_task(i);
            });
    }
    std::this_thread::sleep_for(sleep_time);
    check(no_flags_set(flags));
    sem.release();
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
    sync_out.println("Checking that wait_for() works...");
    std::atomic<bool> done = false;
    pool.detach_task(
        [&done, long_sleep_time]
        {
            std::this_thread::sleep_for(long_sleep_time);
            done = true;
        });
    sync_out.println("Task that lasts ", long_sleep_time.count(), "ms submitted. Waiting for ", short_sleep_time.count(), "ms...");
    pool.wait_for(short_sleep_time);
    check(!done);
    sync_out.println("Waiting for ", long_sleep_time.count() * 2, "ms...");
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
    sync_out.println("Checking that wait_until() works...");
    std::atomic<bool> done = false;
    pool.detach_task(
        [&done, long_sleep_time]
        {
            std::this_thread::sleep_for(long_sleep_time);
            done = true;
        });
    const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    sync_out.println("Task that lasts ", long_sleep_time.count(), "ms submitted. Waiting until ", short_sleep_time.count(), "ms from submission time...");
    pool.wait_until(now + short_sleep_time);
    check(!done);
    sync_out.println("Waiting until ", long_sleep_time.count() * 2, "ms from submission time...");
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
    constexpr std::size_t n_waiting_tasks = 1000;
    sync_out.println("Checking for deadlocks when waiting for tasks...");
    BS::thread_pool pool(1);
    pool.detach_task(
        [sleep_time]
        {
            std::this_thread::sleep_for(sleep_time);
        });
    std::atomic<std::size_t> count = 0;
    for (std::size_t j = 0; j < n_waiting_tasks; ++j)
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
        const std::size_t old_count = count;
        check_wait_multiple_deadlock_pool.wait_for(sleep_time * 2);
        if (count == n_waiting_tasks)
        {
            sync_out.println("All waiting tasks successfully finished!");
            passed = true;
            break;
        }
        if (count == old_count)
        {
            sync_out.println("Error: deadlock detected!");
            passed = false;
            break;
        }
        sync_out.println(count, " tasks out of ", n_waiting_tasks, " finished waiting...");
    }
    check(passed);
}

#ifdef __cpp_exceptions
// An auxiliary thread pool used by check_wait_self_deadlock(). It's a global variable so that the program will not get stuck upon destruction of this pool if a deadlock actually occurs.
BS::wdc_thread_pool check_wait_self_deadlock_pool;

/**
 * @brief Check that calling wait() from within a thread of the same pool throws an exception instead of creating a deadlock.
 */
void check_wait_self_deadlock()
{
    constexpr std::chrono::milliseconds sleep_time(100);
    sync_out.println("Checking for deadlocks when waiting from within a thread of the same pool...");
    std::atomic<bool> passed = false;
    check_wait_self_deadlock_pool.detach_task(
        [&passed]
        {
            try
            {
                check_wait_self_deadlock_pool.wait();
            }
            catch (const BS::wait_deadlock&)
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
bool check_loop_no_return(BS::thread_pool<>& pool, const std::int64_t random_start, const std::int64_t random_end, const std::size_t num_tasks, const std::string_view which_func)
{
    sync_out.println("Verifying that ", which_func, " from ", random_start, " to ", random_end, " with ", num_tasks, num_tasks == 1 ? " task" : " tasks", " modifies all indices exactly once...");
    const std::size_t num_indices = static_cast<std::size_t>(random_end - random_start);
    std::vector<std::atomic<std::int64_t>> flags(num_indices);
    std::atomic<bool> indices_out_of_range = false;
    const auto loop = [&flags, random_start, random_end, &indices_out_of_range](const std::int64_t idx)
    {
        if (idx < random_start || idx > random_end)
            indices_out_of_range = true;
        else
            ++flags[static_cast<std::size_t>(idx - random_start)];
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
        sync_out.println("Error: Loop indices out of range!");
        return false;
    }
    return all_flags_equal(flags, 1);
}

/**
 * @brief Check that detach_loop() and submit_loop() work using several different random values for the range of indices and number of tasks.
 */
void check_loop()
{
    constexpr std::int64_t range = 1000000;
    constexpr std::size_t repeats = 10;
    BS::thread_pool pool;
    for (std::size_t i = 0; i < repeats; ++i)
    {
        const std::pair<std::int64_t, std::int64_t> indices = random_pair(-range, range);
        check(check_loop_no_return(pool, indices.first, indices.second, random<std::size_t>(1, pool.get_thread_count()), "detach_loop()"));
    }
    for (std::size_t i = 0; i < repeats; ++i)
    {
        const std::pair<std::int64_t, std::int64_t> indices = random_pair(-range, range);
        check(check_loop_no_return(pool, indices.first, indices.second, random<std::size_t>(1, pool.get_thread_count()), "submit_loop()"));
    }
    sync_out.println("Verifying that detach_loop() with identical start and end indices does nothing...");
    {
        std::atomic<std::size_t> count = 0;
        const std::int64_t index = random(-range, range);
        sync_out.println("Range: ", index, " to ", index);
        pool.detach_loop(index, index,
            [&count](const std::int64_t)
            {
                ++count;
            });
        pool.wait();
        check(count == 0);
    }
    sync_out.println("Verifying that submit_loop() with identical start and end indices does nothing...");
    {
        std::atomic<std::size_t> count = 0;
        const std::int64_t index = random(-range, range);
        sync_out.println("Range: ", index, " to ", index);
        pool.submit_loop(index, index,
                [&count](const std::int64_t)
                {
                    ++count;
                })
            .wait();
        check(count == 0);
    }
    sync_out.println("Verifying that detach_loop() with end index smaller than the start index does nothing...");
    {
        std::atomic<std::size_t> count = 0;
        const std::pair<std::int64_t, std::int64_t> indices = random_pair(-range, range);
        sync_out.println("Range: ", indices.second, " to ", indices.first);
        pool.detach_loop(indices.second, indices.first,
            [&count](const std::int64_t)
            {
                ++count;
            });
        pool.wait();
        check(count == 0);
    }
    sync_out.println("Verifying that submit_loop() with end index smaller than the start index does nothing...");
    {
        std::atomic<std::size_t> count = 0;
        const std::pair<std::int64_t, std::int64_t> indices = random_pair(-range, range);
        sync_out.println("Range: ", indices.second, " to ", indices.first);
        pool.submit_loop(indices.second, indices.first,
                [&count](const std::int64_t)
                {
                    ++count;
                })
            .wait();
        check(count == 0);
    }
    sync_out.println("Trying detach_loop() with a number of tasks larger than the number of indices:");
    {
        const std::int64_t start = random(-range, range);
        check(check_loop_no_return(pool, start, start + random<std::int64_t>(0, static_cast<std::int64_t>(pool.get_thread_count() * 2)), random<std::size_t>(pool.get_thread_count() * 2, pool.get_thread_count() * 4), "detach_loop()"));
    }
    sync_out.println("Trying submit_loop() with a number of tasks larger than the number of indices:");
    {
        const std::int64_t start = random(-range, range);
        check(check_loop_no_return(pool, start, start + random<std::int64_t>(0, static_cast<std::int64_t>(pool.get_thread_count() * 2)), random<std::size_t>(pool.get_thread_count() * 2, pool.get_thread_count() * 4), "submit_loop()"));
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
bool check_blocks_no_return(BS::thread_pool<>& pool, const std::int64_t random_start, const std::int64_t random_end, const std::size_t num_tasks, const std::string_view which_func)
{
    sync_out.println("Verifying that ", which_func, " from ", random_start, " to ", random_end, " with ", num_tasks, num_tasks == 1 ? " task" : " tasks", " modifies all indices exactly once...");
    const std::size_t num_indices = static_cast<std::size_t>(random_end - random_start);
    std::vector<std::atomic<std::int64_t>> flags(num_indices);
    std::atomic<bool> indices_out_of_range = false;
    const auto loop = [&flags, random_start, random_end, &indices_out_of_range](const std::int64_t start, const std::int64_t end)
    {
        if (start < random_start || end > random_end)
        {
            indices_out_of_range = true;
        }
        else
        {
            for (std::int64_t i = start; i < end; ++i)
                ++flags[static_cast<std::size_t>(i - random_start)];
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
        sync_out.println("Error: Block indices out of range!");
        return false;
    }
    return all_flags_equal(flags, 1);
}

/**
 * @brief Check that submit_blocks() works for a specific range of indices split over a specific number of tasks, with a return value.
 *
 * @param pool The thread pool to check.
 * @param random_start The first index in the loop.
 * @param random_end The last index in the loop plus 1.
 * @param num_tasks The number of tasks.
 */
void check_blocks_return(BS::thread_pool<>& pool, const std::int64_t random_start, const std::int64_t random_end, const std::size_t num_tasks)
{
    sync_out.println("Verifying that submit_blocks() from ", random_start, " to ", random_end, " with ", num_tasks, num_tasks == 1 ? " task" : " tasks", " correctly sums all indices...");
    const auto loop = [](const std::int64_t start, const std::int64_t end)
    {
        std::int64_t total = 0;
        for (std::int64_t i = start; i < end; ++i)
            total += i;
        return total;
    };
    const std::vector<std::int64_t> sums_vector = pool.submit_blocks(random_start, random_end, loop, num_tasks).get();
    std::int64_t sum = 0;
    for (const std::int64_t partial_sum : sums_vector)
        sum += partial_sum;
    check(std::abs(random_start - random_end) * (random_start + random_end - 1) / 2, sum);
}

/**
 * @brief Check that detach_blocks() and submit_blocks() work using several different random values for the range of indices and number of tasks.
 */
void check_blocks()
{
    constexpr std::int64_t range = 1000000;
    constexpr std::size_t repeats = 10;
    BS::thread_pool pool;
    for (std::size_t i = 0; i < repeats; ++i)
    {
        const std::pair<std::int64_t, std::int64_t> indices = random_pair(-range, range);
        check(check_blocks_no_return(pool, indices.first, indices.second, random<std::size_t>(1, pool.get_thread_count()), "detach_blocks()"));
    }
    for (std::size_t i = 0; i < repeats; ++i)
    {
        const std::pair<std::int64_t, std::int64_t> indices = random_pair(-range, range);
        check(check_blocks_no_return(pool, indices.first, indices.second, random<std::size_t>(1, pool.get_thread_count()), "submit_blocks()"));
    }
    for (std::size_t i = 0; i < repeats; ++i)
    {
        const std::pair<std::int64_t, std::int64_t> indices = random_pair(-range, range);
        check_blocks_return(pool, indices.first, indices.second, random<std::size_t>(1, pool.get_thread_count()));
    }
    sync_out.println("Verifying that detach_blocks() with identical start and end indices does nothing...");
    {
        std::atomic<std::size_t> count = 0;
        const std::int64_t index = random(-range, range);
        sync_out.println("Range: ", index, " to ", index);
        pool.detach_blocks(index, index,
            [&count](const std::int64_t, const std::int64_t)
            {
                ++count;
            });
        pool.wait();
        check(count == 0);
    }
    sync_out.println("Verifying that submit_blocks() with identical start and end indices does nothing...");
    {
        std::atomic<std::size_t> count = 0;
        const std::int64_t index = random(-range, range);
        sync_out.println("Range: ", index, " to ", index);
        pool.submit_blocks(index, index,
                [&count](const std::int64_t, const std::int64_t)
                {
                    ++count;
                })
            .wait();
        check(count == 0);
    }
    sync_out.println("Verifying that detach_blocks() with end index smaller than the start index does nothing...");
    {
        std::atomic<std::size_t> count = 0;
        const std::pair<std::int64_t, std::int64_t> indices = random_pair(-range, range);
        sync_out.println("Range: ", indices.second, " to ", indices.first);
        pool.detach_blocks(indices.second, indices.first,
            [&count](const std::int64_t, const std::int64_t)
            {
                ++count;
            });
        pool.wait();
        check(count == 0);
    }
    sync_out.println("Verifying that submit_blocks() with end index smaller than the start index does nothing...");
    {
        std::atomic<std::size_t> count = 0;
        const std::pair<std::int64_t, std::int64_t> indices = random_pair(-range, range);
        sync_out.println("Range: ", indices.second, " to ", indices.first);
        pool.submit_blocks(indices.second, indices.first,
                [&count](const std::int64_t, const std::int64_t)
                {
                    ++count;
                })
            .wait();
        check(count == 0);
    }
    sync_out.println("Trying detach_blocks() with a number of tasks larger than the number of indices:");
    {
        const std::int64_t start = random(-range, range);
        check(check_blocks_no_return(pool, start, start + random<std::int64_t>(0, static_cast<std::int64_t>(pool.get_thread_count() * 2)), random<std::size_t>(pool.get_thread_count() * 2, pool.get_thread_count() * 4), "detach_blocks()"));
    }
    sync_out.println("Trying submit_blocks() with a number of tasks larger than the number of indices:");
    {
        const std::int64_t start = random(-range, range);
        check(check_blocks_no_return(pool, start, start + random<std::int64_t>(0, static_cast<std::int64_t>(pool.get_thread_count() * 2)), random<std::size_t>(pool.get_thread_count() * 2, pool.get_thread_count() * 4), "submit_blocks()"));
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
bool check_sequence_no_return(BS::thread_pool<>& pool, const std::int64_t random_start, const std::int64_t random_end, const std::string_view which_func)
{
    sync_out.println("Verifying that ", which_func, " from ", random_start, " to ", random_end, " modifies all indices exactly once...");
    const std::size_t num_indices = static_cast<std::size_t>(random_end - random_start);
    std::vector<std::atomic<std::int64_t>> flags(num_indices);
    std::atomic<bool> indices_out_of_range = false;
    const auto sequence = [&flags, random_start, random_end, &indices_out_of_range](const std::int64_t index)
    {
        if (index < random_start || index > random_end)
            indices_out_of_range = true;
        else
            ++flags[static_cast<std::size_t>(index - random_start)];
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
        sync_out.println("Error: Sequence indices out of range!");
        return false;
    }
    return all_flags_equal(flags, 1);
}

/**
 * @brief Check that submit_sequence() works for a specific range of indices, with a return value.
 *
 * @param pool The thread pool to check.
 * @param random_start The first index in the sequence.
 * @param random_end The last index in the sequence plus 1.
 */
void check_sequence_return(BS::thread_pool<>& pool, const std::int64_t random_start, const std::int64_t random_end)
{
    sync_out.println("Verifying that submit_sequence() from ", random_start, " to ", random_end, " correctly sums all squares of indices...");
    const auto sequence = [](const std::int64_t index)
    {
        return index * index;
    };
    const std::vector<std::int64_t> sums_vector = pool.submit_sequence(random_start, random_end, sequence).get();
    std::int64_t sum = 0;
    for (const std::int64_t partial_sum : sums_vector)
        sum += partial_sum;
    std::int64_t correct_sum = 0;
    for (std::int64_t i = random_start; i < random_end; i++)
        correct_sum += i * i;
    check(correct_sum, sum);
}

/**
 * @brief Check that detach_sequence() and submit_sequence() work using several different random values for the range of indices.
 */
void check_sequence()
{
    constexpr std::int64_t range = 1000;
    constexpr std::size_t repeats = 10;
    BS::thread_pool pool;
    for (std::size_t i = 0; i < repeats; ++i)
    {
        const std::pair<std::int64_t, std::int64_t> indices = random_pair(-range, range);
        check(check_sequence_no_return(pool, indices.first, indices.second, "detach_sequence()"));
    }
    for (std::size_t i = 0; i < repeats; ++i)
    {
        const std::pair<std::int64_t, std::int64_t> indices = random_pair(-range, range);
        check(check_sequence_no_return(pool, indices.first, indices.second, "submit_sequence()"));
    }
    for (std::size_t i = 0; i < repeats; ++i)
    {
        const std::pair<std::int64_t, std::int64_t> indices = random_pair(-range, range);
        check_sequence_return(pool, indices.first, indices.second);
    }
    sync_out.println("Verifying that detach_sequence() with identical start and end indices does nothing...");
    {
        std::atomic<std::size_t> count = 0;
        const std::int64_t index = random(-range, range);
        sync_out.println("Range: ", index, " to ", index);
        pool.detach_sequence(index, index,
            [&count](const std::int64_t)
            {
                ++count;
            });
        pool.wait();
        check(count == 0);
    }
    sync_out.println("Verifying that submit_sequence() with identical start and end indices does nothing...");
    {
        std::atomic<std::size_t> count = 0;
        const std::int64_t index = random(-range, range);
        sync_out.println("Range: ", index, " to ", index);
        pool.submit_sequence(index, index,
                [&count](const std::int64_t)
                {
                    ++count;
                })
            .wait();
        check(count == 0);
    }
    sync_out.println("Verifying that detach_sequence() with end index smaller than the start index does nothing...");
    {
        std::atomic<std::size_t> count = 0;
        const std::pair<std::int64_t, std::int64_t> indices = random_pair(-range, range);
        sync_out.println("Range: ", indices.second, " to ", indices.first);
        pool.detach_sequence(indices.second, indices.first,
            [&count](const std::int64_t)
            {
                ++count;
            });
        pool.wait();
        check(count == 0);
    }
    sync_out.println("Verifying that submit_sequence() with end index smaller than the start index does nothing...");
    {
        std::atomic<std::size_t> count = 0;
        const std::pair<std::int64_t, std::int64_t> indices = random_pair(-range, range);
        sync_out.println("Range: ", indices.second, " to ", indices.first);
        pool.submit_sequence(indices.second, indices.first,
                [&count](const std::int64_t)
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
    const std::size_t num_threads = std::min<std::size_t>(std::thread::hardware_concurrency(), 4);
    sync_out.println("Creating pool with ", num_threads, " threads.");
    BS::thread_pool pool(num_threads);
    sync_out.println("Submitting ", num_threads * 3, " tasks.");
    BS::counting_semaphore sem(0);
    for (std::size_t i = 0; i < num_threads * 3; ++i)
    {
        pool.detach_task(
            [i, &sem]
            {
                sem.acquire();
                sync_out.println("Task ", i, " released.");
            });
    }
    std::this_thread::sleep_for(sleep_time);
    sync_out.println("After submission, should have: ", num_threads * 3, " tasks total, ", num_threads, " tasks running, ", num_threads * 2, " tasks queued...");
    sync_out.print("Result: ", pool.get_tasks_total(), " tasks total, ", pool.get_tasks_running(), " tasks running, ", pool.get_tasks_queued(), " tasks queued ");
    check(pool.get_tasks_total() == num_threads * 3 && pool.get_tasks_running() == num_threads && pool.get_tasks_queued() == num_threads * 2);
    sem.release(static_cast<std::ptrdiff_t>(num_threads));
    std::this_thread::sleep_for(sleep_time);
    sync_out.println("After releasing ", num_threads, " tasks, should have: ", num_threads * 2, " tasks total, ", num_threads, " tasks running, ", num_threads, " tasks queued...");
    sync_out.print("Result: ", pool.get_tasks_total(), " tasks total, ", pool.get_tasks_running(), " tasks running, ", pool.get_tasks_queued(), " tasks queued ");
    check(pool.get_tasks_total() == num_threads * 2 && pool.get_tasks_running() == num_threads && pool.get_tasks_queued() == num_threads);
    sem.release(static_cast<std::ptrdiff_t>(num_threads));
    std::this_thread::sleep_for(sleep_time);
    sync_out.println("After releasing ", num_threads, " more tasks, should have: ", num_threads, " tasks total, ", num_threads, " tasks running, ", 0, " tasks queued...");
    sync_out.print("Result: ", pool.get_tasks_total(), " tasks total, ", pool.get_tasks_running(), " tasks running, ", pool.get_tasks_queued(), " tasks queued ");
    check(pool.get_tasks_total() == num_threads && pool.get_tasks_running() == num_threads && pool.get_tasks_queued() == 0);
    sem.release(static_cast<std::ptrdiff_t>(num_threads));
    std::this_thread::sleep_for(sleep_time);
    sync_out.println("After releasing the final ", num_threads, " tasks, should have: ", 0, " tasks total, ", 0, " tasks running, ", 0, " tasks queued...");
    sync_out.print("Result: ", pool.get_tasks_total(), " tasks total, ", pool.get_tasks_running(), " tasks running, ", pool.get_tasks_queued(), " tasks queued ");
    check(pool.get_tasks_total() == 0 && pool.get_tasks_running() == 0 && pool.get_tasks_queued() == 0);
}

/**
 * @brief Check that pausing works.
 */
void check_pausing()
{
    constexpr std::chrono::milliseconds sleep_time(200);
    BS::pause_thread_pool pool;
    sync_out.println("Checking that the pool correctly reports that it is not paused after construction...");
    check(!pool.is_paused());
    sync_out.println("Pausing pool.");
    pool.pause();
    sync_out.println("Checking that the pool correctly reports that it is paused...");
    check(pool.is_paused());
    sync_out.println("Submitting task and waiting.");
    std::atomic<bool> flag = false;
    pool.detach_task(
        [&flag]
        {
            flag = true;
            sync_out.println("Task executed.");
        });
    std::this_thread::sleep_for(sleep_time);
    sync_out.println("Verifying that the task has not been executed...");
    check(!flag);
    sync_out.println("Unpausing pool and waiting.");
    pool.unpause();
    std::this_thread::sleep_for(sleep_time);
    sync_out.println("Verifying that the task has been executed...");
    check(flag);
    sync_out.println("Checking that the pool correctly reports that it is not paused...");
    check(!pool.is_paused());
}

/**
 * @brief Check that purge() works.
 */
void check_purge()
{
    constexpr std::chrono::milliseconds long_sleep_time(200);
    constexpr std::chrono::milliseconds short_sleep_time(100);
    constexpr std::size_t num_tasks = 10;
    BS::thread_pool pool(1);
    sync_out.println("Submitting ", num_tasks, " tasks to the pool.");
    std::vector<std::atomic<bool>> flags(num_tasks);
    for (std::size_t i = 0; i < num_tasks; ++i)
    {
        pool.detach_task(
            [&flags, i, long_sleep_time]
            {
                std::this_thread::sleep_for(long_sleep_time);
                sync_out.println("Task ", i, " done.");
                flags[i] = true;
            });
    }
    std::this_thread::sleep_for(short_sleep_time);
    sync_out.println("Purging the pool and waiting for tasks...");
    pool.purge();
    pool.wait();
    sync_out.println("Checking that only the first task was executed...");
    flags[0] = !flags[0];
    check(no_flags_set(flags));
}

#ifdef __cpp_exceptions
// ======================================
// Functions to verify exception handling
// ======================================

/**
 * @brief An exception class to be thrown when testing exception handling.
 */
struct test_exception : public std::runtime_error
{
    test_exception() : std::runtime_error("Exception thrown!") {};
};

/**
 * @brief A function that throws a `test_exception`.
 */
void throws()
{
    sync_out.println("Throwing exception...");
    throw test_exception();
};

/**
 * @brief Check that exceptions are forwarded correctly by submit_task().
 */
void check_exceptions_submit()
{
    BS::thread_pool pool;
    sync_out.println("Checking that exceptions are forwarded correctly by submit_task()...");
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
    sync_out.println("Checking that exceptions are forwarded correctly by BS::multi_future...");
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
#endif

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
bool check_vector_of_size(BS::thread_pool<>& pool, const std::size_t vector_size, const std::size_t num_tasks)
{
    constexpr std::int64_t value_range = 1000000;
    std::vector<std::int64_t> vector_1(vector_size);
    std::vector<std::int64_t> vector_2(vector_size);
    for (std::size_t i = 0; i < vector_size; ++i)
    {
        vector_1[i] = random(-value_range, value_range);
        vector_2[i] = random(-value_range, value_range);
    }
    sync_out.println("Adding two vectors with ", vector_size, " elements using ", num_tasks, " tasks...");
    std::vector<std::int64_t> sum_single(vector_size);
    for (std::size_t i = 0; i < vector_size; ++i)
        sum_single[i] = vector_1[i] + vector_2[i];
    std::vector<std::int64_t> sum_multi(vector_size);
    pool.submit_blocks(
            0, vector_size,
            [&sum_multi, &vector_1, &vector_2](const std::size_t start, const std::size_t end)
            {
                for (std::size_t i = start; i < end; ++i)
                    sum_multi[i] = vector_1[i] + vector_2[i];
            },
            num_tasks)
        .wait();
    for (std::size_t i = 0; i < vector_size; ++i)
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
    constexpr std::size_t size_range = 1000000;
    constexpr std::size_t repeats = 10;
    BS::thread_pool pool;
    for (std::size_t i = 0; i < repeats; ++i)
        check(check_vector_of_size(pool, random<std::size_t>(0, size_range), random<std::size_t>(1, pool.get_thread_count())));
}

// =================================
// Functions to verify task priority
// =================================

// Priorities are 8-bit integers, but `std::uniform_int_distribution` needs at least a 16-bit integer.
using rand_priority_t = std::int16_t;

/**
 * @brief Check that task priority works as expected with all task submission methods.
 */
void check_priority()
{
    constexpr std::chrono::milliseconds sleep_time(200);
    constexpr std::size_t num_tasks = 10;
    // Set the pool to have only 1 thread, so it can only run 1 task at a time. This will ensure the tasks will be executed in priority order.
    BS::thread_pool<BS::tp::priority | BS::tp::pause> pool(1);
    pool.pause();

    // Create a shuffled lists of priorities.
    std::vector<BS::priority_t> priorities;
    priorities.reserve(num_tasks - 1);
    for (std::size_t i = 0; i < num_tasks - 1; ++i)
        priorities.push_back(static_cast<BS::priority_t>((i % 2 == 0) ? random<rand_priority_t>(0, BS::pr::highest) : random<rand_priority_t>(BS::pr::lowest, 0)));
    priorities.push_back(BS::pr::lowest);
    priorities.push_back(0);
    priorities.push_back(BS::pr::highest);
    std::shuffle(priorities.begin(), priorities.end(), std::mt19937_64(std::random_device()()));

    // Submit tasks using various methods in random priority order.
    std::vector<BS::priority_t> execution_order;
    std::mutex exec_mutex;
    const auto execute_task_priority = [&execution_order, &exec_mutex](const BS::priority_t priority)
    {
        const std::scoped_lock lock(exec_mutex);
        sync_out.println("Task with priority ", static_cast<rand_priority_t>(priority), " executed.");
        execution_order.push_back(priority);
    };
    const std::vector<std::string_view> functions = {"detach_task", "submit_task", "detach_sequence", "submit_sequence", "detach_loop", "submit_loop", "detach_blocks", "submit_blocks"};
    for (const BS::priority_t priority : priorities)
    {
        const std::string_view func = functions[random<std::size_t>(0, functions.size() - 1)];
        sync_out.println("Launching ", func, "() with priority ", static_cast<rand_priority_t>(priority), "...");
        if (func == "detach_task")
        {
            pool.detach_task(
                [priority, &execute_task_priority]
                {
                    execute_task_priority(priority);
                },
                priority);
        }
        else if (func == "submit_task")
        {
            std::ignore = pool.submit_task(
                [priority, &execute_task_priority]
                {
                    execute_task_priority(priority);
                },
                priority);
        }
        else if (func == "detach_sequence")
        {
            pool.detach_sequence(
                0, 1,
                [priority, &execute_task_priority](std::int64_t)
                {
                    execute_task_priority(priority);
                },
                priority);
        }
        else if (func == "submit_sequence")
        {
            std::ignore = pool.submit_sequence(
                0, 1,
                [priority, &execute_task_priority](std::int64_t)
                {
                    execute_task_priority(priority);
                },
                priority);
        }
        else if (func == "detach_loop")
        {
            pool.detach_loop(
                0, 1,
                [priority, &execute_task_priority](std::int64_t)
                {
                    execute_task_priority(priority);
                },
                0, priority);
        }
        else if (func == "submit_loop")
        {
            std::ignore = pool.submit_loop(
                0, 1,
                [priority, &execute_task_priority](std::int64_t)
                {
                    execute_task_priority(priority);
                },
                0, priority);
        }
        else if (func == "detach_blocks")
        {
            pool.detach_blocks(
                0, 1,
                [priority, &execute_task_priority](std::int64_t, std::int64_t)
                {
                    execute_task_priority(priority);
                },
                0, priority);
        }
        else if (func == "submit_blocks")
        {
            std::ignore = pool.submit_blocks(
                0, 1,
                [priority, &execute_task_priority](std::int64_t, std::int64_t)
                {
                    execute_task_priority(priority);
                },
                0, priority);
        }
    }

    // Unpause the pool so the tasks can be executed, then check that they were executed in the correct order.
    sync_out.println("Checking execution order...");
    std::this_thread::sleep_for(sleep_time);
    pool.unpause();
    pool.wait();
    std::sort(priorities.rbegin(), priorities.rend());
    check(execution_order == priorities);
}

// =======================================================================
// Functions to verify thread initialization, cleanup, and BS::this_thread
// =======================================================================

/**
 * @brief Check that thread initialization functions and get_index() work.
 */
void check_init()
{
    sync_out.println("Comparing thread indices reported by get_index() using an initialization function passed to reset():");
    std::vector<std::atomic<std::size_t>> thread_indices(std::thread::hardware_concurrency());
    std::atomic<bool> correct = true;
    BS::thread_pool pool;
    pool.reset(
        [&thread_indices, &correct](std::size_t idx)
        {
            const std::optional<std::size_t> reported_idx = BS::this_thread::get_index();
            if (reported_idx.has_value())
                thread_indices[idx] = reported_idx.value();
            else
                correct = false;
        });
    pool.wait();
    sync_out.println("Checking that all reported indices have values...");
    check(correct);
    correct = true;
    for (std::size_t i = 0; i < thread_indices.size(); ++i)
    {
        if (thread_indices[i] != i)
        {
            correct = false;
            break;
        }
    }
    sync_out.println("Checking that all reported indices are correct...");
    check(correct);

    sync_out.println("Verifying that the index of the main thread has no value...");
    const std::optional<std::size_t> main_idx = BS::this_thread::get_index();
    check(!main_idx.has_value());

    sync_out.println("Verifying that the index of an independent thread has no value...");
    std::thread test_thread(
        []
        {
            const std::optional<std::size_t> ind_idx = BS::this_thread::get_index();
            check(!ind_idx.has_value());
        });
    test_thread.join();
}

/**
 * @brief Check that thread cleanup functions work.
 */
void check_cleanup()
{
    sync_out.println("Comparing thread indices reported by get_index() using a cleanup function passed to set_cleanup_func():");
    std::vector<std::atomic<std::size_t>> thread_indices(std::thread::hardware_concurrency());
    std::atomic<bool> correct = true;
    {
        BS::thread_pool pool;
        pool.set_cleanup_func(
            [&thread_indices, &correct](std::size_t idx)
            {
                const std::optional<std::size_t> reported_idx = BS::this_thread::get_index();
                if (reported_idx.has_value())
                    thread_indices[idx] = reported_idx.value();
                else
                    correct = false;
            });
    }
    sync_out.println("Checking that all reported indices have values...");
    check(correct);
    correct = true;
    for (std::size_t i = 0; i < thread_indices.size(); ++i)
    {
        if (thread_indices[i] != i)
        {
            correct = false;
            break;
        }
    }
    sync_out.println("Checking that all reported indices are correct...");
    check(correct);
}

/**
 * @brief Check that get_pool() works.
 */
void check_get_pool()
{
    sync_out.println("Checking that all threads report the correct pool...");
    std::vector<std::atomic<void*>> thread_pool_ptrs1(std::thread::hardware_concurrency());
    std::vector<std::atomic<void*>> thread_pool_ptrs2(std::thread::hardware_concurrency());
    const auto store_pointers = [](std::vector<std::atomic<void*>>& ptrs)
    {
        const auto ptr = BS::this_thread::get_pool();
        if (ptr.has_value())
            ptrs[*BS::this_thread::get_index()] = *ptr;
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
    const auto check_pointers = [](const std::vector<std::atomic<void*>>& ptrs, const BS::thread_pool<>& pool)
    {
        check(all_flags_equal(ptrs, (void*)&pool));
    };
    check_pointers(thread_pool_ptrs1, pool1);
    check_pointers(thread_pool_ptrs2, pool2);
    {
        sync_out.println("Verifying that the pool pointer of the main thread has no value...");
        const auto ptr = BS::this_thread::get_pool();
        check(!ptr.has_value());
    }
    {
        sync_out.println("Verifying that the pool pointer of an independent thread has no value...");
        std::thread test_thread(
            []
            {
                const auto ptr = BS::this_thread::get_pool();
                check(!ptr.has_value());
            });
        test_thread.join();
    }
}

// =========================================================
// Functions to verify proper handling of parallelized tasks
// =========================================================

/**
 * @brief A class used to count how many times the copy and move constructors have been invoked since the creation of the initial object.
 */
class [[nodiscard]] count_copy_move
{
public:
    count_copy_move(std::atomic<std::size_t>* copied_, std::atomic<std::size_t>* moved_) : copied(copied_), moved(moved_) {}

    count_copy_move(const count_copy_move& other) : copied(other.copied), moved(other.moved)
    {
        ++(*copied);
    }

    count_copy_move(count_copy_move&& other) noexcept : copied(other.copied), moved(other.moved)
    {
        ++(*moved);
    }

    count_copy_move& operator=(const count_copy_move&) = delete;
    count_copy_move& operator=(count_copy_move&&) = delete;
    ~count_copy_move() = default;

private:
    std::atomic<std::size_t>* copied = nullptr;
    std::atomic<std::size_t>* moved = nullptr;
}; // class count_copy_move

/**
 * @brief Check, for a specific member function which parallelizes loops or sequences of tasks, that the callable object does not get copied in the process.
 *
 * @param which_func A string naming the function to check.
 */
void check_copy(const std::string_view which_func)
{
    BS::thread_pool pool;
    const std::size_t num_tasks = pool.get_thread_count() * 10;
    sync_out.println("Checking ", which_func, "...");
    std::atomic<std::size_t> copied = 0;
    std::atomic<std::size_t> moved = 0;
    auto task = [detect = count_copy_move(&copied, &moved)](auto&&...) {};
    if (which_func == "detach_blocks()")
        pool.detach_blocks(0, num_tasks, std::move(task), num_tasks);
    else if (which_func == "detach_loop()")
        pool.detach_loop(0, num_tasks, std::move(task));
    else if (which_func == "detach_sequence()")
        pool.detach_sequence(0, num_tasks, std::move(task));
    else if (which_func == "submit_blocks()")
        std::ignore = pool.submit_blocks(0, num_tasks, std::move(task), num_tasks);
    else if (which_func == "submit_loop()")
        std::ignore = pool.submit_loop(0, num_tasks, std::move(task));
    else if (which_func == "submit_sequence()")
        std::ignore = pool.submit_sequence(0, num_tasks, std::move(task));
    pool.wait();
    sync_out.println("Copy count: ");
    check(0, copied.load()); // Note: Move count will be unpredictable if priority is on, so we don't check it.
}

/**
 * @brief Check, for all member functions which parallelize loops or sequences of tasks, that the callable object does not get copied in the process.
 */
void check_copy_all()
{
    check_copy("detach_blocks()");
    check_copy("detach_loop()");
    check_copy("detach_sequence()");
    check_copy("submit_blocks()");
    check_copy("submit_loop()");
    check_copy("submit_sequence()");
}

/**
 * @brief A class used to detect if an object was destructed prematurely.
 */
class detect_destruct
{
public:
    explicit detect_destruct(std::atomic<bool>* object_exists_) : object_exists(object_exists_)
    {
        *object_exists = true;
    };

    detect_destruct(const detect_destruct&) = delete;
    detect_destruct(detect_destruct&&) noexcept = delete;
    detect_destruct& operator=(const detect_destruct&) = delete;
    detect_destruct& operator=(detect_destruct&&) = delete;

    ~detect_destruct()
    {
        *object_exists = false;
    };

private:
    std::atomic<bool>* object_exists = nullptr;
};

/**
 * @brief Check, for a specific member function which parallelizes loops or sequences of tasks, that if a task that captures a shared pointer is submitted, the pointer is correctly shared between all the iterations of the task.
 *
 * @param which_func A string naming the function to check.
 */
void check_shared_ptr(const std::string_view which_func)
{
    BS::thread_pool pool;
    constexpr std::chrono::milliseconds sleep_time(10);
    const std::size_t num_tasks = pool.get_thread_count() * 10;
    std::atomic<bool> object_exists = false;
    std::atomic<std::size_t> uses_before_destruct = 0;
    std::atomic<std::size_t> uses_after_destruct = 0;
    sync_out.println("Checking ", which_func, "...");
    {
        std::shared_ptr<detect_destruct> ptr = std::make_shared<detect_destruct>(&object_exists);
        auto task = [ptr, &object_exists, &uses_before_destruct, &uses_after_destruct, &sleep_time](auto&&...)
        {
            std::this_thread::sleep_for(sleep_time);
            if (object_exists)
                ++uses_before_destruct;
            else
                ++uses_after_destruct;
        };
        if (which_func == "detach_blocks()")
            pool.detach_blocks(0, num_tasks, std::move(task), num_tasks);
        else if (which_func == "detach_loop()")
            pool.detach_loop(0, num_tasks, std::move(task));
        else if (which_func == "detach_sequence()")
            pool.detach_sequence(0, num_tasks, std::move(task));
        else if (which_func == "submit_blocks()")
            std::ignore = pool.submit_blocks(0, num_tasks, std::move(task), num_tasks);
        else if (which_func == "submit_loop()")
            std::ignore = pool.submit_loop(0, num_tasks, std::move(task));
        else if (which_func == "submit_sequence()")
            std::ignore = pool.submit_sequence(0, num_tasks, std::move(task));
        ptr.reset();
    }
    pool.wait();
    std::this_thread::sleep_for(sleep_time);
    sync_out.println("Uses before destruct:");
    check(num_tasks, uses_before_destruct.load());
    sync_out.println("Uses after destruct:");
    check(0, uses_after_destruct.load());
}

/**
 * @brief Check, for all member functions which parallelize loops or sequences of tasks, that if a task that captures a shared pointer is submitted, the pointer is correctly shared between all the iterations of the task.
 */
void check_shared_ptr_all()
{
    check_shared_ptr("detach_blocks()");
    check_shared_ptr("detach_loop()");
    check_shared_ptr("detach_sequence()");
    check_shared_ptr("submit_blocks()");
    check_shared_ptr("submit_loop()");
    check_shared_ptr("submit_sequence()");
}

/**
 * @brief Check that a task is destructed immediately after it executes, and therefore does not artificially extend the lifetime of any captured objects.
 */
void check_task_destruct()
{
    constexpr std::chrono::milliseconds sleep_time(20);
    BS::thread_pool pool;
    std::atomic<bool> object_exists = false;
    {
        const std::shared_ptr<detect_destruct> ptr = std::make_shared<detect_destruct>(&object_exists);
        pool.submit_task([ptr] {}).wait();
    }
    std::this_thread::sleep_for(sleep_time);
    check(!object_exists);
}

/**
 * @brief Check that the type trait `BS::common_index_type` works as expected.
 */
void check_common_index_type()
{
    // NOLINTBEGIN(misc-redundant-expression)
    sync_out.println("Checking std::int8_t...");
    check(std::is_same_v<BS::common_index_type_t<std::int8_t, std::int8_t>, std::int8_t> && std::is_same_v<BS::common_index_type_t<std::int8_t, std::int16_t>, std::int16_t> && std::is_same_v<BS::common_index_type_t<std::int8_t, std::int32_t>, std::int32_t> && std::is_same_v<BS::common_index_type_t<std::int8_t, std::int64_t>, std::int64_t> && std::is_same_v<BS::common_index_type_t<std::int8_t, std::uint8_t>, std::int16_t> && std::is_same_v<BS::common_index_type_t<std::int8_t, std::uint16_t>, std::int32_t> && std::is_same_v<BS::common_index_type_t<std::int8_t, std::uint32_t>, std::int64_t> && std::is_same_v<BS::common_index_type_t<std::int8_t, std::uint64_t>, std::uint64_t>);
    sync_out.println("Checking std::int16_t...");
    check(std::is_same_v<BS::common_index_type_t<std::int16_t, std::int8_t>, std::int16_t> && std::is_same_v<BS::common_index_type_t<std::int16_t, std::int16_t>, std::int16_t> && std::is_same_v<BS::common_index_type_t<std::int16_t, std::int32_t>, std::int32_t> && std::is_same_v<BS::common_index_type_t<std::int16_t, std::int64_t>, std::int64_t> && std::is_same_v<BS::common_index_type_t<std::int16_t, std::uint8_t>, std::int16_t> && std::is_same_v<BS::common_index_type_t<std::int16_t, std::uint16_t>, std::int32_t> && std::is_same_v<BS::common_index_type_t<std::int16_t, std::uint32_t>, std::int64_t> && std::is_same_v<BS::common_index_type_t<std::int16_t, std::uint64_t>, std::uint64_t>);
    sync_out.println("Checking std::int32_t...");
    check(std::is_same_v<BS::common_index_type_t<std::int32_t, std::int8_t>, std::int32_t> && std::is_same_v<BS::common_index_type_t<std::int32_t, std::int16_t>, std::int32_t> && std::is_same_v<BS::common_index_type_t<std::int32_t, std::int32_t>, std::int32_t> && std::is_same_v<BS::common_index_type_t<std::int32_t, std::int64_t>, std::int64_t> && std::is_same_v<BS::common_index_type_t<std::int32_t, std::uint8_t>, std::int32_t> && std::is_same_v<BS::common_index_type_t<std::int32_t, std::uint16_t>, std::int32_t> && std::is_same_v<BS::common_index_type_t<std::int32_t, std::uint32_t>, std::int64_t> && std::is_same_v<BS::common_index_type_t<std::int32_t, std::uint64_t>, std::uint64_t>);
    sync_out.println("Checking std::int64_t...");
    check(std::is_same_v<BS::common_index_type_t<std::int64_t, std::int8_t>, std::int64_t> && std::is_same_v<BS::common_index_type_t<std::int64_t, std::int16_t>, std::int64_t> && std::is_same_v<BS::common_index_type_t<std::int64_t, std::int32_t>, std::int64_t> && std::is_same_v<BS::common_index_type_t<std::int64_t, std::int64_t>, std::int64_t> && std::is_same_v<BS::common_index_type_t<std::int64_t, std::uint8_t>, std::int64_t> && std::is_same_v<BS::common_index_type_t<std::int64_t, std::uint16_t>, std::int64_t> && std::is_same_v<BS::common_index_type_t<std::int64_t, std::uint32_t>, std::int64_t> && std::is_same_v<BS::common_index_type_t<std::int64_t, std::uint64_t>, std::uint64_t>);
    sync_out.println("Checking std::uint8_t...");
    check(std::is_same_v<BS::common_index_type_t<std::uint8_t, std::int8_t>, std::int16_t> && std::is_same_v<BS::common_index_type_t<std::uint8_t, std::int16_t>, std::int16_t> && std::is_same_v<BS::common_index_type_t<std::uint8_t, std::int32_t>, std::int32_t> && std::is_same_v<BS::common_index_type_t<std::uint8_t, std::int64_t>, std::int64_t> && std::is_same_v<BS::common_index_type_t<std::uint8_t, std::uint8_t>, std::uint8_t> && std::is_same_v<BS::common_index_type_t<std::uint8_t, std::uint16_t>, std::uint16_t> && std::is_same_v<BS::common_index_type_t<std::uint8_t, std::uint32_t>, std::uint32_t> && std::is_same_v<BS::common_index_type_t<std::uint8_t, std::uint64_t>, std::uint64_t>);
    sync_out.println("Checking std::uint16_t...");
    check(std::is_same_v<BS::common_index_type_t<std::uint16_t, std::int8_t>, std::int32_t> && std::is_same_v<BS::common_index_type_t<std::uint16_t, std::int16_t>, std::int32_t> && std::is_same_v<BS::common_index_type_t<std::uint16_t, std::int32_t>, std::int32_t> && std::is_same_v<BS::common_index_type_t<std::uint16_t, std::int64_t>, std::int64_t> && std::is_same_v<BS::common_index_type_t<std::uint16_t, std::uint8_t>, std::uint16_t> && std::is_same_v<BS::common_index_type_t<std::uint16_t, std::uint16_t>, std::uint16_t> && std::is_same_v<BS::common_index_type_t<std::uint16_t, std::uint32_t>, std::uint32_t> && std::is_same_v<BS::common_index_type_t<std::uint16_t, std::uint64_t>, std::uint64_t>);
    sync_out.println("Checking std::uint32_t...");
    check(std::is_same_v<BS::common_index_type_t<std::uint32_t, std::int8_t>, std::int64_t> && std::is_same_v<BS::common_index_type_t<std::uint32_t, std::int16_t>, std::int64_t> && std::is_same_v<BS::common_index_type_t<std::uint32_t, std::int32_t>, std::int64_t> && std::is_same_v<BS::common_index_type_t<std::uint32_t, std::int64_t>, std::int64_t> && std::is_same_v<BS::common_index_type_t<std::uint32_t, std::uint8_t>, std::uint32_t> && std::is_same_v<BS::common_index_type_t<std::uint32_t, std::uint16_t>, std::uint32_t> && std::is_same_v<BS::common_index_type_t<std::uint32_t, std::uint32_t>, std::uint32_t> && std::is_same_v<BS::common_index_type_t<std::uint32_t, std::uint64_t>, std::uint64_t>);
    sync_out.println("Checking std::uint64_t...");
    check(std::is_same_v<BS::common_index_type_t<std::uint64_t, std::int8_t>, std::uint64_t> && std::is_same_v<BS::common_index_type_t<std::uint64_t, std::int16_t>, std::uint64_t> && std::is_same_v<BS::common_index_type_t<std::uint64_t, std::int32_t>, std::uint64_t> && std::is_same_v<BS::common_index_type_t<std::uint64_t, std::int64_t>, std::uint64_t> && std::is_same_v<BS::common_index_type_t<std::uint64_t, std::uint8_t>, std::uint64_t> && std::is_same_v<BS::common_index_type_t<std::uint64_t, std::uint16_t>, std::uint64_t> && std::is_same_v<BS::common_index_type_t<std::uint64_t, std::uint32_t>, std::uint64_t> && std::is_same_v<BS::common_index_type_t<std::uint64_t, std::uint64_t>, std::uint64_t>);
    // NOLINTEND(misc-redundant-expression)
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
    constexpr std::size_t tries = 10000;
    std::size_t try_n = 0;
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
        const std::size_t old_try_n = try_n;
        check_deadlock_pool.wait_for(sleep_time);
        if (try_n == tries)
        {
            sync_out.println("Successfully finished all tries!");
            passed = true;
            break;
        }
        if (try_n == old_try_n)
        {
            sync_out.println("Error: deadlock detected!");
            passed = false;
            break;
        }
        sync_out.println("Finished ", try_n, " tries out of ", tries, "...");
    }
    check(passed);
}

#ifdef BS_THREAD_POOL_NATIVE_EXTENSIONS
// ====================================
// Functions to check native extensions
// ====================================

/**
 * @brief A map between pre-defined OS process priorities and their string representations.
 */
const std::map<BS::os_process_priority, std::string> os_process_priority_map = {{BS::os_process_priority::idle, "idle"}, {BS::os_process_priority::below_normal, "below_normal"}, {BS::os_process_priority::normal, "normal"}, {BS::os_process_priority::above_normal, "above_normal"}, {BS::os_process_priority::high, "high"}, {BS::os_process_priority::realtime, "realtime"}};

/**
 * @brief Get the string representation of an OS process priority.
 *
 * @param priority A `std::optional<BS::os_process_priority>` object.
 * @return A string containing the name of the priority, or "unknown" if the priority is not recognized, or "N/A" if the optional value is not set.
 */
std::string os_process_priority_name(const std::optional<BS::os_process_priority>& priority)
{
    if (priority.has_value())
    {
        const std::map<BS::os_process_priority, std::string>::const_iterator it = os_process_priority_map.find(*priority);
        return (it != os_process_priority_map.end()) ? it->second : "unknown";
    }
    return "N/A";
}

/**
 * @brief A map between pre-defined OS thread priorities and their string representations.
 */
const std::map<BS::os_thread_priority, std::string> os_thread_priority_map = {{BS::os_thread_priority::idle, "idle"}, {BS::os_thread_priority::lowest, "lowest"}, {BS::os_thread_priority::below_normal, "below_normal"}, {BS::os_thread_priority::normal, "normal"}, {BS::os_thread_priority::above_normal, "above_normal"}, {BS::os_thread_priority::highest, "highest"}, {BS::os_thread_priority::realtime, "realtime"}};

/**
 * @brief Get the string representation of an OS thread priority.
 *
 * @param priority A `std::optional<BS::os_thread_priority>` object.
 * @return A string containing the name of the priority, or "unknown" if the priority is not recognized, or "N/A" if the optional value is not set.
 */
std::string os_thread_priority_name(const std::optional<BS::os_thread_priority>& priority)
{
    if (priority.has_value())
    {
        const std::map<BS::os_thread_priority, std::string>::const_iterator it = os_thread_priority_map.find(*priority);
        return (it != os_thread_priority_map.end()) ? it->second : "unknown";
    }
    return "N/A";
}

/**
 * @brief Check if a condition is met, report the result, but do not keep count of the total number of successes and failures, because failure is expected if the test is not run as root.
 *
 * @param condition The condition to check.
 */
void check_root(const bool condition)
{
    if (condition)
    {
        sync_out.println("-> passed.");
        ++test_results::tests_succeeded;
    }
    else
    {
        sync_out.println("-> failed, most likely due to insufficient permissions; ignoring.");
    }
}

/**
 * @brief Check if the expected result has been obtained, report the result, but do not keep count of the total number of successes and failures, because failure is expected if the test is not run as root.
 *
 * @param condition The condition to check.
 */
template <typename T1, typename T2>
void check_root(const T1& expected, const T2& obtained)
{
    sync_out.print("- Expected: ", expected, ", obtained: ", obtained, ' ');
    check_root(expected == static_cast<T1>(obtained));
}

/**
 * @brief Check that getting and setting OS process priorities works.
 */
void check_os_process_priorities()
{
    sync_out.println("Checking OS process priorities...");
    sync_out.println("NOTE: This test must be run as admin/root, otherwise it will fail!");
    // We go over the priorities in reverse order because on Linux, a non-root user can only decrease the priority, so if we start from the lowest priority, all tests will fail except the first one.
    const std::vector<BS::os_process_priority> priorities = {BS::os_process_priority::realtime, BS::os_process_priority::high, BS::os_process_priority::above_normal, BS::os_process_priority::normal, BS::os_process_priority::below_normal, BS::os_process_priority::idle};
    for (BS::os_process_priority priority : priorities)
    {
        sync_out.print("Setting OS process priority to ", os_process_priority_name(priority), ' ');
        // On Windows we should be able to set all the priorities even as non-admin; realtime will "succeed" but actually set the priority to high. On Linux, only root can increase the priority beyond normal.
    #ifdef _WIN32
        check(BS::set_os_process_priority(priority));
    #else
        if (priority >= BS::os_process_priority::normal)
            check(BS::set_os_process_priority(priority));
        else
            check_root(BS::set_os_process_priority(priority));
    #endif
        const std::optional<BS::os_process_priority> new_priority = BS::get_os_process_priority();
        sync_out.print("Obtaining new OS process priority ");
        check(new_priority.has_value());
    #ifdef _WIN32
        if (priority != BS::os_process_priority::realtime)
            check(os_process_priority_name(priority), os_process_priority_name(new_priority));
        else
            check_root(os_process_priority_name(priority), os_process_priority_name(new_priority));
    #else
        if (priority >= BS::os_process_priority::normal)
            check(os_process_priority_name(priority), os_process_priority_name(new_priority));
        else
            check_root(os_process_priority_name(priority), os_process_priority_name(new_priority));
    #endif
    }
    // Set the priority back to normal after the test ends. This will fail on Linux if not root.
    sync_out.println("Setting priority back to normal...");
    #ifdef _WIN32
    check(BS::set_os_process_priority(BS::os_process_priority::normal));
    #else
    check_root(BS::set_os_process_priority(BS::os_process_priority::normal));
    #endif
}

/**
 * @brief Check that getting and setting OS thread priorities works.
 */
void check_os_thread_priorities()
{
    BS::thread_pool pool;
    pool.detach_task(
        []
        {
            sync_out.println("Checking OS thread priorities for pool threads...");
    #ifdef __linux__
            sync_out.println("NOTE: On Linux, this test must be run as root, otherwise it will fail!");
    #endif
            const std::vector<BS::os_thread_priority> priorities = {BS::os_thread_priority::realtime, BS::os_thread_priority::highest, BS::os_thread_priority::above_normal, BS::os_thread_priority::normal, BS::os_thread_priority::below_normal, BS::os_thread_priority::lowest, BS::os_thread_priority::idle};
            for (BS::os_thread_priority priority : priorities)
            {
                sync_out.print("Setting OS thread priority to ", os_thread_priority_name(priority), ' ');
            // On Windows we should be able to set all the priorities even as non-admin, including realtime. On Linux, only root can increase the priority beyond normal. (Also, note that on WSL, even root cannot set the priority to highest or above.)
    #ifdef _WIN32
                check(BS::this_thread::set_os_thread_priority(priority));
    #else
                if (priority <= BS::os_thread_priority::normal)
                    check(BS::this_thread::set_os_thread_priority(priority));
                else
                    check_root(BS::this_thread::set_os_thread_priority(priority));
    #endif
                const std::optional<BS::os_thread_priority> new_priority = BS::this_thread::get_os_thread_priority();
                sync_out.print("Obtaining new OS thread priority ");
                check(new_priority.has_value());
    #ifdef _WIN32
                check(os_thread_priority_name(priority), os_thread_priority_name(new_priority));
    #else
                check_root(os_thread_priority_name(priority), os_thread_priority_name(new_priority));
    #endif
            }
            // Set the priority back to normal after the test ends. This will fail on Linux/macOS if not running as root.
            sync_out.println("Setting priority back to normal...");
    #ifdef _WIN32
            check(BS::this_thread::set_os_thread_priority(BS::os_thread_priority::normal));
    #else
            check_root(BS::this_thread::set_os_thread_priority(BS::os_thread_priority::normal));
    #endif
        });
}

/**
 * @brief Check that getting and setting OS thread names works.
 */
void check_os_thread_names()
{
    sync_out.println("Checking OS thread names...");
    const std::string name = "BS_thread_pool";
    sync_out.println("Setting main thread name to \"", name, "\"...");
    check(BS::this_thread::set_os_thread_name(name));
    sync_out.println("Obtaining new OS thread name...");
    std::optional<std::string> new_name = BS::this_thread::get_os_thread_name();
    if (new_name.has_value())
    {
        check(true);
        check(name, *new_name);
    }
    else
    {
        check(false);
    }
}

/**
 * @brief Convert a `std::vector<bool>` representing CPU affinity to a string of 0s and 1s.
 *
 * @param affinity The affinity.
 * @return The string.
 */
std::string affinity_to_string(const std::optional<std::vector<bool>>& affinity)
{
    if (affinity.has_value())
    {
        const std::size_t num_bits = affinity->size();
        std::string str(num_bits, ' ');
        for (std::size_t i = 0; i < num_bits; ++i)
            str[num_bits - i - 1] = (*affinity)[i] ? '1' : '0';
        return str;
    }
    return "N/A";
}

/**
 * @brief Check that getting and setting OS process affinity works.
 */
void check_os_process_affinity()
{
    sync_out.println("Checking OS process affinity...");

    sync_out.print("Obtaining initial process affinity ");
    const std::optional<std::vector<bool>> initial_affinity = BS::get_os_process_affinity();
    check(initial_affinity.has_value());
    sync_out.println("Initial affinity is: ", affinity_to_string(initial_affinity));
    const std::size_t num_bits = initial_affinity.has_value() ? initial_affinity->size() : std::thread::hardware_concurrency();

    sync_out.print("Setting affinity to CPU 1 only ");
    std::vector<bool> cpu_1_in(num_bits, false);
    cpu_1_in[0] = true;
    check(BS::set_os_process_affinity(cpu_1_in));
    sync_out.print("Obtaining new affinity ");
    const std::optional<std::vector<bool>> cpu_1_out = BS::get_os_process_affinity();
    check(cpu_1_out.has_value());
    check(affinity_to_string(cpu_1_in), affinity_to_string(cpu_1_out));

    sync_out.print("Setting affinity to alternating CPUs ");
    std::vector<bool> alternating_in(num_bits, false);
    for (std::size_t i = 0; i < num_bits; ++i)
        alternating_in[i] = (i % 2 == 1);
    check(BS::set_os_process_affinity(alternating_in));
    sync_out.print("Obtaining new affinity ");
    const std::optional<std::vector<bool>> alternating_out = BS::get_os_process_affinity();
    check(alternating_out.has_value());
    check(affinity_to_string(alternating_in), affinity_to_string(alternating_out));

    if (initial_affinity.has_value())
    {
        sync_out.print("Setting affinity back to initial value ");
        check(BS::set_os_process_affinity(*initial_affinity));
        sync_out.print("Obtaining new affinity ");
        const std::optional<std::vector<bool>> initial_out = BS::get_os_process_affinity();
        check(initial_out.has_value());
        check(affinity_to_string(initial_affinity), affinity_to_string(initial_out));
    }
}

/**
 * @brief Check that getting and setting OS thread affinity works.
 */
void check_os_thread_affinity()
{
    BS::thread_pool pool;
    pool.detach_task(
        []
        {
            // Since the thread affinity must be a subset of the process affinity, we first set its affinity to all CPUs if it wasn't already.
            const std::optional<std::vector<bool>> initial_process_affinity = BS::get_os_process_affinity();
            const std::size_t num_process_bits = initial_process_affinity.has_value() ? initial_process_affinity->size() : std::thread::hardware_concurrency();
            const std::vector<bool> all_enabled(num_process_bits, true);
            BS::set_os_process_affinity(all_enabled);

            sync_out.println("Checking OS thread affinity for pool threads...");

            sync_out.print("Obtaining initial thread affinity ");
            const std::optional<std::vector<bool>> initial_affinity = BS::this_thread::get_os_thread_affinity();
            check(initial_affinity.has_value());
            sync_out.println("Initial affinity is: ", affinity_to_string(initial_affinity));
            const std::size_t num_bits = initial_affinity.has_value() ? initial_affinity->size() : std::thread::hardware_concurrency();

            sync_out.print("Setting affinity to CPU 1 only ");
            std::vector<bool> cpu_1_in(num_bits, false);
            cpu_1_in[0] = true;
            check(BS::this_thread::set_os_thread_affinity(cpu_1_in));
            sync_out.print("Obtaining new affinity ");
            const std::optional<std::vector<bool>> cpu_1_out = BS::this_thread::get_os_thread_affinity();
            check(cpu_1_out.has_value());
            check(affinity_to_string(cpu_1_in), affinity_to_string(cpu_1_out));

            sync_out.print("Setting affinity to alternating CPUs ");
            std::vector<bool> alternating_in(num_bits, false);
            for (std::size_t i = 0; i < num_bits; ++i)
                alternating_in[i] = (i % 2 == 1);
            check(BS::this_thread::set_os_thread_affinity(alternating_in));
            sync_out.print("Obtaining new affinity ");
            const std::optional<std::vector<bool>> alternating_out = BS::this_thread::get_os_thread_affinity();
            check(alternating_out.has_value());
            check(affinity_to_string(alternating_in), affinity_to_string(alternating_out));

            if (initial_affinity.has_value())
            {
                sync_out.print("Setting affinity back to initial value ");
                check(BS::this_thread::set_os_thread_affinity(*initial_affinity));
                sync_out.print("Obtaining new affinity ");
                const std::optional<std::vector<bool>> initial_out = BS::this_thread::get_os_thread_affinity();
                check(initial_out.has_value());
                check(affinity_to_string(initial_affinity), affinity_to_string(initial_out));
            }

            if (initial_process_affinity.has_value())
                BS::set_os_process_affinity(*initial_process_affinity);
        });
}

/**
 * @brief Try to set the OS priority of this thread to the highest possible value. Also set the name of the thread for debugging purposes.
 */
void try_os_thread_priority()
{
    if (!BS::this_thread::set_os_thread_priority(BS::os_thread_priority::realtime))
        if (!BS::this_thread::set_os_thread_priority(BS::os_thread_priority::highest))
            BS::this_thread::set_os_thread_priority(BS::os_thread_priority::above_normal);
    std::optional<std::size_t> idx = BS::this_thread::get_index();
    if (idx.has_value())
        BS::this_thread::set_os_thread_name(make_string("Benchmark #", *idx));
    else
        BS::this_thread::set_os_thread_name("Benchmark main");
}
#endif

// ========================
// Functions for benchmarks
// ========================

/**
 * @brief A struct to store the mean and standard deviation of the results of a test.
 */
struct [[nodiscard]] mean_sd
{
    mean_sd(const double mean_, const double sd_) : mean(mean_), sd(sd_) {}

    double mean = 0;
    double sd = 0;
};

/**
 * @brief Print the timing of a specific test.
 *
 * @param stats A struct containing the mean and standard deviation.
 * @param pixels_per_ms The number of pixels per millisecond.
 */
void print_timing(const mean_sd& stats, const double pixels_per_ms)
{
    constexpr int width_mean = 6;
    constexpr int width_sd = 4;
    constexpr int width_pms = 7;
    sync_out.println("-> Mean: ", std::setw(width_mean), stats.mean, " ms, standard deviation: ", std::setw(width_sd), stats.sd, " ms, speed: ", std::setw(width_pms), pixels_per_ms, " pixels/ms.");
}

/**
 * @brief Find the index of the minimum element in a vector.
 *
 * @tparam T The type of elements in the vector.
 * @param vec The vector.
 * @return The index of the smallest element in the vector.
 */
template <typename T>
std::size_t min_element_index(const std::vector<T>& vec)
{
    return static_cast<std::size_t>(std::distance(vec.begin(), std::min_element(vec.begin(), vec.end())));
}

/**
 * @brief Calculate and print the speedup obtained by multithreading.
 *
 * @param timings A vector of the timings corresponding to different numbers of tasks.
 */
void print_speedup(const std::vector<double>& timings, const std::vector<std::size_t>& try_tasks)
{
    const std::size_t min_el = min_element_index(timings);
    const double max_speedup = std::round((timings[0] / timings[min_el]) * 10) / 10;
    const std::size_t num_tasks = try_tasks[min_el];
    sync_out.println("Maximum speedup obtained by multithreading vs. single-threading: ", max_speedup, "x, using ", num_tasks, " tasks.");
}

/**
 * @brief Calculate the mean and standard deviation of a set of integers.
 *
 * @param timings The integers.
 * @return A struct containing the mean and standard deviation.
 */
mean_sd analyze(const std::vector<std::chrono::milliseconds::rep>& timings)
{
    double mean = 0;
    for (std::size_t i = 0; i < timings.size(); ++i)
        mean += static_cast<double>(timings[i]) / static_cast<double>(timings.size());
    double variance = 0;
    for (std::size_t i = 0; i < timings.size(); ++i)
        variance += (static_cast<double>(timings[i]) - mean) * (static_cast<double>(timings[i]) - mean) / static_cast<double>(timings.size());
    return {mean, std::sqrt(variance)};
}

/**
 * @brief A class to save the Mandelbrot image in. Note that rows and columns are inverted compared to the usual matrix syntax, so that `image(x, y)` corresponds to the pixel at coordinates (x, y) where x is the horizontal axis (i.e. column number) and y is the vertical axis (i.e. row number). The width is the number of columns and the height is the number of rows.
 */
template <typename T>
class [[nodiscard]] image_matrix
{
public:
    image_matrix() = default;

    image_matrix(const std::size_t width_, const std::size_t height_) : width(width_), height(height_), pixels(std::make_unique<T[]>(width_ * height_)) {}

    [[nodiscard]] T& operator()(std::size_t x, std::size_t y)
    {
        return pixels[(y * width) + x];
    }

    [[nodiscard]] T operator()(std::size_t x, std::size_t y) const
    {
        return pixels[(y * width) + x];
    }

    [[nodiscard]] T& operator[](std::size_t i)
    {
        return pixels[i];
    }

    [[nodiscard]] T operator[](std::size_t i) const
    {
        return pixels[i];
    }

    [[nodiscard]] std::size_t get_height() const
    {
        return height;
    }

    [[nodiscard]] std::size_t get_width() const
    {
        return width;
    }

private:
    std::size_t width = 0;
    std::size_t height = 0;
    std::unique_ptr<T[]> pixels = nullptr;
}; // class matrix

// The maximum number of iterations to try before deciding whether a point is in the Mandelbrot set.
constexpr std::size_t max_iter = 2000;

/**
 * @brief Find the escape time of a point.
 *
 * @param c The point.
 * @return The escape time, that is, the number of iterations before the point escapes the Mandelbrot set, with an additional fractional part to eliminate color banding; or `max_iter` if the point doesn't escape within the maximum number of iterations.
 */
double mandelbrot_escape(const std::complex<double> c)
{
    // Define the escape radius. A point c is considered to have "escaped" the Mandelbrot set if, after fewer than `max_iter` iterations of the formula z = z^2 + c starting at z = 0, we get |z| > r. Since the Mandelbrot set is contained within a closed disk of radius 2, the escape radius must be at least 2. However, with that choice we will see the actual disk in the image, because any point outside the disk (but still in the output image) will automatically have an iteration count of 1. For the region plotted by this program by default, an escape radius of 4 is enough, but a higher radius generally produces smoother color gradients.
    constexpr double r = 1024;
    std::complex<double> z = c;
    std::size_t iter = 1;
    while (std::norm(z) <= (r * r) && iter < max_iter)
    {
        z = z * z + c;
        ++iter;
    };
    // If the point did not escape within the maximum number of iterations, then it is (most likely) in the Mandelbrot set, and we return the maximum number of iterations as is.
    if (iter == max_iter)
        return static_cast<double>(max_iter);
    // If the point escapes, calculate a continuous value to be used for coloring that points in the image. The iteration count is an integer, which would cause color banding in the final image, as there are large regions with the same iteration count and therefore the same color. We resolve this by adding a fractional part. After the loop ends, z has just escaped the radius r, so we are guaranteed that |r| < |z| < |r^2 + c|. Neglecting c (which we can do if r is large enough), this means log_r(|z|) is in the range [1, 2], and therefore log_2(log_r(|z|)) is in the range [0, 1]. Hence, the quantity log_2(log_r(|z|)) = log_2(log(|z|)/log(r)) = log_2(log(|z|^2)/log(r^2)) provides a fractional part that we can simply add to the integer iteration count to make it continuous and eliminate the banding. We subtract from the iteration count instead of adding to it, because larger values of z have smaller iteration counts.
    return static_cast<double>(iter) - std::log2(std::log(std::norm(z)) / std::log(r * r));
}

/**
 * @brief A helper struct to store the RGB values of a pixel.
 */
struct [[nodiscard]] color
{
    color() = default;

    template <typename T>
    color(const T r_, const T g_, const T b_) : r(static_cast<std::uint8_t>(r_)), g(static_cast<std::uint8_t>(g_)), b(static_cast<std::uint8_t>(b_))
    {
    }

    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;
};

/**
 * @brief Interpolate between two colors.
 *
 * @param first The first color.
 * @param second The second color.
 * @param t The interpolation point, in the range [0, 1] where 0 corresponds to the first color, 1 corresponds to the second color, and any other value combines the two colors.
 * @return The interpolated color.
 */
color interpolate_colors(const color& first, const color& second, const double t)
{
    return {first.r + (t * (second.r - first.r)), first.g + (t * (second.g - first.g)), first.b + (t * (second.b - first.b))};
}

/**
 * @brief Convert the escape time of a point into a color.
 *
 * @param iterations The fractional number of iterations before the point escapes the Mandelbrot set.
 * @return The color.
 */
color iter_to_color(const double iterations)
{
    // Define a nice color pallette for the image.
    static const std::vector<color> palette = {{66, 30, 15}, {25, 7, 26}, {9, 1, 47}, {4, 4, 73}, {0, 7, 100}, {12, 44, 138}, {24, 82, 177}, {57, 125, 209}, {134, 181, 229}, {211, 236, 248}, {241, 233, 191}, {248, 201, 95}, {255, 170, 0}, {204, 128, 0}, {153, 87, 0}, {106, 52, 3}};

    // Points that are in the set (or at least, suspected to be in the set because they did not diverge after the maximum number of iterations) will be black.
    if (iterations == max_iter)
        return {0, 0, 0};

    // Get the integer and fractional parts of the number of iterations.
    double int_part = 0;
    const double frac_part = std::modf(iterations, &int_part);

    // Choose two adjacent colors from the palette based on the integer part. We cycle through the palette, so the same colors will repeat many times (`max_iter` is much larger than the number of colors).
    const color color1 = palette[static_cast<std::size_t>(int_part) % palette.size()];
    const color color2 = palette[(static_cast<std::size_t>(int_part) + 1) % palette.size()];
    // Use the fractional part to interpolate smoothly between the two colors.
    return interpolate_colors(color1, color2, frac_part);
}

/**
 * @brief Calculate the colors of a range of pixels in an image, enumerated as a range of indices in a 1-dimensional array containing the flattened matrix in row-major order.
 *
 * @param image The matrix storing the image.
 * @param start The first index to calculate.
 * @param end The index after the last index to calculate.
 * @param jump How many pixels to jump over each iteration, to allow for splitting the work between different runs of the same test.
 * @param offset How many pixels to shift the calculation by.
 */
void calculate_mandelbrot(image_matrix<color>& image, const std::size_t start, const std::size_t end, const std::size_t jump, const std::size_t offset)
{
    // Define the ranges of real and imaginary values to consider for the Mandelbrot set. The aspect ratio should be exactly 1:1 (width:height) to prevent stretching, since the benchmark always outputs square images for simplicity.
    constexpr double re_min = -2.01;
    constexpr double re_max = 0.51;
    constexpr double im_min = -1.26;
    constexpr double im_max = 1.26;

    // Get the width and height of the image.
    const std::size_t width = image.get_width();
    const std::size_t height = image.get_height();

    for (std::size_t i = start + offset; i < end; i += jump)
    {
        // Convert the pixel index to the corresponding x and y coordinates.
        const std::size_t x = i % width;
        const std::size_t y = i / width;
        // Convert the pixel coordinates, integers (x, y) such that x is in [0, width-1] and y is in [0, height-1], to a complex number c such that Re(c) is in [re_min, re_max] and Im(c) is in [im_min, im_max]}. (Note: We also need to invert the y axis because the y value increases downwards in the image, but the imaginary part increases upwards in the complex plane. However, to avoid doing any extra calculations, we do that later when we save the image.)
        const std::complex<double> c = {(static_cast<double>(x) / static_cast<double>(width) * (re_max - re_min)) + re_min, (static_cast<double>(y) / static_cast<double>(height) * (im_max - im_min)) + im_min};
        // Calculate the pixel's escape time and convert it to a color.
        image[i] = iter_to_color(mandelbrot_escape(c));
    }
}

// A macro to unpack a 16-bit integer into 2 bytes.
#define UNPACK_2_BYTES(value) static_cast<std::uint8_t>(value), static_cast<std::uint8_t>((value) >> 8)
// A macro to unpack a 32-bit integer into 4 bytes.
#define UNPACK_4_BYTES(value) static_cast<std::uint8_t>(value), static_cast<std::uint8_t>((value) >> 8), static_cast<std::uint8_t>((value) >> 16), static_cast<std::uint8_t>((value) >> 24)

/**
 * @brief Save an image to a BMP file.
 *
 * @param image The matrix containing the pixels.
 * @param filename The output file name.
 * @return `true` if the file was saved successfully, `false` otherwise.
 */
void save_bmp(const image_matrix<color>& image, const std::string& filename)
{
    // Create the file.
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        sync_out.println("Error: Could not create the file ", filename, '.');
        return;
    }

    sync_out.print("Saving image to a BMP file: [");

    // Calculate the size of the BMP file in bytes.
    const std::uint32_t width = static_cast<std::uint32_t>(image.get_width());
    const std::uint32_t height = static_cast<std::uint32_t>(image.get_height());
    const std::uint32_t total_pixels = width * height;
    constexpr std::uint32_t file_header_size = 14;
    constexpr std::uint32_t info_header_size = 40;
    constexpr std::uint32_t bytes_per_pixel = 3;
    constexpr std::uint32_t bits_per_pixel = bytes_per_pixel * 8;
    const std::uint32_t file_size = file_header_size + info_header_size + (bytes_per_pixel * total_pixels);

    // The file header of the BMP file: 2 bytes for the "BM" signature, 4 bytes for the file size, 4 bytes reserved, 4 bytes for the start offset of the pixel array. Note that all integers are stored in little-endian format (least-significant byte first), hence the bit shifts (from the macro UNPACK_4_BYTES). We specify the values explicitly to avoid issues with padding.
    const std::uint8_t bmp_file_header[file_header_size] = {'B', 'M', UNPACK_4_BYTES(file_size), UNPACK_4_BYTES(0), UNPACK_4_BYTES(file_header_size + info_header_size)};

    // The information header of the BMP file: 4 bytes for the header size, 4 bytes for the image width, 4 bytes for the image height, 2 bytes for the number of color planes, 2 bytes for the number of bits per pixel, 4 bytes for the compression method (0 = no compression), 4 bytes for the image size (can be 0 if no compression), 4 bytes for the horizontal pixels per meter, 4 bytes for the vertical pixels per meter, 4 bytes for the number of colors (0 = default), 4 bytes for the number of "important colors" (generally ignored).
    const std::uint8_t bmp_info_header[info_header_size] = {UNPACK_4_BYTES(info_header_size), UNPACK_4_BYTES(width), UNPACK_4_BYTES(height), UNPACK_2_BYTES(1), UNPACK_2_BYTES(bits_per_pixel), UNPACK_4_BYTES(0), UNPACK_4_BYTES(0), UNPACK_4_BYTES(0), UNPACK_4_BYTES(0), UNPACK_4_BYTES(0), UNPACK_4_BYTES(0)};

    // Write the headers.
    file.write(reinterpret_cast<const char*>(bmp_file_header), file_header_size);
    file.write(reinterpret_cast<const char*>(bmp_info_header), info_header_size);

    // Create padding bytes for later use.
    const std::uint8_t padding_bytes[3] = {0, 0, 0};
    const std::streamsize num_padding_bytes = (4 - ((width * bytes_per_pixel) % 4)) % 4;

    // Write the pixels. Note that they are stored "bottom-up", starting in the lower left corner, going from left to right and then row by row. However, we need to invert the y axis anyway, because the y value increases downwards in the image, but the imaginary part increases upwards in the complex plane. Therefore, we just use the normal y values when saving the image.
    for (std::size_t y = 0; y < height; ++y)
    {
        for (std::size_t x = 0; x < width; ++x)
        {
            const color col = image(x, y);
            // BMP format stores the colors in BGR order.
            file.write(reinterpret_cast<const char*>(&col.b), 1);
            file.write(reinterpret_cast<const char*>(&col.g), 1);
            file.write(reinterpret_cast<const char*>(&col.r), 1);
        }
        if (num_padding_bytes != 0)
        {
            // BMP format requires that each row is a multiple of 4 bytes long, so we add padding if necessary.
            file.write(reinterpret_cast<const char*>(padding_bytes), num_padding_bytes);
        }
        if (y % (height / 10) == 0)
            sync_out.print('.');
    }

    file.close();
    sync_out.println("]\nMandelbrot image saved successfully as ", filename, '.');
}

/**
 * @brief A utility class to measure execution time for benchmarking purposes.
 */
class [[nodiscard]] timer
{
public:
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

/**
 * @brief Benchmark multithreaded performance by calculating the Mandelbrot set.
 *
 * @param benchmark Whether to perform the full benchmarks.
 * @param plot Whether to perform quick benchmarks by just plotting the image once.
 * @param save Whether to save the image as a BMP file.
 */
void check_performance(const bool benchmark, const bool plot, const bool save)
{
    print_header("Preparing benchmarks:");

#ifdef BS_THREAD_POOL_NATIVE_EXTENSIONS
    // Try to give the process the highest possible priority, so that other processes do not interfere with the benchmarks.
    if (!BS::set_os_process_priority(BS::os_process_priority::realtime))
        if (!BS::set_os_process_priority(BS::os_process_priority::high))
            BS::set_os_process_priority(BS::os_process_priority::above_normal);

    const std::string process_priority = os_process_priority_name(BS::get_os_process_priority());
    sync_out.println("Process priority set to: ", process_priority, ".");
    if (process_priority != "realtime")
        sync_out.println("Note: Please run as admin/root to enable a higher process priority.");

    try_os_thread_priority();
    const std::string thread_priority = os_thread_priority_name(BS::this_thread::get_os_thread_priority());
    sync_out.println("Thread priority set to: ", thread_priority, ".");
    if (thread_priority != "realtime")
        sync_out.println("Note: Please run as admin/root to enable a higher thread priority.");

    // Initialize a thread pool with the default number of threads, and ensure that the threads have the highest possible priority, so that other processes do not interfere with the benchmarks.
    BS::thread_pool pool(try_os_thread_priority);
#else
    // If native extensions are disabled, just initialize a thread pool with the default number of threads.
    BS::thread_pool pool;
#endif

    // Store the number of available hardware threads for easy access.
    const std::size_t thread_count = pool.get_thread_count();
    sync_out.println("Using ", thread_count, " threads.");

    // Set the formatting of floating point numbers.
    sync_out.print(std::fixed, std::setprecision(1));

    // Initialize a timer object to measure execution time.
    timer tmr;

    // The target execution time, in milliseconds, of the multithreaded test with the number of blocks equal to the number of threads. The total time spent on that test will be approximately equal to `repeat * target_ms`.
    constexpr std::chrono::milliseconds::rep target_ms = 50;

    // Find the Mandelbrot image size that will roughly achieve the target execution time.
    sync_out.println("Determining the Mandelbrot image size needed to achieve an approximate mean execution time of ", target_ms, " ms with ", thread_count, " tasks...");
    std::size_t image_size = thread_count;
    image_matrix<color> image;
    std::size_t jump = 1;
    std::size_t offset = 0;

    // Define the loop function.
    const auto loop = [&image, &jump, &offset](const std::size_t start, const std::size_t end)
    {
        calculate_mandelbrot(image, start, end, jump, offset);
    };

    // Increase the image size gradually until the target execution time is reached.
    do
    {
        image_size *= 2;
        image = image_matrix<color>(image_size, image_size);
        tmr.start();
        pool.detach_blocks(0, image_size * image_size, loop);
        pool.wait();
        tmr.stop();
    } while (tmr.ms() < target_ms);

    // Scale the image size to fit the target execution time more precisely, keeping in mind that the time complexity is O(image_size^2).
    image_size = static_cast<std::size_t>(std::llround(static_cast<double>(image_size) * std::sqrt(static_cast<double>(target_ms) / static_cast<double>(tmr.ms()))));
    sync_out.println("Result: ", image_size, 'x', image_size, " pixels.");

    if (benchmark)
    {
        print_header("Performing full benchmarks:");
        // Define vectors to store statistics.
        std::vector<double> different_n_timings;
        std::vector<std::chrono::milliseconds::rep> same_n_timings;

        // The number of times to repeat each run of the test in order to collect reliable statistics.
        constexpr std::size_t num_repeats = 30;

        // Since we are repeating the same test multiple times, we might as well use different parts of the complex plane in each repetition. However, we have to spread the calculations evenly to avoid biasing the results, as some regions have much higher escape times than others. So we calculate the whole image, but at an offset from 0 to `num_repeats`.
        jump = num_repeats;
        const std::size_t benchmark_image_size = static_cast<std::size_t>(std::floor(static_cast<double>(image_size) * std::sqrt(num_repeats)));
        sync_out.println("Generating a ", benchmark_image_size, 'x', benchmark_image_size, " plot of the Mandelbrot set...");
        sync_out.println("Each test will be repeated ", num_repeats, " times to collect reliable statistics.");

        // Perform the test.
        std::vector<std::size_t> try_tasks;
        std::size_t num_tasks = 0;
        double last_timing = std::numeric_limits<double>::max();
        constexpr int width_tasks = 4;
        while (true)
        {
            image = image_matrix<color>(benchmark_image_size, benchmark_image_size);
            try_tasks.push_back(num_tasks);
            if (num_tasks == 0)
                sync_out.print(std::setw(width_tasks), 1, " task:  ");
            else
                sync_out.print(std::setw(width_tasks), num_tasks, " tasks: ");
            sync_out.print('[');
            for (std::size_t i = 0; i < num_repeats; ++i)
            {
                // Measure execution time for this test.
                tmr.start();
                if (num_tasks > 0)
                {
                    pool.detach_blocks(0, benchmark_image_size * benchmark_image_size, loop, num_tasks);
                    pool.wait();
                }
                else
                {
                    loop(0, benchmark_image_size * benchmark_image_size);
                }
                tmr.stop();
                // Save the measurement for later analysis.
                same_n_timings.push_back(tmr.ms());
                // Print a dot to inform the user that we've made progress.
                sync_out.print('.');
                // Increase the offset so we calculate a different part of the image in each repetition of the test.
                offset = (offset + 1) % num_repeats;
            }
            sync_out.println(']', (num_tasks == 0) ? "  (single-threaded)" : "");
            // Analyze, print, and save the mean and standard deviation of all the tests with the same number of tasks.
            const mean_sd stats = analyze(same_n_timings);
            const std::chrono::milliseconds::rep total_time = std::reduce(same_n_timings.begin(), same_n_timings.end());
            const double pixels_per_ms = static_cast<double>(benchmark_image_size * benchmark_image_size) / static_cast<double>(total_time);
            same_n_timings.clear();
            print_timing(stats, pixels_per_ms);
            different_n_timings.push_back(stats.mean);
            if (num_tasks == 0)
            {
                num_tasks = std::max<std::size_t>(thread_count / 4, 2);
            }
            else
            {
                if ((num_tasks > thread_count) && (stats.mean > last_timing))
                    break;
                last_timing = stats.mean;
                num_tasks *= 2;
            }
        }
        print_speedup(different_n_timings, try_tasks);
    }

    if (plot)
    {
        print_header("Performing quick benchmarks:");
        // Just plot whatever we can in 5 seconds. Feel free to increase this to get higher resolution images.
        constexpr std::chrono::milliseconds::rep total_ms = 5000;
        const std::size_t plot_image_size = static_cast<std::size_t>(std::floor(static_cast<double>(image_size) * std::sqrt(static_cast<double>(total_ms) / static_cast<double>(target_ms))));
        image = image_matrix<color>(plot_image_size, plot_image_size);
        sync_out.print("Generating a ", plot_image_size, 'x', plot_image_size, " plot of the Mandelbrot set with ", thread_count, " tasks: [");
        pool.detach_blocks(0, plot_image_size * plot_image_size,
            [&image](const std::size_t start, const std::size_t end)
            {
                calculate_mandelbrot(image, start, end, 1, 0);
                sync_out.print('.');
            });
        pool.wait();
        tmr.stop();
        sync_out.println("]\nDone in ", tmr.ms(), " ms (", static_cast<double>(plot_image_size * plot_image_size) / static_cast<double>(tmr.ms()), " pixels/ms).");
    }

    if (save)
        save_bmp(image, "BS_thread_pool_benchmark_mandelbrot.bmp");

    print_header("Thread pool performance test completed!", '+');
}

// ==================================
// The main function and related code
// ==================================

/**
 * @brief Show basic information about the program.
 */
void show_intro()
{
    sync_out.println(R"(
██████  ███████       ████████ ██   ██ ██████  ███████  █████  ██████          ██████   ██████   ██████  ██
██   ██ ██      ██ ██    ██    ██   ██ ██   ██ ██      ██   ██ ██   ██         ██   ██ ██    ██ ██    ██ ██
██████  ███████          ██    ███████ ██████  █████   ███████ ██   ██         ██████  ██    ██ ██    ██ ██
██   ██      ██ ██ ██    ██    ██   ██ ██   ██ ██      ██   ██ ██   ██         ██      ██    ██ ██    ██ ██
██████  ███████          ██    ██   ██ ██   ██ ███████ ██   ██ ██████  ███████ ██       ██████   ██████  ███████
)");

    sync_out.println("BS::thread_pool: a fast, lightweight, modern, and easy-to-use C++17/C++20/C++23 thread pool library");
    sync_out.println("(c) 2024 Barak Shoshany (baraksh@gmail.com) (https://baraksh.com/)");
    sync_out.println("GitHub: https://github.com/bshoshany/thread-pool");
    sync_out.println();

    sync_out.println("Thread pool library version is v", BS::thread_pool_version, '.');
    sync_out.println("Thread pool library imported using: ", BS::thread_pool_module ? "import BS.thread_pool (" : "#include \"BS_thread_pool.hpp\" (no ", "C++20 modules).");
    sync_out.println();
    sync_out.println("C++ Standard Library imported using:");
    sync_out.println("* Thread pool library: ", BS::thread_pool_import_std ? "import std (" : "#include <...> (no ", "C++23 std module).");
    sync_out.println("* Test program: ", using_import_std ? "import std (" : "#include <...> (no ", "C++23 std module).");
    sync_out.println();

    sync_out.println("Native extensions are ", BS::thread_pool_native_extensions ? "enabled" : "disabled", '.');

    sync_out.println();

    sync_out.println("Detected OS: ", detect_os(), '.');
    sync_out.println("Detected compiler: ", detect_compiler(), '.');
    sync_out.println("Detected standard library: ", detect_lib(), '.');
    sync_out.println("Detected C++ standard: ", detect_cpp_standard(), '.');
    sync_out.println("Detected features:");
    print_features();

    sync_out.println("Hardware concurrency is ", std::thread::hardware_concurrency(), '.');
    sync_out.println("Important: Please do not run any other applications, especially multithreaded applications, in parallel with this test!");
}

/**
 * @brief Get a string representing the current time.
 *
 * @return The string.
 */
std::string get_time()
{
#ifdef __cpp_lib_format
    // Things are much easier with C++20 `std::format`.
    return std::format("{:%Y-%m-%d_%H.%M.%S}", std::chrono::time_point_cast<std::chrono::seconds>(std::chrono::system_clock::now()));
#else
    std::string time_string = "YYYY-MM-DD_HH.MM.SS";
    std::tm local_tm = {};
    const std::time_t epoch = std::time(nullptr);
    #if defined(_MSC_VER) && !defined(__cpp_lib_modules)
    // If MSVC is detected, use `localtime_s()` to avoid warning C4996. (This doesn't work if we used `import std`, so we check that to be on the safe side, although in that case `std::format` should be available anyway).
    if (localtime_s(&local_tm, &epoch) != 0)
        return "";
    #elif defined(__linux__) || defined(__APPLE__)
    // On Linux or macOS, use `localtime_r()` to avoid clang-tidy warning `concurrency-mt-unsafe`.
    if (localtime_r(&epoch, &local_tm) == nullptr)
        return "";
    #else
    local_tm = *std::localtime(&epoch);
    #endif
    const std::size_t bytes = std::strftime(time_string.data(), time_string.length() + 1, "%Y-%m-%d_%H.%M.%S", &local_tm);
    if (bytes != time_string.length())
        return "";
    return time_string;
#endif
}

/**
 * @brief A class to parse command line arguments. All arguments are assumed to be on/off and default to off.
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
    arg_parser(int argc, char* argv[]) : args(argv + 1, argv + argc), executable(argv[0]) {};

    /**
     * @brief Check if a specific command line argument has been passed to the program. If no arguments were passed, use the default value instead.
     *
     * @param arg The argument to check for.
     * @return `true` if the argument exists, `false` otherwise.
     */
    [[nodiscard]] bool operator[](const std::string_view arg)
    {
        if (size() > 0)
            return (args.count(arg) == 1);
        return allowed[arg].def;
    }

    /**
     * @brief Add an argument to the list of allowed arguments.
     *
     * @param arg The argument.
     * @param desc The description of the argument.
     * @param def The default value of the argument.
     */
    void add_argument(const std::string_view arg, const std::string_view desc, const bool def)
    {
        allowed[arg] = {desc, def};
    }

    /**
     * @brief Get the name of the executable.
     *
     * @return The name of the executable.
     */
    std::string_view get_executable()
    {
        return executable;
    }

    void show_help() const
    {
        int width = 1;
        for (const auto& [arg, opt] : allowed)
            width = std::max(width, static_cast<int>(arg.size()));
        sync_out.println("\nAvailable options (all are on/off and default to off):");
        for (const auto& [arg, opt] : allowed)
            sync_out.println("  ", std::left, std::setw(width), arg, "  ", opt.desc);
        sync_out.print("If no options are entered, the default is:\n  ");
        for (const auto& [arg, opt] : allowed)
        {
            if (opt.def)
                sync_out.print(arg, " ");
        }
        sync_out.println();
    }

    /**
     * @brief Get the number of command line arguments.
     *
     * @return The number of arguments.
     */
    [[nodiscard]] std::size_t size() const
    {
        return args.size();
    }

    /**
     * @brief Verify that the command line arguments belong to the list of allowed arguments.
     *
     * @return `true` if all arguments are allowed, `false` otherwise.
     */
    [[nodiscard]] bool verify() const
    {
        return std::all_of(args.begin(), args.end(),
            [this](const std::string_view arg)
            {
                return allowed.count(arg) == 1;
            });
    }

private:
    struct arg_spec
    {
        std::string_view desc;
        bool def = false;
    };

    /**
     * @brief A set containing string views of the command line arguments.
     */
    std::set<std::string_view> args;

    /**
     * @brief A map containing the allowed arguments and their descriptions.
     */
    std::map<std::string_view, arg_spec> allowed;

    /**
     * @brief A string view containing the name of the executable.
     */
    std::string_view executable;
}; // class arg_parser

int main(int argc, char* argv[]) // NOLINT(bugprone-exception-escape)
{
#ifdef __cpp_exceptions
    try
    {
#endif
        // If the file default_args.txt exists, read the default arguments from it (space separated in a single line). Otherwise, use the built-in defaults. This is useful when debugging.
        std::map<std::string, bool> defaults;
        std::ifstream default_args_file("default_args.txt");
        if (default_args_file.is_open())
        {
            std::string line;
            std::getline(default_args_file, line);
            std::istringstream iss(line);
            std::string arg;
            while (iss >> arg)
                defaults[arg] = true;
            default_args_file.close();
        }
        else
        {
            defaults = {{"help", false}, {"stdout", true}, {"log", true}, {"tests", true}, {"deadlock", false}, {"benchmarks", true}, {"plot", false}, {"save", false}};
        }

        // Parse the command line arguments.
        arg_parser args(argc, argv);
        args.add_argument("help", "Show this help message and exit.", defaults["help"]);
        args.add_argument("stdout", "Print to the standard output.", defaults["stdout"]);
        args.add_argument("log", "Print to a log file.", defaults["log"]);
        args.add_argument("tests", "Perform standard tests.", defaults["tests"]);
        args.add_argument("deadlock", "Perform long deadlock tests.", defaults["deadlock"]);
        args.add_argument("benchmarks", "Perform full Mandelbrot plot benchmarks.", defaults["benchmarks"]);
        args.add_argument("plot", "Perform quick Mandelbrot plot benchmarks.", defaults["plot"]);
        args.add_argument("save", "Save the Mandelbrot plot to a file.", defaults["save"]);

        if (args.size() > 0)
        {
            if (args["help"] || !args.verify())
            {
                show_intro();
                args.show_help();
                return 0;
            }
            if (!args["stdout"] && !args["log"])
            {
                show_intro();
                args.show_help();
                sync_out.println("\nERROR: No output stream specified! Please enter one or more of: log, stdout. Aborting.");
                return 0;
            }
            if (!args["benchmarks"] && !args["deadlock"] && !args["plot"] && !args["tests"])
            {
                show_intro();
                args.show_help();
                sync_out.println("\nERROR: No tests or benchmarks requested! Please enter one or more of: benchmarks, deadlock, plot, tests. Aborting.");
                return 0;
            }
        }

        if (!args["stdout"])
            sync_out.remove_stream(std::cout);

        // A stream object used to access the log file.
        std::ofstream log_file;

        if (args["log"])
        {
            // Extract the name of the executable file, or use a default value if it is not available.
            const std::string_view executable = args.get_executable();
            const std::size_t last_slash = executable.find_last_of("/\\") + 1;
            std::string exe_file(executable.substr(last_slash, executable.find('.', last_slash) - last_slash));
            if (exe_file.empty())
                exe_file = "BS_thread_pool_test";
            // Create a log file using the name of the executable, followed by the current date and time.
            const std::string log_filename = exe_file + "-" + get_time() + ".log";
            log_file.open(log_filename);
            if (log_file.is_open())
            {
                sync_out.print("Generating log file: ", log_filename, ".\n");
                sync_out.add_stream(log_file);
            }
            else
            {
                sync_out.println("ERROR: Could not create a log file.");
                return 1;
            }
        }

        show_intro();

        if (args["tests"])
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

            print_header("Checking submission of different callable types:");
            check_callables();

            print_header("Checking wait(), wait_for(), and wait_until():");
            check_wait();
            check_wait_blocks();
            check_wait_for();
            check_wait_until();
            check_wait_multiple_deadlock();
#ifdef __cpp_exceptions
            check_wait_self_deadlock();

            print_header("Checking exception handling:");
            check_exceptions_submit();
            check_exceptions_multi_future();
#else
        print_header("NOTE: Exceptions are disabled, skipping wait deadlock check and exception handling tests.");
#endif

            print_header("Checking detach_loop() and submit_loop():");
            check_loop();

            print_header("Checking detach_blocks() and submit_blocks():");
            check_blocks();

            print_header("Checking detach_sequence() and submit_sequence():");
            check_sequence();

            print_header("Checking task monitoring:");
            check_task_monitoring();

            print_header("Checking pausing:");
            check_pausing();

            print_header("Checking purge():");
            check_purge();

            print_header("Checking parallelized vector operations:");
            check_vectors();

            print_header("Checking task priority:");
            check_priority();

            print_header("Checking thread initialization/cleanup functions and BS::this_thread:");
            check_init();
            check_cleanup();
            check_get_pool();

            print_header("Checking that parallelized tasks do not get copied:");
            check_copy_all();

            print_header("Checking that shared pointers are correctly shared:");
            check_shared_ptr_all();

            print_header("Checking that tasks are destructed immediately after running:");
            check_task_destruct();

            print_header("Checking BS::common_index_type:");
            check_common_index_type();

#ifdef BS_THREAD_POOL_NATIVE_EXTENSIONS
            print_header("Checking native extensions:");
    #ifndef _WIN32
            if ((args["benchmarks"] || args["plot"]) && !BS::set_os_process_priority(BS::os_process_priority::realtime))
            {
                sync_out.println("NOTE: Skipping process/thread priority checks since the test is running on Linux/macOS without root privileges and benchmarks are enabled. On Linux/macOS, if priorities are decreased, they cannot be increased back to normal without root privileges, so the process will be stuck on the lowest priority, and the benchmarks will be unreliable.\n");
            }
            else
    #endif
            {
                // Note: We have to check thread priorities first, because the check for process priorities lowers the priority of the process to the lowest level, and on Linux, if not running as root, we can only lower the priority, not raise it, so the process gets stuck on the lowest priority. Since the thread priorities cannot be set to higher than the process priorities, this means the thread priorities will also be stuck on the lowest priority, and the test will fail.
                check_os_thread_priorities();
                sync_out.println();
                check_os_process_priorities();
                sync_out.println();
            }
            check_os_thread_names();
            sync_out.println();
    #if defined(_WIN32) || defined(__linux__)
            check_os_thread_affinity();
            sync_out.println();
            check_os_process_affinity();
    #else
            sync_out.println("NOTE: macOS does not support affinity, skipping the corresponding test.");
    #endif
#else
        print_header("NOTE: Native extensions disabled, skipping the corresponding test.");
#endif
        }

        if (args["deadlock"])
        {
            print_header("Checking for deadlocks:");
            sync_out.println("Checking for destruction deadlocks...");
            check_deadlock(
                []
                {
                    BS::thread_pool temp_pool;
                    temp_pool.detach_task([] {});
                });
            sync_out.println("Checking for reset deadlocks...");
            BS::thread_pool temp_pool;
            check_deadlock(
                [&temp_pool]
                {
                    temp_pool.reset();
                });
        }

        if (test_results::tests_failed > 0)
        {
            print_header("FAILURE: Passed " + std::to_string(test_results::tests_succeeded) + " checks, but failed " + std::to_string(test_results::tests_failed) + "!", '+');
            sync_out.println("\nPlease submit a bug report at https://github.com/bshoshany/thread-pool/issues including the exact specifications of your system (OS, CPU, compiler, etc.) and the generated log file.");
            log_file.close();
            return static_cast<int>(test_results::tests_failed);
        }

        if (args["tests"])
            print_header("SUCCESS: Passed all " + std::to_string(test_results::tests_succeeded) + " checks!", '+');

        if (args["benchmarks"] || args["plot"])
            check_performance(args["benchmarks"], args["plot"], args["save"]);

        log_file.close();
        return 0;
#ifdef __cpp_exceptions
    }
    catch (const std::exception& e)
    {
        sync_out.println("ERROR: Tests failed due to exception: ", e.what());
        return 1;
    }
#endif
}
