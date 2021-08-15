[![DOI:10.5281/zenodo.4742687](https://zenodo.org/badge/DOI/10.5281/zenodo.4742687.svg)](https://doi.org/10.5281/zenodo.4742687)
[![arXiv:2105.00613](https://img.shields.io/badge/arXiv-2105.00613-b31b1b.svg)](https://arxiv.org/abs/2105.00613)
[![License: MIT](https://img.shields.io/github/license/bshoshany/thread-pool)](https://github.com/bshoshany/thread-pool/blob/master/LICENSE.txt)
![Language: C++17](https://img.shields.io/badge/Language-C%2B%2B17-yellow)
![File size in bytes](https://img.shields.io/github/size/bshoshany/thread-pool/thread_pool.hpp)
![GitHub last commit](https://img.shields.io/github/last-commit/bshoshany/thread-pool)
[![GitHub repo stars](https://img.shields.io/github/stars/bshoshany/thread-pool?style=social)](https://github.com/bshoshany/thread-pool)
[![Twitter @BarakShoshany](https://img.shields.io/twitter/follow/BarakShoshany?style=social)](https://twitter.com/BarakShoshany)
[![Open in Visual Studio Code](https://open.vscode.dev/badges/open-in-vscode.svg)](https://open.vscode.dev/bshoshany/thread-pool)

# A C++17 Thread Pool for High-Performance Scientific Computing

**Barak Shoshany**\
Department of Physics, Brock University,\
1812 Sir Isaac Brock Way, St. Catharines, Ontario, L2S 3A1, Canada\
[bshoshany@brocku.ca](mailto:bshoshany@brocku.ca) | [https://baraksh.com/](https://baraksh.com/)\
Companion paper: [arXiv:2105.00613](https://arxiv.org/abs/2105.00613)\
DOI: [doi:10.5281/zenodo.4742687](https://doi.org/10.5281/zenodo.4742687)

* [Abstract](#abstract)
* [Introduction](#introduction)
    * [Motivation](#motivation)
    * [Overview of features](#overview-of-features)
    * [Compiling and compatibility](#compiling-and-compatibility)
* [Getting started](#getting-started)
    * [Including the library](#including-the-library)
    * [Installing using vcpkg](#installing-using-vcpkg)
    * [Constructors](#constructors)
    * [Getting and resetting the number of threads in the pool](#getting-and-resetting-the-number-of-threads-in-the-pool)
    * [Finding the version of the package](#finding-the-version-of-the-package)
* [Submitting and waiting for tasks](#submitting-and-waiting-for-tasks)
    * [Submitting tasks to the queue with futures](#submitting-tasks-to-the-queue-with-futures)
    * [Submitting tasks to the queue without futures](#submitting-tasks-to-the-queue-without-futures)
    * [Manually waiting for all tasks to complete](#manually-waiting-for-all-tasks-to-complete)
    * [Parallelizing loops](#parallelizing-loops)
* [Helper classes](#helper-classes)
    * [Synchronizing printing to an output stream](#synchronizing-printing-to-an-output-stream)
    * [Measuring execution time](#measuring-execution-time)
* [Other features](#other-features)
    * [Setting the worker function's sleep duration](#setting-the-worker-functions-sleep-duration)
    * [Monitoring the tasks](#monitoring-the-tasks)
    * [Pausing the workers](#pausing-the-workers)
    * [Exception handling](#exception-handling)
* [Testing the package](#testing-the-package)
    * [Automated tests](#automated-tests)
    * [Performance tests](#performance-tests)
    * [Dual Intel Xeon Gold 6148 (80 threads)](#dual-intel-xeon-gold-6148-80-threads)
* [Issue and pull request policy](#issue-and-pull-request-policy)
* [Copyright and citing](#copyright-and-citing)

## Abstract

We present a modern C++17-compatible thread pool implementation, built from scratch with high-performance scientific computing in mind. The thread pool is implemented as a single lightweight and self-contained class, and does not have any dependencies other than the C++17 standard library, thus allowing a great degree of portability. In particular, our implementation does not utilize OpenMP or any other high-level multithreading APIs, and thus gives the programmer precise low-level control over the details of the parallelization, which permits more robust optimizations. The thread pool was extensively tested on both AMD and Intel CPUs with up to 40 cores and 80 threads. This paper provides motivation, detailed usage instructions, and performance tests. The code is freely available in the [GitHub repository](https://github.com/bshoshany/thread-pool). This `README.md` file contains roughly the same content as the [companion paper](https://arxiv.org/abs/2105.00613).

## Introduction

### Motivation

Multithreading is essential for modern high-performance computing. Since C++11, the C++ standard library has included built-in low-level multithreading support using constructs such as `std::thread`. However, `std::thread` creates a new thread each time it is called, which can have a significant performance overhead. Furthermore, it is possible to create more threads than the hardware can handle simultaneously, potentially resulting in a substantial slowdown.

This library contains a thread pool class, `thread_pool`, which avoids these issues by creating a fixed pool of threads once and for all, and then reusing the same threads to perform different tasks throughout the lifetime of the pool. By default, the number of threads in the pool is equal to the maximum number of threads that the hardware can run in parallel.

The user submits tasks to be executed into a queue. Whenever a thread becomes available, it pops a task from the queue and executes it. Each task is automatically assigned an `std::future`, which can be used to wait for the task to finish executing and/or obtain its eventual return value.

In addition to `std::thread`, the C++ standard library also offers the higher-level construct `std::async`, which may internally utilize a thread pool - but this is not guaranteed, and in fact, currently only the MSVC implementation of `std::async` uses a thread pool, while GCC and Clang do not. Using our custom-made thread pool class instead of `std::async` allows the user more control, transparency, and portability.

High-level multithreading APIs, such as OpenMP, allow simple one-line automatic parallelization of C++ code, but they do not give the user precise low-level control over the details of the parallelization. The thread pool class presented here allows the programmer to perform and manage the parallelization at the lowest level, and thus permits more robust optimizations, which can be used to achieve considerably higher performance.

As demonstrated in the performance tests [below](#performance-tests), using our thread pool class we were able to saturate the upper bound of expected speedup for matrix multiplication and generation of random matrices. These performance tests were performed on 12-core / 24-thread and 40-core / 80-thread systems using GCC on Linux.

### Overview of features

* **Fast:**
    * Built from scratch with maximum performance in mind.
    * Suitable for use in high-performance computing nodes with a very large number of CPU cores.
    * Compact code, to reduce both compilation time and binary size.
    * Reusing threads avoids the overhead of creating and destroying them for individual tasks.
    * A task queue ensures that there are never more threads running in parallel than allowed by the hardware.
* **Lightweight:**
    * Only ~180 lines of code, excluding comments, blank lines, and the two optional helper classes.
    * Single header file: simply `#include "thread_pool.hpp"`.
    * Header-only: no need to install or build the library.
    * Self-contained: no external requirements or dependencies. Does not require OpenMP or any other multithreading APIs. Only uses the C++ standard library, and works with any C++17-compliant compiler.
* **Easy to use:**
    * Very simple operation, using a handful of member functions.
    * Every task submitted to the queue automatically generates an `std::future`, which can be used to wait for the task to finish executing and/or obtain its eventual return value.
    * Optionally, tasks may also be submitted without generating a future, sacrificing convenience for greater performance.
    * The code is thoroughly documented using Doxygen comments - not only the interface, but also the implementation, in case the user would like to make modifications.
    * The included test program `thread_pool_test.cpp` can be used to perform comprehensive automated tests and benchmarks, and also serves as an extensive example of how to properly use the package.
* **Additional features:**
    * Automatically parallelize a loop into any number of parallel tasks.
    * Easily wait for all tasks in the queue to complete.
    * Change the number of threads in the pool safely and on-the-fly as needed.
    * Fine-tune the sleep duration of each thread's worker function for optimal performance.
    * Monitor the number of queued and/or running tasks.
    * Pause and resume popping new tasks out of the queue.
    * Catch exceptions thrown by the submitted tasks.
    * Synchronize output to a stream from multiple threads in parallel using the `synced_stream` helper class.
    * Easily measure execution time for benchmarking purposes using the `timer` helper class.
    * Under continuous and active development. Bug reports and feature requests are welcome, and should be made via [GitHub issues](https://github.com/bshoshany/thread-pool/issues).

### Compiling and compatibility

This library should successfully compile on any C++17 standard-compliant compiler, on all operating systems and architectures for which such a compiler is available. Compatibility was verified with a 12-core / 24-thread AMD Ryzen 9 3900X CPU at 3.8 GHz using the following compilers and platforms:

* Windows 10 build 19043.1165:
    * [GCC](https://gcc.gnu.org/) v11.2.0 ([WinLibs build](https://winlibs.com/))
    * [Clang](https://clang.llvm.org/) v12.0.1
    * [MSVC](https://docs.microsoft.com/en-us/cpp/) v19.29.30133
* Ubuntu 21.04:
    * [GCC](https://gcc.gnu.org/) v11.1.0
    * [Clang](https://clang.llvm.org/) v12.0.0

In addition, this library was tested on a [Compute Canada](https://www.computecanada.ca/) node equipped with two 20-core / 40-thread Intel Xeon Gold 6148 CPUs at 2.4 GHz (for a total of 40 cores and 80 threads), running CentOS Linux 7.6.1810, using the following compilers:

* [GCC](https://gcc.gnu.org/) v9.4.0
* [Intel C++ Compiler (ICC)](https://software.intel.com/content/www/us/en/develop/tools/oneapi/components/dpc-compiler.html) v19.1.3.304

The test program `thread_pool_test.cpp` was compiled without warnings (with the warning flags `-Wall -Wpedantic -Wextra -Wconversion -Weffc++` in GCC/Clang and `/W4` in MSVC), executed, and successfully completed all [automated tests](#testing-the-package) using all of the compilers and systems mentioned above.

As this library requires C++17 features, the code must be compiled with C++17 support:

* For GCC, Clang, or ICC, use the `-std=c++17` flag. On Linux, you will also need to use the `-pthread` flag to enable the POSIX threads library.
* For MSVC, use `/std:c++17`.

For maximum performance, it is recommended to compile with all available compiler optimizations:

* For GCC, Clang, or ICC, use the `-O3` flag.
* For MSVC, use `/O2`.

## Getting started

### Including the library

To use the thread pool library, simply download the [latest release](https://github.com/bshoshany/thread-pool/releases) from the GitHub repository, place the single header file `thread_pool.hpp` in the desired folder, and include it in your program:

```cpp
#include "thread_pool.hpp"
```

The thread pool will now be accessible via the `thread_pool` class.

### Installing using vcpkg

If you are using the [vcpkg](https://github.com/microsoft/vcpkg) C/C++ library manager, you can easily download and install this package with the following commands.

On Linux/macOS:

```none
./vcpkg install bshoshany-thread-pool
```

On Windows:

```none
.\vcpkg install bshoshany-thread-pool:x86-windows bshoshany-thread-pool:x64-windows
```

The thread pool will then be available automatically in the build system you integrated vcpkg with (e.g. MSBuild or CMake). Simply write `#include "thread_pool.hpp"` in any project to use the thread pool, without having to copy to file into the project first. I will update the vcpkg port with each new release, so it will be updated automatically when you run `vcpkg upgrade`.

Please see the [vcpkg repository](https://github.com/microsoft/vcpkg) for more information on how to use vcpkg.

### Constructors

The default constructor creates a thread pool with as many threads as the hardware can handle concurrently, as reported by the implementation via `std::thread::hardware_concurrency()`. With a hyperthreaded CPU, this will be twice the number of CPU cores. This is probably the constructor you want to use. For example:

```cpp
// Constructs a thread pool with as many threads as available in the hardware.
thread_pool pool;
```

Optionally, a number of threads different from the hardware concurrency can be specified as an argument to the constructor. However, note that adding more threads than the hardware can handle will **not** improve performance, and in fact will most likely hinder it. This option exists in order to allow using **less** threads than the hardware concurrency, in cases where you wish to leave some threads available for other processes. For example:

```cpp
// Constructs a thread pool with only 12 threads.
thread_pool pool(12);
```

If your program's main thread only submits tasks to the thread pool and waits for them to finish, and does not perform any computationally intensive tasks on its own, then it is recommended to use the default value for the number of threads. This ensures that all of the threads available in the hardware will be put to work while the main thread waits.

However, if your main thread does perform computationally intensive tasks on its own, then it is recommended to use the value `std::thread::hardware_concurrency() - 1` for the number of threads. In this case, the main thread plus the thread pool will together take up exactly all the threads available in the hardware.

### Getting and resetting the number of threads in the pool

The member function `get_thread_count()` returns the number of threads in the pool. This will be equal to `std::thread::hardware_concurrency()` if the default constructor was used.

It is generally unnecessary to change the number of threads in the pool after it has been created, since the whole point of a thread pool is that you only create the threads once. However, if needed, this can be done, safely and on-the-fly, using the `reset()` member function.

`reset()` will wait for all currently running tasks to be completed, but will leave the rest of the tasks in the queue. Then it will destroy the thread pool and create a new one with the desired new number of threads, as specified in the function's argument (or the hardware concurrency if no argument is given). The new thread pool will then resume executing the tasks that remained in the queue and any new submitted tasks.

### Finding the version of the package

If desired, the version of this package may be read during compilation time from the macro `THREAD_POOL_VERSION`. The value will be a string containing the version number and release date. For example:

```cpp
std::cout << "Thread pool library version is " << THREAD_POOL_VERSION << ".\n";
```

Sample output:

```none
Thread pool library version is v2.0.0 (2021-08-14).
```

## Submitting and waiting for tasks

### Submitting tasks to the queue with futures

A task can be any function, with zero or more arguments, and with or without a return value. Once a task has been submitted to the queue, it will be executed as soon as a thread becomes available. Tasks are executed in the order that they were submitted (first-in, first-out).

The member function `submit()` is used to submit tasks to the queue. The first argument is the function to execute, and the rest of the arguments are the arguments to pass to the function, if any. The return value is an `std::future` associated to the task. For example:

```cpp
// Submit a task without arguments to the queue, and get a future for it.
auto my_future = pool.submit(task);
// Submit a task with one argument to the queue, and get a future for it.
auto my_future = pool.submit(task, arg);
// Submit a task with two arguments to the queue, and get a future for it.
auto my_future = pool.submit(task, arg1, arg2);
```

Using `auto` for the return value of `submit()` is recommended, since it means the compiler will automatically detect which instance of the template `std::future` to use. The value of the future depends on whether the function has a return value or not:

* If the submitted function has a return value, then the future will be set to that value when the function finishes its execution.
* If the submitted function does not have a return value, then the future will be a `bool` that will be set to `true` when the function finishes its execution.

To wait until the future's value becomes available, use the member function `wait()`. To obtain the value itself, use the member function `get()`, which will also automatically wait for the future if it's not ready yet. For example:

```cpp
// Submit a task and get a future.
auto my_future = pool.submit(task);
// Do some other stuff while the task is executing.
do_stuff();
// Get the task's return value from the future, waiting for it to finish running if needed.
auto my_return_value = my_future.get();
```

### Submitting tasks to the queue without futures

Usually, it is best to submit a task to the queue using `submit()`. This allows you to wait for the task to finish and/or get its return value later. However, sometimes a future is not needed, for example when you just want to "set and forget" a certain task, or if the task already communicates with the main thread or with other tasks without using futures, such as via references or pointers. In such cases, you may wish to avoid the overhead involved in assigning a future to the task in order to increase performance.

The member function `push_task()` allows you to submit a task to the queue without generating a future for it. The task can have any number of arguments, but it cannot have a return value. For example:

```cpp
// Submit a task without arguments or return value to the queue.
pool.push_task(task);
// Submit a task with one argument and no return value to the queue.
pool.push_task(task, arg);
// Submit a task with two arguments and no return value to the queue.
pool.push_task(task, arg1, arg2);
```

### Manually waiting for all tasks to complete

To wait for a **single** submitted task to complete, use `submit()` and then use the `wait()` or `get()` member functions of the obtained future. However, in cases where you need to wait until **all** submitted tasks finish their execution, or if the tasks have been submitted without futures using `push_task()`, you can use the member function `wait_for_tasks()`.

Consider, for example, the following code:

```cpp
thread_pool pool;
size_t a[100];
for (size_t i = 0; i < 100; i++)
    pool.push_task([&a, i] { a[i] = i * i; });
std::cout << a[50];
```

The output will most likely be garbage, since the task that modifies `a[50]` has not yet finished executing by the time we try to access that element (in fact, that task is probably still waiting in the queue). One solution would be to use `submit()` instead of `push_task()`, but perhaps we don't want the overhead of generating 100 different futures. Instead, simply adding the line

```cpp
pool.wait_for_tasks();
```

after the `for` loop will ensure - as efficiently as possible - that all tasks have finished running before we attempt to access any elements of the array `a`, and the code will print out the value `2500` as expected. (Note, however, that `wait_for_tasks()` will wait for **all** the tasks in the queue, including those that are unrelated to the `for` loop. Using `parallelize_loop()` would make much more sense in this particular case, as it will wait only for the tasks related to the loop.)

### Parallelizing loops

Consider the following loop:

```cpp
for (T i = start; i < end; i++)
    do_something(i);
```

where:

* `T` is any signed or unsigned integer type.
* The loop is over the range `[start, end)`, i.e. inclusive of `start` but exclusive of `end`.
* `do_something()` is an operation performed for each loop index `i`, such as modifying an array with `end - start` elements.

This loop may be automatically parallelized and submitted to the thread pool's queue using the member function `parallelize_loop()` as follows:

```cpp
auto loop = [](const T &a, const T &b)
{
    for (T i = a; i < b; i++)
        do_something(i);
};
pool.parallelize_loop(start, end, loop, n);
```

The range of indices `[start, end)` will be divided into `n` blocks of the form `[a, b)`. For example, if the range is `[0, 9)` and there are 3 blocks, then the blocks will be the ranges `[0, 3)`, `[3, 6)`, and `[6, 9)`. If possible, the blocks will be equal in size, otherwise the last block may be a bit longer. Then, a task will be submitted for each block, consisting of the function `loop()` with its two arguments being the start and end of the range `[a, b)` of each block. The main thread will then wait until all tasks generated by `parallelize_loop()` finish executing (and only those tasks - not any other tasks that also happen to be in the queue).

In the example above, the lambda function `loop` was defined separately for clarity. In practice, the lambda function will usually be defined within the argument itself, as in the example below. `loop` can also be an ordinary function (with no return value) instead of a lambda function, but that may be less useful, since typically one would like to capture some of the surrounding variables, as below.

If the fourth argument `n` is not specified, the number of blocks will be equal to the number of threads in the pool. For best performance, it is recommended to do your own benchmarks to find the optimal number of blocks for each loop (you can use the `timer` helper class - see [below](#measuring-execution-time)). Using less tasks than there are threads may be preferred if you are also running other tasks in parallel. Using more tasks than there are threads may improve performance in some cases.

As a simple example, the following code will calculate the squares of all integers from 0 to 99. Since there are 10 threads, and we did not specify a fourth argument, the loop will be divided into 10 blocks, each calculating 10 squares:

```cpp
#include "thread_pool.hpp"

int main()
{
    thread_pool pool(10);
    uint32_t squares[100];
    pool.parallelize_loop(0, 100,
                          [&squares](const uint32_t &a, const uint32_t &b)
                          {
                              for (uint32_t i = a; i < b; i++)
                                  squares[i] = i * i;
                          });
    std::cout << "16^2 = " << squares[16] << '\n';
    std::cout << "32^2 = " << squares[32] << '\n';
}
```

The output should be:

```none
16^2 = 256
32^2 = 1024
```

## Helper classes

### Synchronizing printing to an output stream

When printing to an output stream from multiple threads in parallel, the output may become garbled. For example, consider this code:

```cpp
thread_pool pool;
for (auto i = 1; i <= 5; i++)
    pool.push_task([i] {
        std::cout << "Task no. " << i << " executing.\n";
    });
```

The output may look as follows:

```none
Task no. Task no. 2Task no. 5 executing.
Task no.  executing.
Task no. 1 executing.
4 executing.
3 executing.
```

The reason is that, although each **individual** insertion to `std::cout` is thread-safe, there is no mechanism in place to ensure subsequent insertions from the same thread are printed contiguously.

The helper class `synced_stream` is designed to eliminate such synchronization issues. The constructor takes one optional argument, specifying the output stream to print to. If no argument is supplied, `std::cout` will be used:

```cpp
// Construct a synced stream that will print to std::cout.
synced_stream sync_out;
// Construct a synced stream that will print to the output stream my_stream.
synced_stream sync_out(my_stream);
```

The member function `print()` takes an arbitrary number of arguments, which are inserted into the stream one by one, in the order they were given. `println()` does the same, but also prints a newline character `\n` at the end, for convenience. A mutex is used to synchronize this process, so that any other calls to `print()` or `println()` using the same `synced_stream` object must wait until the previous call has finished.

As an example, this code:

```cpp
synced_stream sync_out;
thread_pool pool;
for (auto i = 1; i <= 5; i++)
    pool.push_task([i, &sync_out] {
        sync_out.println("Task no. ", i, " executing.");
    });
```

Will print out:

```none
Task no. 1 executing.
Task no. 2 executing.
Task no. 3 executing.
Task no. 4 executing.
Task no. 5 executing.
```

**Warning:** Always create the `synced_stream` object **before** the `thread_pool` object, as we did in this example. When the `thread_pool` object goes out of scope, it waits for the remaining tasks to be executed. If the `synced_stream` object goes out of scope before the `thread_pool` object, then any tasks using the `synced_stream` will crash. Since objects are destructed in the opposite order of construction, creating the `synced_stream` object before the `thread_pool` object ensures that the `synced_stream` is always available to the tasks, even while the pool is destructing.

### Measuring execution time

If you are using a thread pool, then your code is most likely performance-critical. Achieving maximum performance requires performing a considerable amount of benchmarking to determine the optimal settings and algorithms. Therefore, it is important to be able to measure the execution time of various computations and operations under different conditions.

For example, you may be interested in figuring out:

* The optimal number of threads in the pool.
* The optimal number of tasks to divide a specific operation into, either using `parallelize_loop()` or manually.
* The optimal [sleep duration](#setting-the-worker-functions-sleep-duration) for the worker functions.

The helper class `timer` provides a simple way to measure execution time. It is very straightforward to use:

1. Create a new `timer` object.
2. Immediately before you execute the computation that you want to time, call the `start()` member function.
3. Immediately after the computation ends, call the `stop()` member function.
4. Use the member function `ms()` to obtain the elapsed time for the computation in milliseconds.

For example:

```cpp
timer tmr;
tmr.start();
do_something();
tmr.stop();
std::cout << "The elapsed time was " << tmr.ms() << " ms.\n";
```

## Other features

### Setting the worker function's sleep duration

The **worker function** is the function that controls the execution of tasks by each thread. It loops continuously, and with each iteration of the loop, checks if there are any tasks in the queue. If it finds a task, it pops it out of the queue and executes it. If it does not find a task, it will wait for a bit, by calling `std::this_thread::sleep_for()`, and then check the queue again. The public member variable `sleep_duration` controls the duration, in microseconds, that the worker function sleeps for when it cannot find a task in the queue.

The default value of `sleep_duration` is `1000` microseconds, or `1` millisecond. In our benchmarks, lower values resulted in high CPU usage when the workers were idle. The value of `1000` microseconds was roughly the minimum value needed to reduce the idle CPU usage to a negligible amount.

In addition, in our benchmarks this value resulted in moderately improved performance compared to lower values, since the workers check the queue - which is a costly process - less frequently. On the other hand, increasing the value even more could potentially cause the workers to spend too much time sleeping and not pick up tasks from the queue quickly enough, so `1000` is the "sweet spot".

However, please note that this value is likely unique to the particular system our benchmarks were performed on, and your own optimal value would depend on factors such as your OS and C++ implementation, the type, complexity, and average duration of the tasks submitted to the pool, and whether there are any other programs running at the same time. Therefore, it is strongly recommended to do your own benchmarks and find the value that works best for you.

If `sleep_duration` is set to `0`, then the worker function will execute `std::this_thread::yield()` instead of sleeping if there are no tasks in the queue. This will suggest to the OS that it should put this thread on hold and allow other threads to run instead. However, this also causes the worker functions to have high CPU usage when idle. On the other hand, for some applications this setting may provide better performance than sleeping - again, do your own benchmarks and find what works best for you.

### Monitoring the tasks

Sometimes you may wish to monitor what is happening with the tasks you submitted to the pool. This may be done using three member functions:

* `get_tasks_queued()` gets the number of tasks currently waiting in the queue to be executed by the threads.
* `get_tasks_running()` gets the number of tasks currently being executed by the threads.
* `get_tasks_total()` gets the total number of unfinished tasks - either still in the queue, or running in a thread.
* Note that `get_tasks_running() == get_tasks_total() - get_tasks_queued()`.

These functions are demonstrated in the following program:

```cpp
#include "thread_pool.hpp"

synced_stream sync_out;
thread_pool pool(4);

void sleep_half_second(const size_t &i)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    sync_out.println("Task ", i, " done.");
}

void monitor_tasks()
{
    sync_out.println(pool.get_tasks_total(),
                     " tasks total, ",
                     pool.get_tasks_running(),
                     " tasks running, ",
                     pool.get_tasks_queued(),
                     " tasks queued.");
}

int main()
{
    for (size_t i = 0; i < 12; i++)
        pool.push_task(sleep_half_second, i);
    monitor_tasks();
    std::this_thread::sleep_for(std::chrono::milliseconds(750));
    monitor_tasks();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    monitor_tasks();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    monitor_tasks();
}
```

Assuming you have at least 4 hardware threads (so that 4 tasks can run concurrently), the output will be similar to:

```none
12 tasks total, 0 tasks running, 12 tasks queued.
Task 0 done.
Task 1 done.
Task 2 done.
Task 3 done.
8 tasks total, 4 tasks running, 4 tasks queued.
Task 4 done.
Task 5 done.
Task 6 done.
Task 7 done.
4 tasks total, 4 tasks running, 0 tasks queued.
Task 8 done.
Task 9 done.
Task 10 done.
Task 11 done.
0 tasks total, 0 tasks running, 0 tasks queued.
```

### Pausing the workers

Sometimes you may wish to temporarily pause the execution of tasks, or perhaps you want to submit tasks to the queue but only start executing them at a later time. You can do this using the public member variable `paused`.

When `paused` is set to `true`, the workers will temporarily stop popping new tasks out of the queue. However, any tasks already executed will keep running until they are done, since the thread pool has no control over the internal code of your tasks. If you need to pause a task in the middle of its execution, you must do that manually by programming your own pause mechanism into the task itself. To resume popping tasks, set `paused` back to its default value of `false`.

Here is an example:

```cpp
#include "thread_pool.hpp"

synced_stream sync_out;
thread_pool pool(4);

void sleep_half_second(const size_t &i)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    sync_out.println("Task ", i, " done.");
}

int main()
{
    for (size_t i = 0; i < 8; i++)
        pool.push_task(sleep_half_second, i);
    sync_out.println("Submitted 8 tasks.");
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    pool.paused = true;
    sync_out.println("Pool paused.");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    sync_out.println("Still paused...");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    for (size_t i = 8; i < 12; i++)
        pool.push_task(sleep_half_second, i);
    sync_out.println("Submitted 4 more tasks.");
    sync_out.println("Still paused...");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    pool.paused = false;
    sync_out.println("Pool resumed.");
}
```

Assuming you have at least 4 hardware threads, the output will be similar to:

```none
Submitted 8 tasks.
Pool paused.
Task 0 done.
Task 1 done.
Task 2 done.
Task 3 done.
Still paused...
Submitted 4 more tasks.
Still paused...
Pool resumed.
Task 4 done.
Task 5 done.
Task 6 done.
Task 7 done.
Task 8 done.
Task 9 done.
Task 10 done.
Task 11 done.
```

Here is what happened. We initially submitted a total of 8 tasks to the queue. Since we waited for 250ms before pausing, the first 4 tasks have already started running, so they kept running until they finished. While the pool was paused, we submitted 4 more tasks to the queue, but they just waited at the end of the queue. When we resumed, the remaining 4 initial tasks were executed, followed by the 4 new tasks.

While the workers are paused, `wait_for_tasks()` will wait for the running tasks instead of all tasks (otherwise it would wait forever). This is demonstrated by the following program:

```cpp
#include "thread_pool.hpp"

synced_stream sync_out;
thread_pool pool(4);

void sleep_half_second(const size_t &i)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    sync_out.println("Task ", i, " done.");
}

int main()
{
    for (size_t i = 0; i < 8; i++)
        pool.push_task(sleep_half_second, i);
    sync_out.println("Submitted 8 tasks. Waiting for them to complete.");
    pool.wait_for_tasks();
    for (size_t i = 8; i < 20; i++)
        pool.push_task(sleep_half_second, i);
    sync_out.println("Submitted 12 more tasks.");
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    pool.paused = true;
    sync_out.println("Pool paused. Waiting for the ", pool.get_tasks_running(), " running tasks to complete.");
    pool.wait_for_tasks();
    sync_out.println("All running tasks completed. ", pool.get_tasks_queued(), " tasks still queued.");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    sync_out.println("Still paused...");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    sync_out.println("Still paused...");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    pool.paused = false;
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    sync_out.println("Pool resumed. Waiting for the remaining ", pool.get_tasks_total(), " tasks (", pool.get_tasks_running(), " running and ", pool.get_tasks_queued(), " queued) to complete.");
    pool.wait_for_tasks();
    sync_out.println("All tasks completed.");
}
```

The output should be similar to:

```none
Submitted 8 tasks. Waiting for them to complete.
Task 0 done.
Task 1 done.
Task 2 done.
Task 3 done.
Task 4 done.
Task 5 done.
Task 6 done.
Task 7 done.
Submitted 12 more tasks.
Pool paused. Waiting for the 4 running tasks to complete.
Task 8 done.
Task 9 done.
Task 10 done.
Task 11 done.
All running tasks completed. 8 tasks still queued.
Still paused...
Still paused...
Pool resumed. Waiting for the remaining 8 tasks (4 running and 4 queued) to complete.
Task 12 done.
Task 13 done.
Task 14 done.
Task 15 done.
Task 16 done.
Task 17 done.
Task 18 done.
Task 19 done.
All tasks completed.
```

The first `wait_for_tasks()`, which was called with `paused == false`, waited for all 8 tasks, both running and queued. The second `wait_for_tasks()`, which was called with `paused == true`, only waited for the 4 running tasks, while the other 8 tasks remained queued, and were not executed since the pool was paused. Finally, the third `wait_for_tasks()`, which was called with `paused == false`, waited for the remaining 8 tasks, both running and queued.

**Warning**: If the thread pool is destroyed while paused, any tasks still in the queue will never be executed.

### Exception handling

`submit()` catches any exceptions thrown by the submitted task and forwards them to the corresponding future. They can then be caught when invoking the `get()` member function of the future. For example:

```cpp
#include "thread_pool.hpp"

double inverse(const double &x)
{
    if (x == 0)
        throw std::runtime_error("Division by zero!");
    else
        return 1 / x;
}

int main()
{
    thread_pool pool;
    auto my_future = pool.submit(inverse, 0);
    try
    {
        double result = my_future.get();
        std::cout << "The result is: " << result << '\n';
    }
    catch (const std::exception &e)
    {
        std::cout << "Caught exception: " << e.what() << '\n';
    }
}
```

The output will be:

```none
Caught exception: Division by zero!
```

## Testing the package

The included file `thread_pool_test.cpp` will perform automated tests of all aspects of the package, and benchmark some multithreaded matrix operations. The output will be printed both to `std::cout` and to a file named `thread_pool_test-yyyy-mm-dd_hh.mm.ss.log` based on the current date and time. In addition, the code is thoroughly documented, and is meant to serve as an extensive example of how to properly use the package.

Please make sure to:

1. [Compile](#compiling-and-compatibility) `thread_pool_test.cpp` with optimization flags enabled (e.g. `-O3` on GCC / Clang or `/O2` on MSVC).
2. Run the test without any other applications, especially multithreaded applications, running in parallel.

If any of the tests fail, please [submit a bug report](https://github.com/bshoshany/thread-pool/issues) including the exact specifications of your system (OS, CPU, compiler, etc.) and the generated log file.

### Automated tests

A sample output of a successful run of the automated tests is as follows:

```none
A C++17 Thread Pool for High-Performance Scientific Computing
(c) 2021 Barak Shoshany (baraksh@gmail.com) (http://baraksh.com)
GitHub: https://github.com/bshoshany/thread-pool

Thread pool library version is v2.0.0 (2021-08-14).
Hardware concurrency is 24.
Generating log file: thread_pool_test-2021-08-14_23.34.25.log.

Important: Please do not run any other applications, especially multithreaded applications, in parallel with this test!

====================================
Checking that the constructor works:
====================================
Checking that the thread pool reports a number of threads equal to the hardware concurrency...
-> PASSED!
Checking that the manually counted number of unique thread IDs is equal to the reported number of threads...
-> PASSED!

============================
Checking that reset() works:
============================
Checking that after reset() the thread pool reports a number of threads equal to half the hardware concurrency...
-> PASSED!
Checking that after reset() the manually counted number of unique thread IDs is equal to the reported number of threads...
-> PASSED!
Checking that after a second reset() the thread pool reports a number of threads equal to the hardware concurrency...
-> PASSED!
Checking that after a second reset() the manually counted number of unique thread IDs is equal to the reported number of threads...
-> PASSED!

================================
Checking that push_task() works:
================================
Checking that push_task() works for a function with no arguments or return value...
-> PASSED!
Checking that push_task() works for a function with one argument and no return value...
-> PASSED!
Checking that push_task() works for a function with two arguments and no return value...
-> PASSED!

=============================
Checking that submit() works:
=============================
Checking that submit() works for a function with no arguments or return value...
-> PASSED!
Checking that submit() works for a function with one argument and no return value...
-> PASSED!
Checking that submit() works for a function with two arguments and no return value...
-> PASSED!
Checking that submit() works for a function with no arguments and a return value...
-> PASSED!
Checking that submit() works for a function with one argument and a return value...
-> PASSED!
Checking that submit() works for a function with two arguments and a return value...
-> PASSED!

=======================================
Checking that wait_for_tasks() works...
=======================================
-> PASSED!

=======================================
Checking that parallelize_loop() works:
=======================================
Verifying that a loop from -2064 to 551 with 4 tasks modifies all indices...
-> PASSED!
Verifying that a loop from -658 to -77 with 19 tasks modifies all indices...
-> PASSED!
Verifying that a loop from 1512 to -1046 with 1 task modifies all indices...
-> PASSED!
Verifying that a loop from -2334 to -1770 with 23 tasks modifies all indices...
-> PASSED!
Verifying that a loop from 1775 to -1242 with 13 tasks modifies all indices...
-> PASSED!
Verifying that a loop from 846 to -506 with 14 tasks modifies all indices...
-> PASSED!
Verifying that a loop from -301 to -2111 with 5 tasks modifies all indices...
-> PASSED!
Verifying that a loop from 1758 to -1602 with 11 tasks modifies all indices...
-> PASSED!
Verifying that a loop from 94 to -1103 with 24 tasks modifies all indices...
-> PASSED!
Verifying that a loop from 612 to 2026 with 13 tasks modifies all indices...
-> PASSED!

======================================================
Checking that different values of sleep_duration work:
======================================================
Submitting tasks with sleep_duration = 0 microseconds...
-> PASSED!
Submitting tasks with sleep_duration = 1909 microseconds...
-> PASSED!
Submitting tasks with sleep_duration = 469 microseconds...
-> PASSED!
Submitting tasks with sleep_duration = 964 microseconds...
-> PASSED!
Submitting tasks with sleep_duration = 1946 microseconds...
-> PASSED!
Submitting tasks with sleep_duration = 773 microseconds...
-> PASSED!
Resetting sleep_duration to the default value (1000 microseconds).

====================================
Checking that task monitoring works:
====================================
Resetting pool to 4 threads.
Submitting 12 tasks.
After submission, should have: 12 tasks total, 4 tasks running, 8 tasks queued...
-> PASSED!
Task 1 released.
Task 3 released.
Task 0 released.
Task 2 released.
After releasing 4 tasks, should have: 8 tasks total, 4 tasks running, 4 tasks queued...
Task 5 released.
Task 4 released.
Task 7 released.
Task 6 released.
-> PASSED!
After releasing 4 more tasks, should have: 4 tasks total, 4 tasks running, 0 tasks queued...
-> PASSED!
Task 11 released.
Task 8 released.
Task 9 released.
Task 10 released.
After releasing the final 4 tasks, should have: 0 tasks total, 0 tasks running, 0 tasks queued...
-> PASSED!
Resetting pool to 24 threads.

============================
Checking that pausing works:
============================
Resetting pool to 4 threads.
Pausing pool.
Submitting 12 tasks, each one waiting for 200ms.
Immediately after submission, should have: 12 tasks total, 0 tasks running, 12 tasks queued...
-> PASSED!
300ms later, should still have: 12 tasks total, 0 tasks running, 12 tasks queued...
-> PASSED!
Unpausing pool.
Task 1 done.
Task 2 done.
Task 3 done.
Task 0 done.
300ms later, should have: 8 tasks total, 4 tasks running, 4 tasks queued...
-> PASSED!
Pausing pool and using wait_for_tasks() to wait for the running tasks.
Task 7 done.
Task 5 done.
Task 6 done.
Task 4 done.
After waiting, should have: 4 tasks total, 0 tasks running, 4 tasks queued...
-> PASSED!
200ms later, should still have: 4 tasks total, 0 tasks running, 4 tasks queued...
-> PASSED!
Unpausing pool and using wait_for_tasks() to wait for all tasks.
Task 9 done.
Task 8 done.
Task 10 done.
Task 11 done.
After waiting, should have: 0 tasks total, 0 tasks running, 0 tasks queued...
-> PASSED!
Resetting pool to 24 threads.

=======================================
Checking that exception handling works:
=======================================
-> PASSED!

============================================================
Testing that matrix operations produce the expected results:
============================================================
Using matrices of size 240x240 with a total of 57600 elements.
Adding two matrices (single-threaded)...
Adding two matrices (multithreaded)...
Comparing the results...
-> PASSED!
Transposing a matrix (single-threaded)...
Transposing a matrix (multithreaded)...
Comparing the results...
-> PASSED!
Multiplying two matrices (single-threaded)...
Multiplying two matrices (multithreaded)...
Comparing the results...
-> PASSED!

++++++++++++++++++++++++++++++
SUCCESS: Passed all 46 checks!
++++++++++++++++++++++++++++++
```

### Performance tests

If all checks passed, `thread_pool_test.cpp` will perform benchmarking of multithreaded matrix operations. Here we will present the results obtained with two different systems.

The first test was performed on a high-end desktop computer equipped with a 12-core / 24-thread AMD Ryzen 9 3900X CPU at 3.8 GHz and 32 GB of DDR4 RAM at 3600 MHz, compiled using GCC v11.2.0 on Windows 10 build 19043.1165 with the `-O3` compiler flag. The thread pool used 22 out of 24 threads, leaving 2 threads free for the operating system - which in our tests increased performance, presumably since all 22 threads could be dedicated entirely to the test. The output was as follows:

```none
===================================
Performing matrix performance test:
===================================
Using 22 out of 24 threads.
Determining the optimal sleep duration........................
Result: The optimal sleep duration is 300 microseconds.

Adding two 4400x4400 matrices 20 times:
With   1  task, mean execution time was   39.3 ms with standard deviation  2.4 ms.
With   5 tasks, mean execution time was   21.2 ms with standard deviation  1.7 ms.
With  11 tasks, mean execution time was   20.4 ms with standard deviation  1.1 ms.
With  22 tasks, mean execution time was   18.3 ms with standard deviation  1.3 ms.
With  44 tasks, mean execution time was   17.4 ms with standard deviation  0.7 ms.
With  88 tasks, mean execution time was   18.0 ms with standard deviation  1.0 ms.
Maximum speedup obtained: 2.3x.

Transposing one 4400x4400 matrix 20 times:
With   1  task, mean execution time was  139.8 ms with standard deviation  3.0 ms.
With   5 tasks, mean execution time was   38.2 ms with standard deviation  2.4 ms.
With  11 tasks, mean execution time was   23.3 ms with standard deviation  1.8 ms.
With  22 tasks, mean execution time was   18.9 ms with standard deviation  1.6 ms.
With  44 tasks, mean execution time was   19.5 ms with standard deviation  1.5 ms.
With  88 tasks, mean execution time was   18.1 ms with standard deviation  0.7 ms.
Maximum speedup obtained: 7.7x.

Multiplying two 550x550 matrices 20 times:
With   1  task, mean execution time was  165.2 ms with standard deviation  2.5 ms.
With   5 tasks, mean execution time was   35.9 ms with standard deviation  1.0 ms.
With  11 tasks, mean execution time was   17.6 ms with standard deviation  0.5 ms.
With  22 tasks, mean execution time was   10.2 ms with standard deviation  0.7 ms.
With  44 tasks, mean execution time was   16.1 ms with standard deviation  1.4 ms.
With  88 tasks, mean execution time was   15.4 ms with standard deviation  0.7 ms.
Maximum speedup obtained: 16.2x.

Generating random 4400x4400 matrix 20 times:
With   1  task, mean execution time was  244.7 ms with standard deviation  2.6 ms.
With   5 tasks, mean execution time was   51.5 ms with standard deviation  1.5 ms.
With  11 tasks, mean execution time was   25.7 ms with standard deviation  0.9 ms.
With  22 tasks, mean execution time was   19.1 ms with standard deviation  2.7 ms.
With  44 tasks, mean execution time was   17.2 ms with standard deviation  2.1 ms.
With  88 tasks, mean execution time was   15.8 ms with standard deviation  1.0 ms.
Maximum speedup obtained: 15.5x.

Overall, multithreading provided speedups of up to 16.2x.

+++++++++++++++++++++++++++++++++++++++
Thread pool performance test completed!
+++++++++++++++++++++++++++++++++++++++
```

Here are some lessons we can learn from these results:

* For simple element-wise operations such as addition, multithreading improves performance very modestly, only by a factor of 2.3, even when utilizing 22 threads in parallel. This is because compiler optimizations already parallelize simple loops fairly well on their own. Omitting the `-O3` optimization flag, we observed a 6.8x speedup for addition. However, the user will most likely be compiling with optimizations turned on anyway.
* Transposition enjoys a moderate 7.7x speedup with multithreading. Note that transposition requires reading memory is non-sequential order, jumping between the rows of the source matrix, which is why, compared to sequential operations such as addition, it is much slower when single-threaded, and benefits more from multithreading.
* Matrix multiplication and random matrix generation, which are more complicated operations that cannot be automatically parallelized by compiler optimizations, gain the most out of multithreading - with a very significant speedup by a factor of around 16 on average. Given that the test CPU only has 12 physical cores, and hyperthreading can generally produce no more than a 30% performance improvement, a 16x speedup is about as good as can be expected.
* Using as many tasks as there are threads almost always provides the best performance. Although in some cases 44 or 88 tasks seem to provide a slightly lower mean execution time compared to 22 tasks, the difference is within less than 1 standard deviation in all cases.

### Dual Intel Xeon Gold 6148 (80 threads)

The second test was performed on a [Compute Canada](https://www.computecanada.ca/) node equipped with dual 20-core / 40-thread Intel Xeon Gold 6148 CPUs at 2.4 GHz (for a total of 40 cores and 80 threads) and 202 GB of RAM, compiled using GCC v9.4.0 on CentOS Linux 7.6.1810 with the `-O3` compiler flag. The thread pool consisted of 78 threads. The output was as follows:

```none
===================================
Performing matrix performance test:
===================================
Using 78 out of 80 threads.
Determining the optimal sleep duration........................
Result: The optimal sleep duration is 1000 microseconds.

Adding two 15600x15600 matrices 20 times:
With   1  task, mean execution time was  846.1 ms with standard deviation 40.2 ms.
With  19 tasks, mean execution time was   88.1 ms with standard deviation  8.6 ms.
With  39 tasks, mean execution time was   73.5 ms with standard deviation  4.8 ms.
With  78 tasks, mean execution time was   67.3 ms with standard deviation  2.2 ms.
With 156 tasks, mean execution time was   64.9 ms with standard deviation  2.3 ms.
With 312 tasks, mean execution time was   65.8 ms with standard deviation  1.5 ms.
Maximum speedup obtained: 13.0x.

Transposing one 15600x15600 matrix 20 times:
With   1  task, mean execution time was 1689.4 ms with standard deviation 75.3 ms.
With  19 tasks, mean execution time was  155.3 ms with standard deviation 19.7 ms.
With  39 tasks, mean execution time was  115.0 ms with standard deviation 10.8 ms.
With  78 tasks, mean execution time was   99.0 ms with standard deviation  6.0 ms.
With 156 tasks, mean execution time was   96.2 ms with standard deviation  1.6 ms.
With 312 tasks, mean execution time was   97.8 ms with standard deviation  1.7 ms.
Maximum speedup obtained: 17.6x.

Multiplying two 1950x1950 matrices 20 times:
With   1  task, mean execution time was 15415.1 ms with standard deviation 672.5 ms.
With  19 tasks, mean execution time was 1152.5 ms with standard deviation 62.8 ms.
With  39 tasks, mean execution time was  537.9 ms with standard deviation  4.1 ms.
With  78 tasks, mean execution time was  292.3 ms with standard deviation 42.5 ms.
With 156 tasks, mean execution time was  936.4 ms with standard deviation 15.8 ms.
With 312 tasks, mean execution time was  951.2 ms with standard deviation 22.3 ms.
Maximum speedup obtained: 52.7x.

Generating random 15600x15600 matrix 20 times:
With   1  task, mean execution time was 4318.3 ms with standard deviation  6.3 ms.
With  19 tasks, mean execution time was  260.8 ms with standard deviation 15.1 ms.
With  39 tasks, mean execution time was  156.1 ms with standard deviation  1.6 ms.
With  78 tasks, mean execution time was   86.2 ms with standard deviation  1.9 ms.
With 156 tasks, mean execution time was   84.8 ms with standard deviation  0.4 ms.
With 312 tasks, mean execution time was   85.2 ms with standard deviation  1.3 ms.
Maximum speedup obtained: 51.0x.

Overall, multithreading provided speedups of up to 52.7x.

+++++++++++++++++++++++++++++++++++++++
Thread pool performance test completed!
+++++++++++++++++++++++++++++++++++++++
```

The speedup of around 51.9x on average for matrix multiplication and random matrix generation again matches the estimation of a 30% improvement in performance over the 40 physical CPU cores due to hyperthreading, which indicates that we are once again saturating the maximum possible performance of our system.

## Issue and pull request policy

This package is under continuous and active development. If you encounter any bugs, or if you would like to request any additional features, please feel free to [open a new issue on GitHub](https://github.com/bshoshany/thread-pool/issues) and I will look into it as soon as I can.

Contributions are always welcome. However, I release my projects in cumulative updates after editing them locally on my system, so my policy is not to accept any pull requests. If you open a pull request, and I decide to incorporate it into the code, I will first perform some tests to ensure that the change doesn't break anything, and then merge it into the next release of the project, possibly together with some other changes, and along with a version bump and a corresponding note in `CHANGELOG.md` with a link to the pull request.

## Copyright and citing

Copyright (c) 2021 [Barak Shoshany](http://baraksh.com). Licensed under the [MIT license](LICENSE.txt).

If you use this library in published research, please cite it as follows:

* Barak Shoshany, "A C++17 Thread Pool for High-Performance Scientific Computing", [doi:10.5281/zenodo.4742687](https://doi.org/10.5281/zenodo.4742687), [arXiv:2105.00613](https://arxiv.org/abs/2105.00613) (May 2021)

You can use the following BibTeX entry:

```none
@article{Shoshany2021_ThreadPool,
    archiveprefix = {arXiv},
    author        = {Barak Shoshany},
    doi           = {10.5281/zenodo.4742687},
    eid           = {arXiv:2105.00613},
    eprint        = {2105.00613},
    journal       = {arXiv e-prints},
    keywords      = {Computer Science - Distributed, Parallel, and Cluster Computing, D.1.3, D.1.5},
    month         = {May},
    primaryclass  = {cs.DC},
    title         = {{A C++17 Thread Pool for High-Performance Scientific Computing}},
    year          = {2021}
}
```
