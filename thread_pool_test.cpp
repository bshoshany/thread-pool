// Get rid of annoying MSVC warning.
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <fstream>
#include <iomanip>
#include <random>
#include <string>
#include <vector>

#include "thread_pool.hpp"

// Define short names for commonly-used integer types.
typedef std::int_fast32_t i32;
typedef std::uint_fast32_t ui32;
typedef std::int_fast64_t i64;
typedef std::uint_fast64_t ui64;

// Define two global synced_streams objects: one prints to std::cout and the other to a file.
synced_stream sync_cout(std::cout);
std::ofstream log_file;
synced_stream sync_file(log_file);

// A global thread pool object.
thread_pool pool;

// A global random_device object used to seed some random number generators.
std::random_device rd;

// Global variables to measure how many checks succeeded and how many failed.
ui32 tests_succeeded = 0;
ui32 tests_failed = 0;

/**
 * @brief Print any number of items into both std::cout and the log file, syncing both independently.
 *
 * @tparam T The types of the items.
 * @param items The items to print.
 */
template <typename... T>
void dual_print(const T &...items)
{
    sync_cout.print(items...);
    sync_file.print(items...);
}

/**
 * @brief Print any number of items into both std::cout and the log file, followed by a newline character, syncing both independently.
 *
 * @tparam T The types of the items.
 * @param items The items to print.
 */
template <typename... T>
void dual_println(const T &...items)
{
    dual_print(items..., '\n');
}

/**
 * @brief Print a stylized header.
 *
 * @param text The text of the header. Will appear between two lines.
 * @param symbol The symbol to use for the lines. Default is '='.
 */
void print_header(const std::string &text, const char &symbol = '=')
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
 * @brief Check if a condition is met, report the result, and count the number of successes and failures.
 *
 * @param condition The condition to check.
 */
void check(const bool condition)
{
    if (condition)
    {
        dual_println("-> PASSED!");
        tests_succeeded++;
    }
    else
    {
        dual_println("-> FAILED!");
        tests_failed++;
    }
}

/**
 * @brief Store the ID of the current thread in memory. Waits for a short time to ensure it does not get evaluated by more than one thread.
 *
 * @param location A pointer to the location where the thread ID should be stored.
 */
