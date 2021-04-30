# A simple but powerful C++17 thread pool class

<!-- TOC depthFrom:2 -->

- [Introduction](#introduction)
- [Features](#features)
- [Basic usage](#basic-usage)
    - [Including the library](#including-the-library)
    - [Constructors](#constructors)
    - [Submitting tasks to the queue](#submitting-tasks-to-the-queue)
    - [Parallelizing loops](#parallelizing-loops)
- [Advanced usage](#advanced-usage)
    - [Getting and resetting the number of threads in the pool](#getting-and-resetting-the-number-of-threads-in-the-pool)
    - [Submitting tasks to the queue without futures](#submitting-tasks-to-the-queue-without-futures)
    - [Manually waiting for all tasks to complete](#manually-waiting-for-all-tasks-to-complete)
    - [Setting the worker function's sleep duration](#setting-the-worker-functions-sleep-duration)
- [Synchronizing printing to an output stream](#synchronizing-printing-to-an-output-stream)
- [Measuring execution time](#measuring-execution-time)
- [Compiling](#compiling)
- [Version history](#version-history)
- [Feedback](#feedback)
- [Author and copyright](#author-and-copyright)

<!-- /TOC -->

<a id="markdown-introduction" name="introduction"></a>
## Introduction

Multithreading is essential for modern high-performance computing. Since C++11, the C++ standard library has included built-in low-level multithreading support using constructs such as `std::thread`. However, `std::thread` creates a new thread each time it is called, which can have a significant performance overhead. Furthermore, it is possible to create more threads than the hardware can handle simultaneously, potentially resulting in a substantial slowdown.

This library contains a thread pool class, `thread_pool`, which avoids these issues by creating a fixed pool of threads once and for all, and then reusing the same threads to perform different tasks throughout the lifetime of the pool. By default, the number of threads in the pool is equal to the maximum number of threads that the hardware can run in parallel.

The user submits tasks to be executed into a queue. Whenever a thread becomes available, it pops a task from the queue and executes it. Each task is automatically assigned an `std::future`, which can be used to wait for the task to finish executing and/or obtain its eventual return value.

In addition to `std::thread`, the C++ standard library also offers the higher-level construct `std::async`, which may internally utilize a thread pool - but this is not guaranteed, and in fact, currently only the MSVC implementation of `std::async` uses a thread pool, while GCC and Clang do not. Using this custom-made thread pool class instead of `std::async` allows the user more control, transparency, and portability.

<a id="markdown-features" name="features"></a>
## Features

* **Fast:**
    * Built from scratch with maximum performance in mind.
    * Suitable for use in high-performance computing clusters with a very large number of CPU cores.
    * Compact code, to reduce both compilation time and binary size.
    * Reusing threads avoids the overhead of creating and destroying them for individual tasks.
    * A task queue ensures that there are never more threads running in parallel than allowed by the hardware.
* **Lightweight:**
    * Only ~160 lines of code, excluding comments, blank lines, and the two optional helper classes.
    * Single header file: simply `#include "thread_pool.hpp"`, and you're done.
    * Header-only: no need to install or build the library.
    * Self-contained: no external requirements or dependencies. Does not require OpenMP or any other multithreading APIs. Only uses the C++ standard library, and works with any C++17-compliant compiler.
* **Easy to use:**
    * Very simple operation, using a handful of member functions.
    * Every task submitted to the queue automatically generates an `std::future`, which can be used to wait for the task to finish executing and/or obtain its eventual return value.
    * Optionally, tasks may also be submitted without generating a future, sacrificing convenience for extra performance.
    * The code is thoroughly documented using Doxygen comments - not only the interface, but also the implementation, in case you would like to make modifications.
* **Additional features:**
    * Automatically parallelize a loop into any number of parallel tasks.
    * Easily wait for all tasks in the queue to complete.
    * Change the number of threads in the pool safely as needed.
    * Fine-tune the sleep duration of each thread's worker function for optimal performance.
    * Synchronize output to a stream from multiple threads in parallel using the `synced_stream` helper class.
    * Easily measure execution time for benchmarking purposes using the `timer` helper class.

<a id="markdown-basic-usage" name="basic-usage"></a>
## Basic usage

<a id="markdown-including-the-library" name="including-the-library"></a>
### Including the library

To use the thread pool library, simply include the header file:

```cpp
#include "thread_pool.hpp"
```

The thread pool will now be accessible via the `thread_pool` class.

<a id="markdown-constructors" name="constructors"></a>
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

<a id="markdown-submitting-tasks-to-the-queue" name="submitting-tasks-to-the-queue"></a>
### Submitting tasks to the queue

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

Please see [the C++ reference entry for `std::future`](https://en.cppreference.com/w/cpp/thread/future) for more information on using futures.

<a id="markdown-parallelizing-loops" name="parallelizing-loops"></a>
### Parallelizing loops

Consider the following loop:

```cpp
for (T i = start; i <= end; i++)
    loop(i);
```

where:

* `T` is any signed or unsigned integer type.
* `start` is the first index to loop over (inclusive).
* `end` is the last index to loop over (inclusive).
* `loop()` is a function that takes exactly one argument, the loop index, and has no return value.

This loop may be automatically parallelized and submitted to the thread pool's queue using the member function `parallelize_loop()` as follows:

```cpp
// Equivalent to the above loop, but will be automatically parallelized.
pool.parallelize_loop(start, end, loop);
```

The loop will be parallelized into a number of tasks equal to the number of threads in the pool, with each task executing the function `loop()` for a roughly equal number of indices. The main thread will then wait until all tasks generated by `parallelize_loop()` finish executing (and only those tasks - not any other tasks that also happen to be in the queue).

If desired, the number of parallel tasks may be manually specified using a fourth argument:

```cpp
// Parallelize the loop into 12 parallel tasks
pool.parallelize_loop(start, end, loop, 12);
```

For best performance, it is recommended to do your own benchmarks to find the optimal number of tasks for each loop (you can use the `timer` helper class - see [below](#measuring-execution-time)). Using less tasks than there are threads may be preferred if you are also running other tasks in parallel. Using more tasks than there are threads may improve performance in some cases; see the documentation for [my multithreaded matrix class template](https://github.com/bshoshany/multithreaded-matrix) for analysis.

<a id="markdown-advanced-usage" name="advanced-usage"></a>
## Advanced usage

<a id="markdown-getting-and-resetting-the-number-of-threads-in-the-pool" name="getting-and-resetting-the-number-of-threads-in-the-pool"></a>
### Getting and resetting the number of threads in the pool

The member function `get_thread_count()` returns the number of threads in the pool. This will be equal to `std::thread::hardware_concurrency()` if the default constructor has been used.

It is generally unnecessary to change the number of threads in the pool after it has been created, since the whole point of a thread pool is that you only create the threads once. However, if needed, this can be done - safely - using the `reset()` member function, which waits for all submitted tasks to be completed, then destroys all of the threads and creates a new thread pool with the desired new number of threads, as specified in the function's argument. If no argument is given to `reset()`, the new number of threads will be the hardware concurrency.

<a id="markdown-submitting-tasks-to-the-queue-without-futures" name="submitting-tasks-to-the-queue-without-futures"></a>
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

<a id="markdown-manually-waiting-for-all-tasks-to-complete" name="manually-waiting-for-all-tasks-to-complete"></a>
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

<a id="markdown-setting-the-worker-functions-sleep-duration" name="setting-the-worker-functions-sleep-duration"></a>
### Setting the worker function's sleep duration

The **worker function** is the function that controls the execution of tasks by each thread. It loops continuously, and with each iteration of the loop, checks if there are any tasks in the queue. If it finds a task, it pops it out of the queue and executes it. If it does not find a task, it will wait for a bit, by calling `std::this_thread::sleep_for()`, and then check the queue again. The public member variable `sleep_duration` controls the duration, in microseconds, that the worker function sleeps for when it cannot find a task in the queue.

The default value of `sleep_duration` is `1000` microseconds, or `1` millisecond. In my benchmarks, lower values resulted in high CPU usage when the workers were idle. The value of `1000` microseconds was roughly the minimum value needed to reduce the idle CPU usage to a negligible amount.

In addition, this value resulted in moderately improved performance compared to lower values, since the workers check the queue - which is a costly process - less frequently. On the other hand, increasing the value even more could potentially cause the workers to spend too much time sleeping and not pick up tasks from the queue quickly enough, so `1000` is the "sweet spot".

However, please note that this value is likely unique to my particular system, and your own optimal value would depend on factors such as your OS and C++ implementation, the type, complexity, and average duration of the tasks submitted to the pool, and whether there are any other programs running at the same time. Therefore, it is strongly recommended to do your own benchmarks (you can use the `timer` helper class - see [below](#measuring-execution-time)) and find the value that works best for you.

If `sleep_duration` is set to `0`, then instead of sleeping, the worker function will execute `std::this_thread::yield()` if there are no tasks in the queue. This will suggest to the OS that it should put this thread on hold and allow other threads to run instead. However, this also causes the worker functions to have high CPU usage when idle. On the other hand, for some applications this setting may provide better performance than sleeping - again, do your own benchmarks and find what works best for you.

<a id="markdown-synchronizing-printing-to-an-output-stream" name="synchronizing-printing-to-an-output-stream"></a>
## Synchronizing printing to an output stream

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

<a id="markdown-measuring-execution-time" name="measuring-execution-time"></a>
## Measuring execution time

If you are using a thread pool, then your code is most likely performance-critical. Achieving maximum performance requires performing a considerable amount of benchmarking to determine the optimal settings and algorithms. Therefore, it is important to be able to measure the execution time of various computations under different conditions. In the context of the thread pool class, you would probably be interested in finding the optimal number of threads in the pool and the optimal sleep duration for the worker functions.

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

For a detailed example of using the `timer` class for benchmarking, please see the code and documentation for [my multithreaded matrix class template](https://github.com/bshoshany/multithreaded-matrix).

<a id="markdown-compiling" name="compiling"></a>
## Compiling

This library was tested with a 12-core / 24-thread AMD Ryzen 9 3900X CPU using the following compilers and platforms:

* GCC v10.2.0 on Windows 10 build 19042.928 and Ubuntu 20.04.2 LTS.
* Clang 11.0.0 on Windows 10 build 19042.928 and Ubuntu 20.04.2 LTS.
* MSVC v14.28.29910 on Windows 10 build 19042.928.

As this library requires C++17 features, the code must be compiled with C++17 support. For GCC or Clang, use the `-std=c++17` flag. For MSVC, use `/std:c++17`. On Linux, you will also need to use the `-pthread` flag with GCC or Clang to enable the POSIX threads library.

<a id="markdown-version-history" name="version-history"></a>
## Version history

* Version 1.2 (2021-04-29)
    * The worker function, which controls the execution of tasks by each thread, now sleeps by default instead of yielding. Previously, when the worker could not find any tasks in the queue, it called `std::this_thread::yield()` and then tried again. However, this caused the workers to have high CPU usage when idle, [as reported by some users](https://github.com/bshoshany/thread-pool/issues/1). Now, when the worker function cannot find a task to run, it instead sleeps for a duration given by the public member variable `sleep_duration` (in microseconds) before checking the queue again. The default value is `1000` microseconds, which I found to be optimal in terms of both CPU usage and performance, but your own optimal value may be different.
    * If the constructor is called with an argument of zero for the number of threads, then the default value, `std::thread::hardware_concurrency()`, is used instead.
    * Added a simple helper class, `timer`, which can be used to measure execution time for benchmarking purposes.
    * Improved and expanded the documentation.
* Version 1.1 (2021-04-24)
    * Cosmetic changes only. Fixed a typo in the Doxygen comments and added a link to the GitHub repository.
* Version 1.0 (2021-01-15)
    * Initial release.

<a id="markdown-feedback" name="feedback"></a>
## Feedback

If you would like a request any additional features, or if you encounter any bugs, please feel free to open a new issue!

<a id="markdown-author-and-copyright" name="author-and-copyright"></a>
## Author and copyright

Copyright (c) 2021 [Barak Shoshany](http://baraksh.com) (baraksh@gmail.com). Licensed under the [MIT license](LICENSE.txt).

If you use this class in your code, please acknowledge the author and provide a link to the [GitHub repository](https://github.com/bshoshany/thread-pool). Thank you!