void store_ID(std::thread::id *location)
{
    *location = std::this_thread::get_id();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

/**
 * @brief Count the number of unique threads in the thread pool to ensure that the correct number of individual threads was created. Pushes a number of tasks equal to four times the thread count into the thread pool, and count the number of unique thread IDs returned by the tasks.
 */
ui32 count_unique_threads()
{
    std::vector<std::thread::id> thread_IDs(pool.get_thread_count() * 4);
    for (std::thread::id &id : thread_IDs)
        pool.push_task(store_ID, &id);
    pool.wait_for_tasks();
    std::sort(thread_IDs.begin(), thread_IDs.end());
    ui32 unique_threads = (ui32)(std::unique(thread_IDs.begin(), thread_IDs.end()) - thread_IDs.begin());
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

/**
 * @brief Check that push_task() works.
 */
void check_push_task()
{
    dual_println("Checking that push_task() works for a function with no arguments or return value...");
    {
        bool flag = false;
        pool.push_task([&flag]
                       { flag = true; });
        pool.wait_for_tasks();
        check(flag);
    }
    dual_println("Checking that push_task() works for a function with one argument and no return value...");
    {
        bool flag = false;
        pool.push_task([](bool *flag)
                       { *flag = true; },
                       &flag);
        pool.wait_for_tasks();
        check(flag);
    }
    dual_println("Checking that push_task() works for a function with two arguments and no return value...");
    {
        bool flag1 = false;
        bool flag2 = false;
        pool.push_task([](bool *flag1, bool *flag2)
                       { *flag1 = *flag2 = true; },
                       &flag1, &flag2);
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
        auto my_future = pool.submit([&flag]
                                     { flag = true; });
        check(my_future.get() && flag);
    }
    dual_println("Checking that submit() works for a function with one argument and no return value...");
    {
        bool flag = false;
        auto my_future = pool.submit([](bool *flag)
                                     { *flag = true; },
                                     &flag);
        check(my_future.get() && flag);
    }
    dual_println("Checking that submit() works for a function with two arguments and no return value...");
    {
        bool flag1 = false;
        bool flag2 = false;
        auto my_future = pool.submit([](bool *flag1, bool *flag2)
                                     { *flag1 = *flag2 = true; },
                                     &flag1, &flag2);
        check(my_future.get() && flag1 && flag2);
    }
    dual_println("Checking that submit() works for a function with no arguments and a return value...");
    {
        bool flag = false;
        auto my_future = pool.submit([&flag]
                                     {
                                         flag = true;
                                         return 42;
                                     });
        check(my_future.get() == 42 && flag);
    }
    dual_println("Checking that submit() works for a function with one argument and a return value...");
    {
        bool flag = false;
        auto my_future = pool.submit([](bool *flag)
                                     {
                                         *flag = true;
                                         return 42;
                                     },
                                     &flag);
        check(my_future.get() == 42 && flag);
    }
    dual_println("Checking that submit() works for a function with two arguments and a return value...");
    {
        bool flag1 = false;
        bool flag2 = false;
        auto my_future = pool.submit([](bool *flag1, bool *flag2)
                                     {
                                         *flag1 = *flag2 = true;
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
    ui32 n = pool.get_thread_count() * 10;
    std::vector<std::atomic<bool>> flags(n);
    for (ui32 i = 0; i < n; i++)
        pool.push_task([&flags, i]
                       {
                           std::this_thread::sleep_for(std::chrono::milliseconds(10));
                           flags[i] = true;
                       });
    pool.wait_for_tasks();
    bool all_flags = true;
    for (ui32 i = 0; i < n; i++)
        all_flags = all_flags && flags[i];
    check(all_flags);
}

/**
 * @brief Check that parallelize_loop() works for a specific number of indices split over a specific number of tasks.
 *
 * @param start The first index in the loop.
 * @param end The last index in the loop plus 1.
 * @param num_tasks The number of tasks.
 */
void check_parallelize_loop(i32 start, i32 end, const ui32 &num_tasks)
{
    if (start == end)
        end++;
    dual_println("Verifying that a loop from ", start, " to ", end, " with ", num_tasks, num_tasks == 1 ? " task" : " tasks", " modifies all indices...");
    ui64 num_indices = (ui64)std::abs(end - start);
    i32 offset = std::min(start, end);
    std::vector<std::atomic<bool>> flags((ui64)num_indices);
    pool.parallelize_loop(
        start, end, [&flags, &offset](const i32 &start, const i32 &end)
        {
            for (i32 i = start; i < end; i++)
                flags[(ui64)(i - offset)] = true;
        },
        num_tasks);
    bool all_flags = true;
    for (ui64 i = 0; i < num_indices; i++)
        all_flags = all_flags && flags[i];
    check(all_flags);
}

/**
 * @brief Check that parallelize_loop() works using several different random values for the range of indices and number of tasks.
 */
void check_parallelize_loop()
{
    std::mt19937_64 mt(rd());
    std::uniform_int_distribution<i32> index_dist((i32)pool.get_thread_count() * -100, (i32)pool.get_thread_count() * 100);
    std::uniform_int_distribution<ui32> task_dist(1, pool.get_thread_count());
    for (ui32 i = 0; i < 10; i++)
        check_parallelize_loop(index_dist(mt), index_dist(mt), task_dist(mt));
}

/**
 * @brief Check that sleep_duration works for a specific value.
 *
 * @param duration The value of sleep_duration.
 */
void check_sleep_duration(const ui32 &duration)
{
    dual_println("Submitting tasks with sleep_duration = ", duration, " microseconds...");
    pool.sleep_duration = duration;
    ui32 n = pool.get_thread_count() * 100;
    std::vector<std::atomic<bool>> flags(n);
    for (ui32 i = 0; i < n; i++)
        pool.push_task([&flags, i]
                       { flags[i] = true; });
    pool.wait_for_tasks();
    bool all_flags = true;
    for (ui32 i = 0; i < n; i++)
        all_flags = all_flags && flags[i];
    check(all_flags);
}

/**
 * @brief Check that sleep_duration works for several different random values.
 */
void check_sleep_duration()
{
    ui32 old_duration = pool.sleep_duration;
    check_sleep_duration(0);
    std::mt19937_64 mt(rd());
    std::uniform_int_distribution<ui32> dist(1, 2000);
    for (ui32 i = 0; i < 5; i++)
        check_sleep_duration(dist(mt));
    dual_println("Resetting sleep_duration to the default value (", old_duration, " microseconds).");
    pool.sleep_duration = old_duration;
}

/**
 * @brief Check that task monitoring works.
 */
void check_task_monitoring()
{
    ui32 n = std::min<ui32>(std::thread::hardware_concurrency(), 4);
    dual_println("Resetting pool to ", n, " threads.");
    pool.reset(n);
    dual_println("Submitting ", n * 3, " tasks.");
    std::vector<std::atomic<bool>> release(n * 3);
    for (ui32 i = 0; i < n * 3; i++)
        pool.push_task([&release, i]
                       {
                           while (!release[i])
                               std::this_thread::yield();
                           dual_println("Task ", i, " released.");
                       });
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    dual_println("After submission, should have: ", n * 3, " tasks total, ", n, " tasks running, ", n * 2, " tasks queued...");
    check(pool.get_tasks_total() == n * 3 && pool.get_tasks_running() == n && pool.get_tasks_queued() == n * 2);

    for (ui32 i = 0; i < n; i++)
        release[i] = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    dual_println("After releasing ", n, " tasks, should have: ", n * 2, " tasks total, ", n, " tasks running, ", n, " tasks queued...");
    check(pool.get_tasks_total() == n * 2 && pool.get_tasks_running() == n && pool.get_tasks_queued() == n);

    for (ui32 i = n; i < n * 2; i++)
        release[i] = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    dual_println("After releasing ", n, " more tasks, should have: ", n, " tasks total, ", n, " tasks running, ", 0, " tasks queued...");
    check(pool.get_tasks_total() == n && pool.get_tasks_running() == n && pool.get_tasks_queued() == 0);

    for (ui32 i = n * 2; i < n * 3; i++)
        release[i] = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
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
    ui32 n = std::min<ui32>(std::thread::hardware_concurrency(), 4);
    dual_println("Resetting pool to ", n, " threads.");
    pool.reset(n);
    dual_println("Pausing pool.");
    pool.paused = true;
    dual_println("Submitting ", n * 3, " tasks, each one waiting for 200ms.");
    for (ui32 i = 0; i < n * 3; i++)
        pool.push_task([i]
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

/**
 * @brief Check that exception handling work.
 */
void check_exceptions()
{
    bool caught = false;
    auto my_future = pool.submit([]
                                 { throw std::runtime_error("Exception thrown!"); });
    try
    {
        my_future.get();
    }
    catch (const std::exception &e)
    {
        if (e.what() == std::string("Exception thrown!"))
            caught = true;
    }
    check(caught);
}

/**
 * @brief A lightweight matrix class template for performance testing purposes. Not for general use; only contains the bare minimum functionality needed for the test. Based on https://github.com/bshoshany/multithreaded-matrix
 *
 * @tparam T The type to use for the matrix elements.
 */
template <typename T>
class matrix
{
public:
    // =====================================
    // Constructors and assignment operators
    // =====================================

    /**
     * @brief Construct an uninitialized matrix.
     *
     * @param _rows The number of rows.
     * @param _cols The number of columns.
     */
    matrix(const ui64 &_rows, const ui64 &_cols)
        : rows(_rows), cols(_cols), smart_elements(new T[rows * cols])
    {
        elements = smart_elements.get();
    }

    /**
     * @brief Construct a new matrix by copying the elements of an existing matrix.
     *
     * @param m The matrix to be copied.
     */
    matrix(const matrix<T> &m)
        : rows(m.rows), cols(m.cols), smart_elements(new T[rows * cols])
    {
        elements = smart_elements.get();
        for (ui64 i = 0; i < rows * cols; i++)
            elements[i] = m.elements[i];
    }

    /**
     * @brief Construct a new matrix by moving the elements of an existing matrix.
     *
     * @param m The matrix to be moved.
     */
    matrix(matrix<T> &&m)
        : rows(m.rows), cols(m.cols), smart_elements(std::move(m.smart_elements))
    {
        elements = smart_elements.get();
        m.rows = 0;
        m.cols = 0;
        m.elements = nullptr;
    }

    /**
     * @brief Copy the elements of another matrix to this matrix.
     *
     * @param m The matrix to be copied.
     * @return A reference to this matrix.
     */
    matrix<T> &operator=(const matrix<T> &m)
    {
        rows = m.rows;
        cols = m.cols;
        smart_elements.reset(new T[rows * cols]);
        elements = smart_elements.get();
        for (ui64 i = 0; i < rows * cols; i++)
            elements[i] = m.elements[i];
        return *this;
    }

    /**
     * @brief Move the elements of another matrix to this matrix.
     *
     * @param m The matrix to be moved.
     * @return A reference to this matrix.
     */
    matrix<T> &operator=(matrix<T> &&m)
    {
        rows = m.rows;
        cols = m.cols;
        smart_elements = std::move(m.smart_elements);
        elements = smart_elements.get();
        m.rows = 0;
        m.cols = 0;
        m.elements = nullptr;
        return *this;
    }

    // ====================
    // Overloaded operators
    // ====================

    /**
     * @brief Read or modify a matrix element.
     *
     * @param row The row index (starting from zero).
     * @param col The column index (starting from zero).
     * @return A reference to the element.
     */
    inline T &operator()(const ui64 &row, const ui64 &col)
    {
        return elements[(cols * row) + col];
    }

    /**
     * @brief Read a matrix element.
     *
     * @param row The row index (starting from zero).
     * @param col The column index (starting from zero).
     * @return The value of the element.
     */
    inline T operator()(const ui64 &row, const ui64 &col) const
    {
        return elements[(cols * row) + col];
    }

    /**
     * @brief Read or modify an element of the underlying 1-dimensional array.
     *
     * @param i The element index (starting from zero).
     * @return A reference to the element.
     */
    inline T &operator[](const ui64 &i)
    {
        return elements[i];
    }

    /**
     * @brief Read an element of the underlying 1-dimensional array.
     *
     * @param i The element index (starting from zero).
     * @return The value of the element.
     */
    inline T operator[](const ui64 &i) const
    {
        return elements[i];
    }

    /**
     * @brief Compare this matrix to another matrix.
     *
     * @param m The matrix to compare to.
     * @return Whether the matrices have the same elements.
     */
    bool operator==(const matrix<T> &m) const
    {
        bool compare_result = true;
        for (ui64 i = 0; i < rows * cols; i++)
            compare_result = compare_result && (elements[i] == m.elements[i]);
        return compare_result;
    }

    // =======================
    // Public member functions
    // =======================

    /**
     * @brief Transpose a matrix.
     *
     * @param num_tasks The number of parallel tasks to use. If set to 0, no multithreading will be used.
     * @return The transposed matrix.
     */
    matrix<T> transpose(const ui32 &num_tasks) const
    {
        matrix<T> out(cols, rows);
        if (num_tasks == 0)
        {
            for (ui64 i = 0; i < out.rows; i++)
                for (ui64 j = 0; j < out.cols; j++)
                    out(i, j) = operator()(j, i);
        }
        else
        {
            pool.parallelize_loop(
                0, out.rows, [this, &out](const ui64 &start, const ui64 &end)
                {
                    for (ui64 i = start; i < end; i++)
                        for (ui64 j = 0; j < out.cols; j++)
                            out(i, j) = operator()(j, i);
                },
                num_tasks);
        }
        return out;
    }

    // ================
    // Friend functions
    // ================

    /**
     * @brief Add two matrices using the specified number of parallel tasks.
     *
     * @param a The first matrix to be added.
     * @param b The second matrix to be added.
     * @param num_tasks The number of parallel tasks to use. If set to 0, no multithreading will be used.
     * @return The sum of the matrices.
     */
    friend matrix<T> add_matrices(const matrix<T> &a, const matrix<T> &b, const ui32 &num_tasks)
    {
        matrix<T> c(a.rows, a.cols);
        if (num_tasks == 0)
            for (ui64 i = 0; i < a.rows * a.cols; i++)
                c[i] = a[i] + b[i];
        else
            pool.parallelize_loop(
                0, a.rows * a.cols, [&a, &b, &c](const ui64 &start, const ui64 &end)
                {
                    for (ui64 i = start; i < end; i++)
                        c[i] = a[i] + b[i];
                },
                num_tasks);
        return c;
    }

    /**
     * @brief Multiply two matrices using the specified number of parallel tasks.
     *
     * @param a The first matrix to be multiplied.
     * @param b The second matrix to be multiplied.
     * @param num_tasks The number of parallel tasks to use. If set to 0, no multithreading will be used.
     * @return The product of the matrices.
     */
    friend matrix<T> multiply_matrices(const matrix<T> &a, const matrix<T> &b, const ui32 &num_tasks)
    {
        matrix<T> c(a.rows, b.cols);
        if (num_tasks == 0)
        {
            for (ui64 i = 0; i < a.rows; i++)
                for (ui64 j = 0; j < b.cols; j++)
                {
                    c(i, j) = 0;
                    for (ui64 k = 0; k < a.cols; k++)
                        c(i, j) += a(i, k) * b(k, j);
                }
        }
        else
        {
            pool.parallelize_loop(
                0, a.rows, [&a, &b, &c, &a_cols = a.cols, &b_cols = b.cols](const ui64 &start, const ui64 &end)
                {
                    for (ui64 i = start; i < end; i++)
                        for (ui64 j = 0; j < b_cols; j++)
                        {
                            c(i, j) = 0;
                            for (ui64 k = 0; k < a_cols; k++)
                                c(i, j) += a(i, k) * b(k, j);
                        }
                },
                num_tasks);
        }
        return c;
    }

private:
    // ========================
    // Private member variables
    // ========================

    /**
     * @brief The number of rows.
     */
    ui64 rows = 0;

    /**
     * @brief The number of columns.
     */
    ui64 cols = 0;

    /**
     * @brief A pointer to an array storing the elements of the matrix in flattened 1-dimensional form.
     */
    T *elements = nullptr;

    /**
     * @brief A smart pointer to manage the memory allocated for the matrix elements.
     */
    std::unique_ptr<T[]> smart_elements;
};

/**
 * @brief A class template for generating random matrices.
 *
 * @tparam T The type to use for the matrix elements.
 * @tparam D The distribution to use, e.g. std::uniform_real_distribution<double>.
 */
template <typename T, typename D>
class random_matrix_generator
{
public:
    // ============
    // Constructors
    // ============

    /**
     * @brief Construct a new random matrix generator.
     *
     * @tparam P The types of the parameters to pass to the constructor of the distribution.
     * @param params The parameters to pass to the constructor of the distribution. The number of parameters and their types depends on the particular distribution being used.
     */
    template <typename... P>
    random_matrix_generator(const P &...params) : dist(params...), rd() {}

    // =======================
    // Public member functions
    // =======================

    /**
     * @brief Generate a random matrix with the given number of rows and columns.
     *
     * @param rows The desired number of rows in the matrix.
     * @param cols The desired number of columns in the matrix.
     * @param num_tasks The number of parallel tasks to use. If set to 0, no multithreading will be used.
     * @return The random matrix.
     */
    matrix<T> generate_matrix(const ui64 &rows, const ui64 &cols, const ui32 &num_tasks)
    {
        matrix<T> m(rows, cols);
        if (num_tasks == 0)
        {
            std::mt19937_64 mt(generate_seed());
            for (ui64 i = 0; i < rows * cols; i++)
                m[i] = dist(mt);
        }
        else
            pool.parallelize_loop(
                0, rows * cols, [this, &m](const ui64 &start, const ui64 &end)
                {
                    std::mt19937_64 mt(generate_seed());
                    for (ui64 i = start; i < end; i++)
                        m[i] = dist(mt);
                },
                num_tasks);
        return m;
    }

private:
    // ========================
    // Private member functions
    // ========================

    /**
     * @brief Generate a seed. The std::mt19937_64 in each block will be seeded using this function in order to avoid depleting the entropy of the random_device.
     *
     * @return A random unsigned 64-bit integer.
     */
    ui64 generate_seed()
    {
        static std::mt19937_64 mt(rd());
        return mt();
    }

    // ========================
    // Private member variables
    // ========================

    /**
     * @brief The distribution to use for generating random numbers.
     */
    D dist;

    /**
     * @brief The random device to be used for seeding the pseudo-random number generators.
     */
    std::random_device rd;
};

/**
 * @brief Check the matrix class template by comparing the results of adding, multiplying, and transposing matrices calculated in two ways: single-threaded and multithreaded.
 */
void check_matrix()
{
    // Initialize a random_matrix_generator object to generates matrices with integers uniformly distributed between -1000 and 1000.
    random_matrix_generator<i64, std::uniform_int_distribution<i64>> rnd(-1000, 1000);
    // Define the size of the matrices to use.
    const ui32 thread_count = pool.get_thread_count();
    const ui64 rows = thread_count * 10;
    const ui64 cols = rows;
    const ui64 total_size = rows * cols;
    dual_println("Using matrices of size ", rows, "x", cols, " with a total of ", total_size, " elements.");

    matrix<i64> A = rnd.generate_matrix(rows, cols, thread_count);
    matrix<i64> B = rnd.generate_matrix(rows, cols, thread_count);

    dual_println("Adding two matrices (single-threaded)...");
    matrix<i64> ApB_single = add_matrices(A, B, 0);
    dual_println("Adding two matrices (multithreaded)...");
    matrix<i64> ApB_multi = add_matrices(A, B, thread_count);
    dual_println("Comparing the results...");
    check(ApB_single == ApB_multi);

    dual_println("Transposing a matrix (single-threaded)...");
    matrix<i64> At_single = A.transpose(0);
    dual_println("Transposing a matrix (multithreaded)...");
    matrix<i64> At_multi = A.transpose(thread_count);
    dual_println("Comparing the results...");
    check(At_single == At_multi);

    dual_println("Multiplying two matrices (single-threaded)...");
    matrix<i64> AxB_single = multiply_matrices(A, B, 0);
    dual_println("Multiplying two matrices (multithreaded)...");
    matrix<i64> AxB_multi = multiply_matrices(A, B, thread_count);
    dual_println("Comparing the results...");
    check(AxB_single == AxB_multi);
}

/**
 * @brief Print the timing of a specific test.
 *
 * @param num_tasks The number of tasks.
 * @param mean_sd std::pair containing the mean as the first member and standard deviation as the second member.
 */
void print_timing(const ui32 &num_tasks, const std::pair<double, double> &mean_sd)
{
    if (num_tasks == 1)
        dual_print("With   1  task");
    else
        dual_print("With ", std::setw(3), num_tasks, " tasks");
    dual_println(", mean execution time was ", std::setw(6), mean_sd.first, " ms with standard deviation ", std::setw(4), mean_sd.second, " ms.");
}

/**
 * @brief Calculate and print the speedup obtained by multithreading.
 *
 * @param timings A vector of the timings corresponding to different numbers of tasks.
 * @return The maximum speedup obtained.
 */
double print_speedup(const std::vector<double> &timings)
{
    const auto [min_time, max_time] = std::minmax_element(std::begin(timings), std::end(timings));
    double max_speedup = *max_time / *min_time;
    dual_println("Maximum speedup obtained: ", max_speedup, "x.");
    return max_speedup;
}

/**
 * @brief Calculate the mean and standard deviation of a set of integers.
 *
 * @param timings The integers.
 * @return std::pair containing the mean as the first member and standard deviation as the second member.
 */
std::pair<double, double> analyze(const std::vector<i64> &timings)
{
    double mean = 0;
    for (size_t i = 0; i < timings.size(); i++)
        mean += (double)timings[i] / (double)timings.size();
    double variance = 0;
    for (size_t i = 0; i < timings.size(); i++)
        variance += ((double)timings[i] - mean) * ((double)timings[i] - mean) / (double)timings.size();
    double sd = std::sqrt(variance);
    return std::pair(mean, sd);
}

/**
 * @brief Perform a performance test using some matrix operations.
 */
void check_performance()
{
    // Set the formatting of floating point numbers.
    dual_print(std::fixed, std::setprecision(1));

    // Initialize a random_matrix_generator object to generates matrices with real (floating-point) numbers uniformly distributed between -1000 and 1000.
    random_matrix_generator<double, std::uniform_real_distribution<double>> rnd(-1000, 1000);

    // Initialize a timer object to measure the execution time of various operations.
    timer tmr;

    // If the CPU has more than 8 threads, we leave 2 threads for the rest of the operating system. Otherwise, performance may suffer.
    const ui32 thread_count = pool.get_thread_count() <= 8 ? pool.get_thread_count() : pool.get_thread_count() - 2;
    dual_println("Using ", thread_count, " out of ", pool.get_thread_count(), " threads.");

    // Define the size of the matrices to use.
    const ui64 rows = thread_count * 200;
    const ui64 cols = rows;

    // The number of tasks to try for each operation.
    const ui32 try_tasks[] = {1, thread_count / 4, thread_count / 2, thread_count, thread_count * 2, thread_count * 4};

    // Generate two random test matrices to be used for benchmarking addition, transposition, and random matrix generation.
    matrix<double> A = rnd.generate_matrix(rows, cols, thread_count);
    matrix<double> B = rnd.generate_matrix(rows, cols, thread_count);

    // Generate two random test matrices to be used for benchmarking multiplication. Since matrix multiplication is O(n^3), we reduce the size of the test matrices so that this operation completes within a reasonable time.
    constexpr ui64 mult_factor = 8;
    matrix<double> X = rnd.generate_matrix(rows / mult_factor, cols / mult_factor, thread_count);
    matrix<double> Y = rnd.generate_matrix(cols / mult_factor, rows / mult_factor, thread_count);

    // Determine the optimal sleep duration for this system.
    dual_print("Determining the optimal sleep duration...");
    i64 optimal_ms = 0;
    ui64 optimal_sleep = 0;
    for (ui64 sleep = 0; sleep <= 2000; sleep += 100)
    {
        dual_print(".");
        pool.sleep_duration = (ui32)sleep;
        tmr.start();
        matrix<double> C = add_matrices(A, B, thread_count);
        matrix<double> D = A.transpose(thread_count);
        matrix<double> E = multiply_matrices(X, Y, thread_count);
        matrix<double> F = rnd.generate_matrix(rows, cols, thread_count);
        tmr.stop();
        if (tmr.ms() < optimal_ms || optimal_ms == 0)
        {
            optimal_ms = tmr.ms();
            optimal_sleep = sleep;
        }
    }
    if (optimal_sleep == 0)
        dual_println("\nResult: Using std::this_thread::yield() instead of std::this_thread::sleep_for() is optimal.");
    else
        dual_println("\nResult: The optimal sleep duration is ", optimal_sleep, " microseconds.");
    pool.sleep_duration = (ui32)optimal_sleep;

    // Vectors to store statistics.
    std::vector<double> different_n_timings;
    std::vector<i64> same_n_timings;
    std::vector<double> speedups;

    // How many times to run each test.
    constexpr ui32 repeat = 20;

    dual_println("\nAdding two ", rows, "x", cols, " matrices ", repeat, " times:");
    for (ui32 n : try_tasks)
    {
        for (ui32 i = 0; i < repeat; i++)
        {
            tmr.start();
            matrix<double> C = add_matrices(A, B, n);
            tmr.stop();
            same_n_timings.push_back(tmr.ms());
        }
        auto mean_sd = analyze(same_n_timings);
        print_timing(n, mean_sd);
        different_n_timings.push_back(mean_sd.first);
        same_n_timings.clear();
    }
    speedups.push_back(print_speedup(different_n_timings));
    different_n_timings.clear();

    dual_println("\nTransposing one ", rows, "x", cols, " matrix ", repeat, " times:");
    for (ui32 n : try_tasks)
    {
        for (ui32 i = 0; i < repeat; i++)
        {
            tmr.start();
            matrix<double> C = A.transpose(n);
            tmr.stop();
            same_n_timings.push_back(tmr.ms());
        }
        auto mean_sd = analyze(same_n_timings);
        print_timing(n, mean_sd);
        different_n_timings.push_back(mean_sd.first);
        same_n_timings.clear();
    }
    speedups.push_back(print_speedup(different_n_timings));
    different_n_timings.clear();

    dual_println("\nMultiplying two ", rows / mult_factor, "x", cols / mult_factor, " matrices ", repeat, " times:");
    for (ui32 n : try_tasks)
    {
        for (ui32 i = 0; i < repeat; i++)
        {
            tmr.start();
            matrix<double> C = multiply_matrices(X, Y, n);
            tmr.stop();
            same_n_timings.push_back(tmr.ms());
        }
        auto mean_sd = analyze(same_n_timings);
        print_timing(n, mean_sd);
        different_n_timings.push_back(mean_sd.first);
        same_n_timings.clear();
    }
    speedups.push_back(print_speedup(different_n_timings));
    different_n_timings.clear();

    dual_println("\nGenerating random ", rows, "x", cols, " matrix ", repeat, " times:");
    for (ui32 n : try_tasks)
    {
        for (ui32 i = 0; i < repeat; i++)
        {
            tmr.start();
            matrix<double> C = rnd.generate_matrix(rows, cols, n);
            tmr.stop();
            same_n_timings.push_back(tmr.ms());
        }
        auto mean_sd = analyze(same_n_timings);
        print_timing(n, mean_sd);
        different_n_timings.push_back(mean_sd.first);
        same_n_timings.clear();
    }
    speedups.push_back(print_speedup(different_n_timings));

    const double max_speedup = *std::max_element(std::begin(speedups), std::end(speedups));
    dual_println("\nOverall, multithreading provided speedups of up to ", max_speedup, "x.");
}

int main()
{
    std::string log_filename = "thread_pool_test-" + get_time() + ".log";
    log_file.open(log_filename);

    dual_println("A C++17 Thread Pool for High-Performance Scientific Computing");
    dual_println("(c) 2021 Barak Shoshany (baraksh@gmail.com) (http://baraksh.com)");
    dual_println("GitHub: https://github.com/bshoshany/thread-pool\n");

    dual_println("Thread pool library version is ", THREAD_POOL_VERSION, ".");
    dual_println("Hardware concurrency is ", std::thread::hardware_concurrency(), ".");
    dual_println("Generating log file: ", log_filename, ".\n");

    dual_println("Important: Please do not run any other applications, especially multithreaded applications, in parallel with this test!");

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

    print_header("Checking that different values of sleep_duration work:");
    check_sleep_duration();

    print_header("Checking that task monitoring works:");
    check_task_monitoring();

    print_header("Checking that pausing works:");
    check_pausing();

    print_header("Checking that exception handling works:");
    check_exceptions();

    print_header("Testing that matrix operations produce the expected results:");
    check_matrix();

    if (tests_failed == 0)
    {
        print_header("SUCCESS: Passed all " + std::to_string(tests_succeeded) + " checks!", '+');
        print_header("Performing matrix performance test:");
        check_performance();
        print_header("Thread pool performance test completed!", '+');
    }
    else
    {
        print_header("FAILURE: Passed " + std::to_string(tests_succeeded) + " checks, but failed " + std::to_string(tests_failed) + "!", '+');
        dual_println("\nPlease submit a bug report at https://github.com/bshoshany/thread-pool/issues including the exact specifications of your system (OS, CPU, compiler, etc.) and the generated log file.");
    }

    return 0;
}
