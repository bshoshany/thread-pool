[![Static Badge](https://img.shields.io/badge/DOI-10.5281%2Fzenodo.4742687-b31b1b)
](https://doi.org/10.5281/zenodo.4742687)
[![arXiv:2105.00613](https://img.shields.io/badge/arXiv-2105.00613-b31b1b)
](https://arxiv.org/abs/2105.00613)
[![License: MIT](https://img.shields.io/github/license/bshoshany/thread-pool)](https://github.com/bshoshany/thread-pool/blob/master/LICENSE.txt)
[![Language: C++17](https://img.shields.io/badge/Language-C%2B%2B17-yellow)](https://cppreference.com)
[![GitHub release](https://img.shields.io/github/v/release/bshoshany/thread-pool)](https://github.com/bshoshany/thread-pool/releases)
[![Vcpkg Version](https://img.shields.io/vcpkg/v/bshoshany-thread-pool)](https://vcpkg.io/)
[![Conan Version](https://img.shields.io/conan/v/bshoshany-thread-pool)](https://conan.io/center/recipes/bshoshany-thread-pool)
[![GitHub Repo stars](https://img.shields.io/github/stars/bshoshany/thread-pool?color=009999)](https://github.com/bshoshany/thread-pool/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/bshoshany/thread-pool?color=009999)](https://github.com/bshoshany/thread-pool/forks)
[![Open in Visual Studio Code](https://img.shields.io/badge/Open_in_Visual_Studio_Code-007acc)](https://vscode.dev/github/bshoshany/thread-pool)
[![Barak Shoshany's Website](https://img.shields.io/badge/Barak_Shoshany's_Website-009933)](https://baraksh.com/)

# `BS::thread_pool`: a fast, lightweight, and easy-to-use C++17 thread pool library

By Barak Shoshany\
Email: <baraksh@gmail.com>\
Website: <https://baraksh.com/>\
GitHub: <https://github.com/bshoshany>

This is the complete documentation for v4.0.1 of the library, released on 2023-12-28.

* [Introduction](#introduction)
    * [Motivation](#motivation)
    * [Overview of features](#overview-of-features)
    * [Compiling and compatibility](#compiling-and-compatibility)
* [Getting started](#getting-started)
    * [Installing the library](#installing-the-library)
    * [Constructors](#constructors)
    * [Getting and resetting the number of threads in the pool](#getting-and-resetting-the-number-of-threads-in-the-pool)
    * [Finding the version of the library](#finding-the-version-of-the-library)
* [Submitting tasks to the queue](#submitting-tasks-to-the-queue)
    * [Submitting tasks with no arguments and receiving a future](#submitting-tasks-with-no-arguments-and-receiving-a-future)
    * [Submitting tasks with arguments and receiving a future](#submitting-tasks-with-arguments-and-receiving-a-future)
    * [Detaching and waiting for tasks](#detaching-and-waiting-for-tasks)
    * [Waiting for submitted or detached tasks with a timeout](#waiting-for-submitted-or-detached-tasks-with-a-timeout)
    * [Class member functions as tasks](#class-member-functions-as-tasks)
* [Parallelizing loops](#parallelizing-loops)
    * [Automatic parallelization of loops](#automatic-parallelization-of-loops)
    * [Parallelizing loops without futures](#parallelizing-loops-without-futures)
    * [Parallelizing individual indices vs. blocks](#parallelizing-individual-indices-vs-blocks)
    * [Loops with return values](#loops-with-return-values)
    * [Parallelizing sequences](#parallelizing-sequences)
    * [More about `BS::multi_future<T>`](#more-about-bsmulti_futuret)
* [Utility classes](#utility-classes)
    * [Synchronizing printing to a stream with `BS::synced_stream`](#synchronizing-printing-to-a-stream-with-bssynced_stream)
    * [Measuring execution time with `BS::timer`](#measuring-execution-time-with-bstimer)
    * [Sending simple signals between threads with `BS::signaller`](#sending-simple-signals-between-threads-with-bssignaller)
* [Managing tasks](#managing-tasks)
    * [Monitoring the tasks](#monitoring-the-tasks)
    * [Purging tasks](#purging-tasks)
    * [Exception handling](#exception-handling)
    * [Getting information about the threads](#getting-information-about-the-threads)
    * [Thread pool initialization functions](#thread-pool-initialization-functions)
    * [Passing task arguments by constant reference](#passing-task-arguments-by-constant-reference)
* [Optional features](#optional-features)
    * [Pausing the pool](#pausing-the-pool)
    * [Avoiding wait deadlocks](#avoiding-wait-deadlocks)
    * [Accessing native thread handles](#accessing-native-thread-handles)
    * [Setting task priority](#setting-task-priority)
* [Testing the library](#testing-the-library)
    * [Automated tests](#automated-tests)
    * [Performance tests](#performance-tests)
* [Installing the library using package managers](#installing-the-library-using-package-managers)
    * [Installing using vcpkg](#installing-using-vcpkg)
    * [Installing using Conan](#installing-using-conan)
    * [Installing using Meson](#installing-using-meson)
    * [Installing using CMake with CPM](#installing-using-cmake-with-cpm)
* [Complete library reference](#complete-library-reference)
    * [Main thread pool header file (`BS_thread_pool.hpp`)](#main-thread-pool-header-file-bs_thread_poolhpp)
        * [The `BS::thread_pool` class](#the-bsthread_pool-class)
        * [Optional features for the `BS::thread_pool` class](#optional-features-for-the-bsthread_pool-class)
        * [The `BS::this_thread` namespace](#the-bsthis_thread-namespace)
        * [The `BS::multi_future<T>` class](#the-bsmulti_futuret-class)
    * [Utility header file (`BS_thread_pool_utils.hpp`)](#utility-header-file-bs_thread_pool_utilshpp)
        * [The `BS::signaller` class](#the-bssignaller-class)
        * [The `BS::synced_stream` class](#the-bssynced_stream-class)
        * [The `BS::timer` class](#the-bstimer-class)
* [About the project](#about-the-project)
    * [Issue and pull request policy](#issue-and-pull-request-policy)
    * [Acknowledgements](#acknowledgements)
    * [Starring the repository](#starring-the-repository)
    * [Copyright and citing](#copyright-and-citing)
    * [Learning more about C++](#learning-more-about-c)

## Introduction

### Motivation

Multithreading is essential for modern high-performance computing. Since C\+\+11, the C++ standard library has included built-in low-level multithreading support using constructs such as `std::thread`. However, `std::thread` creates a new thread each time it is called, which can have a significant performance overhead. Furthermore, it is possible to create more threads than the hardware can handle simultaneously, potentially resulting in a substantial slowdown.

The library presented here contains a C++ thread pool class, `BS::thread_pool`, which avoids these issues by creating a fixed pool of threads once and for all, and then continuously reusing the same threads to perform different tasks throughout the lifetime of the program. By default, the number of threads in the pool is equal to the maximum number of threads that the hardware can run in parallel.

The user submits tasks to be executed into a queue. Whenever a thread becomes available, it retrieves the next task from the queue and executes it. The pool automatically produces an `std::future` for each task, which allows the user to wait for the task to finish executing and/or obtain its eventual return value, if applicable. Threads and tasks are autonomously managed by the pool in the background, without requiring any input from the user aside from submitting the desired tasks.

The design of this library is guided by four important principles. First, *compactness*: the entire library consists of just one self-contained header file, with no other components or dependencies, aside from a small self-contained header file with optional utilities. Second, *portability*: the library only utilizes the C\+\+17 standard library, without relying on any compiler extensions or 3rd-party libraries, and is therefore compatible with any modern standards-conforming C\+\+17 compiler on any platform. Third, *ease of use*: the library is extensively documented, and programmers of any level should be able to use it right out of the box.

The fourth and final guiding principle is *performance*: each and every line of code in this library was carefully designed with maximum performance in mind, and performance was tested and verified on a variety of compilers and platforms. Indeed, the library was originally designed for use in the author's own computationally-intensive scientific computing projects, running both on high-end desktop/laptop computers and high-performance computing nodes.

Other, more advanced multithreading libraries may offer more features and/or higher performance. However, they typically consist of a vast codebase with multiple components and dependencies, and involve complex APIs that require a substantial time investment to learn. This library is not intended to replace these more advanced libraries; instead, it was designed for users who don't require very advanced features, and prefer a simple and lightweight library that is easy to learn and use and can be readily incorporated into existing or new projects.

### Overview of features

* **Fast:**
    * Built from scratch with maximum performance in mind.
    * Suitable for use in high-performance computing nodes with a very large number of CPU cores.
    * Compact code, to reduce both compilation time and binary size.
    * Reusing threads avoids the overhead of creating and destroying them for individual tasks.
    * A task queue ensures that there are never more threads running in parallel than allowed by the hardware.
* **Lightweight:**
    * Single header file: simply `#include "BS_thread_pool.hpp"` and you're all set!
    * Header-only: no need to install or build the library.
    * Self-contained: no external requirements or dependencies.
    * Portable: uses only the C++ standard library, and works with any C++17-compliant compiler.
    * Only 304 lines of code, excluding comments, blank lines, and lines containing only a single brace, with all optional features disabled.
    * Only 396 lines of code across both header files with all optional features enabled and including all optional utilities.
* **Easy to use:**
    * Very simple operation, using only a handful of member functions, with additional member functions for more advanced use.
    * Every task submitted to the queue using the `submit_task()` member function automatically generates an `std::future`, which can be used to wait for the task to finish executing and/or obtain its eventual return value.
    * Loops can be automatically parallelized into any number of tasks using the `submit_loop()` member function, which returns a `BS::multi_future` that can be used to track the execution of all parallel tasks at once.
    * If futures are not needed, tasks may be submitted using `detach_task()`, and loops can be parallelized using `detach_loop()` - sacrificing convenience for even greater performance. In that case, `wait()`, `wait_for()`, and `wait_until()` can be used to wait for all the tasks in the queue to complete.
    * The code is thoroughly documented using Doxygen comments - not only the interface, but also the implementation, in case the user would like to make modifications.
    * The included test program `BS_thread_pool_test.cpp` can be used to perform exhaustive automated tests and benchmarks, and also serves as a comprehensive example of how to properly use the library. The included PowerShell script `BS_thread_pool_test.ps1` provides a portable way to run the tests with multiple compilers.
* **Utility classes:**
    * The optional header file `BS_thread_pool_utils.hpp` contains several useful utility classes.
    * Send simple signals between threads using the `BS::signaller` utility class.
    * Synchronize output to a stream from multiple threads in parallel using the `BS::synced_stream` utility class.
    * Easily measure execution time for benchmarking purposes using the `BS::timer` utility class.
* **Additional features:**
    * Assign a priority to each task using the optional task priority feature. Tasks with higher priorities will be executed first.
    * Submit a sequence of tasks enumerated by indices to the queue using `detach_sequence()` and `submit_sequence()`.
    * Change the number of threads in the pool safely and on-the-fly as needed using the `reset()` member function.
    * Monitor the number of queued and/or running tasks using the `get_tasks_queued()`, `get_tasks_running()`, and `get_tasks_total()` member functions.
    * Get the current thread count of the pool using `get_thread_count()`.
    * Freely pause and resume the pool using the `pause()`, `unpause()`, and `is_paused()` member functions; when paused, threads do not retrieve new tasks out of the queue.
    * Purge all tasks currently waiting in the queue with the `purge()` member function.
    * Catch exceptions thrown by tasks submitted using `submit_task()` or `submit_loop()` from the main thread through their futures.
    * Run an initialization function in each thread before it starts to execute any submitted tasks.
    * Get the pool index of the current thread using `BS::this_thread::get_index()` and a pointer to the pool that owns the thread using `BS::this_thread::get_pool()`.
    * Get the unique thread IDs for all threads in the pool using `get_thread_ids()` or the implementation-defined thread handles using the optional `get_native_handles()` member function.
    * Submit class member functions to the pool, either applied to a specific object or from within the object itself.
    * Pass arguments to tasks by value, reference, or constant reference.
    * Under continuous and active development. Bug reports and feature requests are welcome, and should be made via [GitHub issues](https://github.com/bshoshany/thread-pool/issues).

### Compiling and compatibility

This library should successfully compile on any C++17 standard-compliant compiler, on all operating systems and architectures for which such a compiler is available. Compatibility was verified with a 24-core (8P+16E) / 32-thread Intel i9-13900K CPU using the following compilers and platforms:

* Windows 11 build 22631.2861:
    * [Clang](https://clang.llvm.org/) v17.0.6
    * [GCC](https://gcc.gnu.org/) v13.2.0 ([MSYS2 build](https://www.msys2.org/))
    * [MSVC](https://docs.microsoft.com/en-us/cpp/) v19.38.33133
* Ubuntu 23.10:
    * [Clang](https://clang.llvm.org/) v17.0.6
    * [GCC](https://gcc.gnu.org/) v13.2.0

In addition, this library was tested on a [Digital Research Alliance of Canada](https://alliancecan.ca/en) node equipped with two 20-core / 40-thread Intel Xeon Gold 6148 CPUs (for a total of 40 cores and 80 threads), running CentOS Linux 7.9.2009, using [GCC](https://gcc.gnu.org/) v13.2.0.

The test program `BS_thread_pool_test.cpp` was compiled without warnings (with the warning flags `-Wall -Wextra -Wconversion -Wsign-conversion -Wpedantic -Weffc++ -Wshadow` in GCC/Clang and `/W4` in MSVC), executed, and successfully completed all [automated tests](#testing-the-library) and benchmarks using all of the compilers and systems mentioned above.

As this library requires C\+\+17 features, the code must be compiled with C\+\+17 support:

* For Clang or GCC, use the `-std=c++17` flag. On Linux, you will also need to use the `-pthread` flag to enable the POSIX threads library.
* For MSVC, use `/std:c++17`, and also `/permissive-` to ensure standards conformance.

For maximum performance, it is recommended to compile with all available compiler optimizations:

* For Clang or GCC, use the `-O3` flag.
* For MSVC, use `/O2`.

As an example, to compile the test program `BS_thread_pool_test.cpp` with warnings and optimizations, it is recommended to use the following commands:

* On Linux with GCC: `g++ BS_thread_pool_test.cpp -std=c++17 -O3 -Wall -Wextra -Wconversion -Wsign-conversion -Wpedantic -Weffc++ -Wshadow -pthread -o BS_thread_pool_test`
* On Linux with Clang: replace `g++` with `clang++`.
* On Windows with GCC or Clang: replace `-o BS_thread_pool_test` with `-o BS_thread_pool_test.exe` and remove `-pthread`.
* On Windows with MSVC: `cl BS_thread_pool_test.cpp /std:c++17 /permissive- /O2 /W4 /EHsc /Fo:BS_thread_pool_test.obj /Fe:BS_thread_pool_test.exe`

## Getting started

### Installing the library

To install `BS::thread_pool`, simply download the [latest release](https://github.com/bshoshany/thread-pool/releases) from the GitHub repository, place the header file `BS_thread_pool.hpp` from the `include` folder in the desired folder, and include it in your program:

```cpp
#include "BS_thread_pool.hpp"
```

The thread pool will now be accessible via the `BS::thread_pool` class. For an even quicker installation, you can download the header file itself directly [at this URL](https://raw.githubusercontent.com/bshoshany/thread-pool/master/include/BS_thread_pool.hpp).

This library also comes with an independent utilities header file `BS_thread_pool_utils.hpp`, which is not required to use the thread pool, but provides some utility classes that may be helpful for multithreading. This header file also resides in the `include` folder. It can be downloaded directly [at this URL](https://raw.githubusercontent.com/bshoshany/thread-pool/master/include/BS_thread_pool_utils.hpp).

This library is also available on various package managers and build system, including [vcpkg](https://vcpkg.io/), [Conan](https://conan.io/), [Meson](https://mesonbuild.com/), and [CMake](https://cmake.org/) with [CPM](https://github.com/cpm-cmake/CPM.cmake). Please [see below](#installing-the-library-using-package-managers) for more details.

### Constructors

The default constructor creates a thread pool with as many threads as the hardware can handle concurrently, as reported by the implementation via `std::thread::hardware_concurrency()`. This is usually determined by the number of cores in the CPU. If a core is hyperthreaded, it will count as two threads. For example:

```cpp
// Constructs a thread pool with as many threads as available in the hardware.
BS::thread_pool pool;
```

Optionally, a number of threads different from the hardware concurrency can be specified as an argument to the constructor. However, note that adding more threads than the hardware can handle will **not** improve performance, and in fact will most likely hinder it. This option exists in order to allow using **less** threads than the hardware concurrency, in cases where you wish to leave some threads available for other processes. For example:

```cpp
// Constructs a thread pool with only 12 threads.
BS::thread_pool pool(12);
```

Usually, when the thread pool is used, a program's main thread should only submit tasks to the thread pool and wait for them to finish, and should not perform any computationally intensive tasks on its own. In that case, it is recommended to use the default value for the number of threads. This ensures that all of the threads available in the hardware will be put to work while the main thread waits.

### Getting and resetting the number of threads in the pool

The member function `get_thread_count()` returns the number of threads in the pool. This will be equal to `std::thread::hardware_concurrency()` if the default constructor was used.

It is generally unnecessary to change the number of threads in the pool after it has been created, since the whole point of a thread pool is that you only create the threads once. However, if needed, this can be done, safely and on-the-fly, using the `reset()` member function.

`reset()` will wait for all currently running tasks to be completed, but will leave the rest of the tasks in the queue. Then it will destroy the thread pool and create a new one with the desired new number of threads, as specified in the function's argument (or the hardware concurrency if no argument is given). The new thread pool will then resume executing the tasks that remained in the queue and any new submitted tasks.

### Finding the version of the library

If desired, the version of this library may be read during compilation time from the following three macros:

* `BS_THREAD_POOL_VERSION_MAJOR` - indicates the major version.
* `BS_THREAD_POOL_VERSION_MINOR` - indicates the minor version.
* `BS_THREAD_POOL_VERSION_PATCH` - indicates the patch version.

 ```cpp
std::cout << "Thread pool library version is " << BS_THREAD_POOL_VERSION_MAJOR << '.' << BS_THREAD_POOL_VERSION_MINOR << '.' << BS_THREAD_POOL_VERSION_PATCH << ".\n";
```

Sample output:

```none
Thread pool library version is 4.0.1.
```

This can be used, for example, to allow the same code base to work with several incompatible versions of the library using `#if` directives.

**Note:** This feature is only available starting with v4.0.1. Earlier releases of this library do not define these macros.

## Submitting tasks to the queue

### Submitting tasks with no arguments and receiving a future

In this section we will learn how to submit a task with no arguments, but potentially with a return value, to the queue. Once a task has been submitted, it will be executed as soon as a thread becomes available. Tasks are executed in the order that they were submitted (first-in, first-out), unless task priority is enabled (see below).

For example, if the pool has 8 threads and an empty queue, and we submitted 16 tasks, then we should expect the first 8 tasks to be executed in parallel, with the remaining tasks being picked up by the threads one by one as each thread finishes executing its first task, until no tasks are left in the queue.

The member function `submit_task()` is used to submit tasks to the queue. It takes exactly one input, the task to submit. This task must be a function with no arguments, but it can have a return value. The return value is an `std::future` associated to the task.

If the submitted function has a return value of type `T`, then the future will be of type `std::future<T>`, and will be set to the return value when the function finishes its execution. If the submitted function does not have a return value, then the future will be an `std::future<void>`, which will not return any value but may still be used to wait for the function to finish.

Using `auto` for the return value of `submit_task()` means the compiler will automatically detect which instance of the template `std::future` to use. However, specifying the particular type `std::future<T>`, as in the examples below, is recommended for increased readability.

To wait until the task finishes, use the member function `wait()` of the future. To obtain the return value, use the member function `get()`, which will also automatically wait for the task to finish if it hasn't yet. Here is a simple example:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <future>             // std::future
#include <iostream>           // std::cout

int the_answer()
{
    return 42;
}

int main()
{
    BS::thread_pool pool;
    std::future<int> my_future = pool.submit_task(the_answer);
    std::cout << my_future.get() << '\n';
}
```

In this example we submitted the function `the_answer()`, which returns an `int`. The member function `submit_task()` of the pool therefore returned an `std::future<int>`. We then used used the `get()` member function of the future to get the return value, and printed it out.

In addition to submitted a pre-defined function, we can also use a [lambda expression](https://en.cppreference.com/w/cpp/language/lambda) to quickly define the task on-the-fly. Rewriting the previous example in terms of a lambda expression, we get:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <future>             // std::future
#include <iostream>           // std::cout

int main()
{
    BS::thread_pool pool;
    std::future<int> my_future = pool.submit_task([]{ return 42; });
    std::cout << my_future.get() << '\n';
}
```

Here, the lambda expression `[]{ return 42; }` has two parts:

1. An empty capture clause, denoted by `[]`. This signifies to the compiler that a lambda expression is being defined.
2. A code block `{ return 42; }` that simply returns the value `42`.

It is generally simpler and faster to submit lambda expressions rather than pre-defined functions, especially due to the ability to capture local variables, which we will discuss in the next section.

Of course, tasks do not have to return values. In the following example, we submit a function with no return value and then using the future to wait for it to finish executing:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <chrono>             // std::chrono
#include <future>             // std::future
#include <iostream>           // std::cout
#include <thread>             // std::this_thread

int main()
{
    BS::thread_pool pool;
    const std::future<void> my_future = pool.submit_task(
        []
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        });
    std::cout << "Waiting for the task to complete... ";
    my_future.wait();
    std::cout << "Done." << '\n';
}
```

Here we split the lambda into multiple lines to make it more readable. The command `std::this_thread::sleep_for(std::chrono::milliseconds(500))` instructs the task to simply sleep for 500 milliseconds, simulating a computationally-intensive task.

### Submitting tasks with arguments and receiving a future

As stated in the previous section, tasks submitted using `submit_task()` cannot have any arguments. However, it is easy to submit tasks with argument either by wrapping the function in a lambda or using lambda captures directly. Here are two examples.

The following is an example of submitting a pre-defined function with arguments by wrapping it with a lambda:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <future>             // std::future
#include <iostream>           // std::cout

double multiply(const double lhs, const double rhs)
{
    return lhs * rhs;
}

int main()
{
    BS::thread_pool pool;
    std::future<double> my_future = pool.submit_task(
        []
        {
            return multiply(6, 7);
        });
    std::cout << my_future.get() << '\n';
}
```

As you can see, to pass the arguments to `multiply` we simply called `multiply(6, 7)` explicitly inside a lambda. If the arguments are not literals, we need to use the lambda capture clause to capture the arguments from the local scope:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <future>             // std::future
#include <iostream>           // std::cout

double multiply(const double lhs, const double rhs)
{
    return lhs * rhs;
}

int main()
{
    BS::thread_pool pool;
    constexpr double first = 6;
    constexpr double second = 7;
    std::future<double> my_future = pool.submit_task(
        [first, second]
        {
            return multiply(first, second);
        });
    std::cout << my_future.get() << '\n';
}
```

We could even get rid of the `multiply` function entirely and put everything inside a lambda, if desired:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <future>             // std::future
#include <iostream>           // std::cout

int main()
{
    BS::thread_pool pool;
    constexpr double first = 6;
    constexpr double second = 7;
    std::future<double> my_future = pool.submit_task(
        [first, second]
        {
            return first * second;
        });
    std::cout << my_future.get() << '\n';
}
```

### Detaching and waiting for tasks

Usually, it is best to submit a task to the queue using `submit_task()`. This allows you to wait for the task to finish and/or get its return value later. However, sometimes a future is not needed, for example when you just want to "set and forget" a certain task, or if the task already communicates with the main thread or with other tasks without using futures, such as via condition variables.

In such cases, you may wish to avoid the overhead involved in assigning a future to the task in order to increase performance. This is called "detaching" the task, as the task detaches from the main thread and runs independently.

Detaching tasks is done using the `detach_task()` member function, which allows you to detach a task to the queue without generating a future for it. The task can have any number of arguments, but it cannot have a return value, as there would be no way for the main thread to retrieve that value.

Since `detach_task()` does not return a future, there is no built-in way for the user to know when the task finishes executing. You must manually ensure that the task finishes executing before trying to use anything that depends on its output. Otherwise, bad things will happen!

`BS::thread_pool` provides the member function `wait()` to facilitate waiting for all of the tasks in the queue to complete, whether they were detached or submitted with a future. The `wait()` member function works similarly to the `wait()` member function of `std::future`. Consider, for example, the following code:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <chrono>             // std::chrono
#include <iostream>           // std::cout
#include <thread>             // std::this_thread

int main()
{
    BS::thread_pool pool;
    int result = 0;
    pool.detach_task(
        [&result]
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            result = 42;
        });
    std::cout << result << '\n';
}
```

This program first defines a local variable named `result` and initializes it to `0`. It then detaches a task in the form of a lambda expression. Note that the lambda captures `result` **by reference**, as indicated by the `&` in front of it. This means that the task can modify `result`, and any such modification will be reflected in the main thread. The task changes `result` to `42`, but it first sleeps for 100 milliseconds. When the main thread prints out the value of `result`, the task has not yet had time to modify its value, since it is still sleeping. Therefore, the program will print out the initial value `0`.

To wait for the task to complete, we must use the `wait()` member function after detaching it:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <chrono>             // std::chrono
#include <iostream>           // std::cout
#include <thread>             // std::this_thread

int main()
{
    BS::thread_pool pool;
    int result = 0;
    pool.detach_task(
        [&result]
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            result = 42;
        });
    pool.wait();
    std::cout << result << '\n';
}
```

Now the program will print out the value `42`, as expected. Note, however, that `wait()` will wait for **all** the tasks in the queue, including any other tasks that were potentially submitted before or after the one we care about. If we want to wait for just one task, `submit_task()` would be a better choice.

### Waiting for submitted or detached tasks with a timeout

Sometimes you may wish to wait for the tasks to complete, but only for a certain amount of time, or until a specific point in time. For example, if the tasks have not yet completed after some time, you may wish to let the user know that there is a delay.

For task submitted with futures using `submit_task()`, this can be achieved using two member functions of `std::future`:

* `wait_for()` waits for the task to be completed, but stops waiting after the specified duration, given as an argument of type `std::chrono::duration`, has passed.
* `wait_until()` waits for the task to be completed, but stops waiting after the specified time point, given as an argument of type `std::chrono::time_point`, has been reached.

In both cases, the functions will return `future_status::ready` if the future is ready, meaning the task is finished and its return value, if any, has been obtained. However, it will return `std::future_status::timeout` if the future is not yet ready by the time the timeout has expired.

Here is an example:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <chrono>             // std::chrono
#include <future>             // std::future
#include <iostream>           // std::cout
#include <thread>             // std::this_thread

int main()
{
    BS::thread_pool pool;
    const std::future<void> my_future = pool.submit_task(
        []
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            std::cout << "Task done!\n";
        });
    while (true)
    {
        if (my_future.wait_for(std::chrono::milliseconds(200)) != std::future_status::ready)
            std::cout << "Sorry, the task is not done yet.\n";
        else
            break;
    }
}
```

The output should look similar to this:

```none
Sorry, the task is not done yet.
Sorry, the task is not done yet.
Sorry, the task is not done yet.
Sorry, the task is not done yet.
Task done!
```

For detached tasks, since we do not have a future for them, we cannot use this method. However, `BS::thread_pool` has two member functions, also named `wait_for()` and `wait_until()`, which similarly wait for a specified duration or until a specified time point, but do so for **all** tasks (whether submitted or detached). Instead of an `std::future_status`, the thread pool's wait functions returns `true` if all tasks finished running, or `false` if the duration expired or the time point was reached but some tasks are still running.

Here is the same example as above, using `detach_task()` and `pool.wait_for()`:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <chrono>             // std::chrono
#include <iostream>           // std::cout
#include <thread>             // std::this_thread

int main()
{
    BS::thread_pool pool;
    pool.detach_task(
        []
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            std::cout << "Task done!\n";
        });
    while (true)
    {
        if (!pool.wait_for(std::chrono::milliseconds(200)))
            std::cout << "Sorry, the task is not done yet.\n";
        else
            break;
    }
}
```

### Class member functions as tasks

Let us consider the following program:

```cpp
#include <iostream> // std::cout, std::boolalpha

class flag_class
{
public:
    [[nodiscard]] bool get_flag() const
    {
        return flag;
    }

    void set_flag(const bool arg)
    {
        flag = arg;
    }

private:
    bool flag = false;
};

int main()
{
    flag_class flag_object;
    flag_object.set_flag(true);
    std::cout << std::boolalpha << flag_object.get_flag() << '\n';
}
```

This program creates a new object `flag_object` of the class `flag_class`, sets the flag to `true` using the setter member function `set_flag()`, and then prints out the flag's value using the getter member function `get_flag()`.

What if we want to submit the member function `set_flag()` as a task to the thread pool? We simply wrap the entire statement `flag_object.set_flag(true);` from line in a lambda, and pass `flag_object` to the lambda by reference, as in this example:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <iostream>           // std::cout, std::boolalpha

class flag_class
{
public:
    [[nodiscard]] bool get_flag() const
    {
        return flag;
    }

    void set_flag(const bool arg)
    {
        flag = arg;
    }

private:
    bool flag = false;
};

int main()
{
    BS::thread_pool pool;
    flag_class flag_object;
    pool.submit_task(
            [&flag_object]
            {
                flag_object.set_flag(true);
            })
        .wait();
    std::cout << std::boolalpha << flag_object.get_flag() << '\n';
}
```

Of course, this will also work with `detach_task()`, if we call `wait()` on the pool itself instead of on the returned future.

Note that in this example, instead of getting a future from `submit_task()` and then waiting for that future, we simply called `wait()` on that future straight away. This is a common way of waiting for a task to complete if we have nothing else to do in the meantime. Note also that we passed `flag_object` by reference to the lambda, since we want to set the flag on that same object, not a copy of it (passing by value wouldn't have worked anyway, since variables captured by value are implicitly `const`).

Another thing you might want to do is call a member function from within the object itself, that is, from another member function. This follows a similar syntax, except that you must also capture `this` (i.e. a pointer to the current object) in the lambda. Here is an example:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <iostream>           // std::cout, std::boolalpha

BS::thread_pool pool;

class flag_class
{
public:
    [[nodiscard]] bool get_flag() const
    {
        return flag;
    }

    void set_flag(const bool arg)
    {
        flag = arg;
    }

    void set_flag_to_true()
    {
        pool.submit_task(
                [this]
                {
                    set_flag(true);
                })
            .wait();
    }

private:
    bool flag = false;
};

int main()
{
    flag_class flag_object;
    flag_object.set_flag_to_true();
    std::cout << std::boolalpha << flag_object.get_flag() << '\n';
}
```

Note that in this example we defined the thread pool as a global object, so that it is accessible outside the `main()` function.

## Parallelizing loops

### Automatic parallelization of loops

One of the most common and effective methods of parallelization is splitting a loop into smaller loops and running them in parallel. It is most effective in "embarrassingly parallel" computations, such as vector or matrix operations, where each iteration of the loop is completely independent of every other iteration.

For example, if we are summing up two vectors of 1000 elements each, and we have 10 threads, we could split the summation into 10 blocks of 100 elements each, and run all the blocks in parallel, potentially increasing performance by up to a factor of 10.

`BS::thread_pool` can automatically parallelize loops. To see how this works, consider the following generic loop:

```cpp
for (T i = start; i < end; ++i)
    loop(i);
```

where:

* `T` is any signed or unsigned integer type.
* The loop is over the range `[start, end)`, i.e. inclusive of `start` but exclusive of `end`.
* `loop()` is an operation performed for each loop index `i`, such as modifying an array with `end - start` elements.

This loop may be automatically parallelized and submitted to the thread pool's queue using the member function `submit_loop()`, which has the follows syntax:

```cpp
pool.submit_loop(start, end, loop, num_blocks);
```

where:

* `start` is the first index in the range.
* `end` is the index after the last index in the range, such that the full range is `[start, end)`. In other words, the loop will be equivalent to the one above if `start` and `end` are the same.
    * `start` and `end` must both be of the same integer type `T`. See below for examples of what to do when they are not of the same type.
    * Note that if `end <= start`, nothing will happen.
* `loop()` is the function that should run in every iteration of the loop, and takes one argument, the loop index.
* `num_blocks` is the number of blocks of the form `[a, b)` to split the loop into. For example, if the range is `[0, 9)` and there are 3 blocks, then the blocks will be the ranges `[0, 3)`, `[3, 6)`, and `[6, 9)`.
    * The internal algorithm ensures that each of the blocks has one of two sizes, differing by 1, with the larger blocks always first, so that the tasks are as evenly distributed as possible. For example, if the range `[0, 100)` is split into 15 blocks, the result will be 10 blocks of size 7, which will be executed first, and 5 blocks of size 6.
    * This argument can be omitted, in which case the number of blocks will be the number of threads in the pool.

Each block will be submitted to the thread pool's queue as a separate task. Therefore, a loop that is split into 3 blocks will be split into 3 individual tasks, which may run in parallel. If there is only one block, then the entire loop will run as one task, and no parallelization will take place.

To parallelize the generic loop above, we use the following commands:

```cpp
BS::multi_future<void> loop_future = pool.submit_loop(start, end, loop, num_blocks);
loop_future.wait();
```

`submit_loop()` returns an object of the helper class template [`BS::multi_future`](#more-about-bsmulti_futuret). This is essentially a specialization of `std::vector<std::future<T>>` with additional member functions. Each of the `num_blocks` blocks will have an `std::future` assigned to it, and all these futures will be stored inside the returned `BS::multi_future`. When `loop_future.wait()` is called, the main thread will wait until all tasks generated by `submit_loop()` finish executing, and only those tasks - not any other tasks that also happen to be in the queue. This is essentially the role of the `BS::multi_future` class: to wait for a specific **group of tasks**, in this case the tasks running the loop blocks.

What value should you use for `num_blocks`? Omitting this argument, so that the number of blocks will be equal to the number of threads in the pool, is typically a good choice. For best performance, it is recommended to do your own benchmarks to find the optimal number of blocks for each loop (you can use the [`BS::timer`](#measuring-execution-time-with-bstimer) utility class). Using fewer tasks than there are threads may be preferred if you are also running other tasks in parallel. Using more tasks than there are threads may improve performance in some cases, but parallelization with too many tasks will suffer from diminishing returns.

As a simple example, the following code calculates and prints a table of squares of all integers from 0 to 99:

```cpp
#include <iomanip>  // std::setw
#include <iostream> // std::cout

int main()
{
    constexpr unsigned int max = 100;
    unsigned int squares[max];
    for (unsigned int i = 0; i < max; ++i)
        squares[i] = i * i;
    for (unsigned int i = 0; i < max; ++i)
        std::cout << std::setw(2) << i << "^2 = " << std::setw(4) << squares[i] << ((i % 5 != 4) ? " | " : "\n");
}
```

We can parallelize it as follows:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <iomanip>            // std::setw
#include <iostream>           // std::cout

int main()
{
    BS::thread_pool pool(10);
    constexpr unsigned int max = 100;
    unsigned int squares[max];
    const BS::multi_future<void> loop_future = pool.submit_loop<unsigned int>(0, max,
        [&squares](const unsigned int i)
        {
            squares[i] = i * i;
        });
    loop_future.wait();
    for (unsigned int i = 0; i < max; ++i)
        std::cout << std::setw(2) << i << "^2 = " << std::setw(4) << squares[i] << ((i % 5 != 4) ? " | " : "\n");
}
```

Since there are 10 threads, and we omitted the `num_blocks` argument, the loop will be divided into 10 blocks, each calculating 10 squares.

Note that `submit_loop()` was executed with the explicit template parameter `<unsigned int>`. The reason is that the two loop indices must be of the same type. However, here `max` is a `unsigned int`, while `0` is a (signed) `int`, so the types do not match, and the code will not compile unless we force the `0` to be of the right type. This can be done most elegantly by specifying the type of the indices explicitly using the template parameter.

The reason this is not done automatically (e.g. using [`std::common_type`](https://en.cppreference.com/w/cpp/types/common_type) is that it may result in accidentally casting negative indices to an unsigned type, or integer indices to a too narrow integer type, which may lead to an incorrect loop range.

We could also cast the `0` explicitly to unsigned int, but that doesn't look as nice:

```cpp
pool.submit_loop(static_cast<unsigned int>(0), max, /* ... */);
```

Or we could use a C-style cast:

```cpp
pool.submit_loop((unsigned int)(0), max, /* ... */);
```

Or we could use an integer literal suffix:

```cpp
pool.submit_loop<size_t>(0U, max, ...);
```

As a side note, notice that here we parallelized the calculation of the squares, but we did not parallelize printing the results. This is for two reasons:

1. We want to print out the squares in ascending order, and we have no guarantee that the blocks will be executed in the correct order. This is very important; you must never expect that the parallelized loop will execute at the same order as the non-parallelized loop.
2. If we did print out the squares from within the parallel tasks, we would get a huge mess, since all 10 blocks would print to the standard output at once. [Later](#synchronizing-printing-to-a-stream-with-bssynced_stream) we will see how to synchronize printing to a stream from multiple tasks at the same time.

### Parallelizing loops without futures

Just as in the case of [`detach_task()`](#detaching-and-waiting-for-tasks) vs. [`submit_task()`](#submitting-tasks-with-no-arguments-and-receiving-a-future), sometimes you may want to parallelize a loop, but you don't need it to return a `BS::multi_future`. In this case, you can save the overhead of generating the futures (which can be significant, depending on the number of blocks) by using `detach_loop()` instead of `submit_loop()`, with the same arguments.

For example, we could detach the loop of squares example above as follows:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <iomanip>            // std::setw
#include <iostream>           // std::cout

int main()
{
    BS::thread_pool pool(10);
    constexpr unsigned int max = 100;
    unsigned int squares[max];
    pool.detach_loop<unsigned int>(0, max,
        [&squares](const unsigned int i)
        {
            squares[i] = i * i;
        });
    pool.wait();
    for (unsigned int i = 0; i < max; ++i)
        std::cout << std::setw(2) << i << "^2 = " << std::setw(4) << squares[i] << ((i % 5 != 4) ? " | " : "\n");
}
```

**Warning:** Since `detach_loop()` does not return a `BS::multi_future`, there is no built-in way for the user to know when the loop finishes executing. You must use either [`wait()`](#detaching-and-waiting-for-tasks) as we did here, or some other method such as condition variables, to ensure that the loop finishes executing before trying to use anything that depends on its output. Otherwise, bad things will happen!

### Parallelizing individual indices vs. blocks

We have seen that `detach_loop()` and `submit_loop()` execute the function `loop(i)` for each index `i` in the loop. However, behind the scenes, the loop is split into blocks, and each block executes the `loop()` function multiple times. Each block has an internal loop of the form (where `T` is the type of the indices):

```cpp
for (T i = start; i < end; ++i)
    loop(i);
```

The `start` and `end` indices of each block are determined automatically by the pool. For example, in the previous section, the loop from 0 to 100 was split into 10 blocks of 10 indices each: `start = 0` to `end = 10`, `start = 10` to `end = 20`, and so on; the blocks are not inclusive of the last index, since the `for` loop has the condition `i < end` and not `i <= end`.

However, this also means that the `loop()` function is executed multiple times per block. This generates additional overhead due to the multiple function calls. For short loops, this should not affect performance. However, for very long loops, with millions of indices, the performance cost may be significate.

For this reason, the thread pool library provides two additional member functions for parallelizing loops: `detach_blocks()` and `submit_blocks()`. While `detach_loop()` and `submit_loop()` execute a function `loop(i)` once per index but multiple times per block, `detach_blocks()` and `submit_blocks()` execute a function `block(start, end)` once per block.

The main advantage of this method is increased performance, but the main disadvantage is slightly more complicated code. In particular, the user must define the loop from `start` to `end` manually within each block. Here is the previous example using `detach_blocks()`:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <iomanip>            // std::setw
#include <iostream>           // std::cout

int main()
{
    BS::thread_pool pool(10);
    constexpr unsigned int max = 100;
    unsigned int squares[max];
    pool.detach_blocks<unsigned int>(0, max,
        [&squares](const unsigned int start, const unsigned int end)
        {
            for (unsigned int i = start; i < end; ++i)
                squares[i] = i * i;
        });
    pool.wait();
    for (unsigned int i = 0; i < max; ++i)
        std::cout << std::setw(2) << i << "^2 = " << std::setw(4) << squares[i] << ((i % 5 != 4) ? " | " : "\n");
}
```

Note how the block function takes two arguments, and includes the internal loop.

Generally, compiler optimizations should be able to make `detach_loop()` and `submit_loop()` perform roughly the same as `detach_blocks()` and `submit_blocks()`. However, you should perform your own benchmarks to see which option works best for your particular use case.

### Loops with return values

Unlike `submit_task()`, the member function `submit_loop()` only takes loop functions with no return values. The reason is that it wouldn't make sense to return a future for every single index of the loop. However, `submit_blocks()` does allow the block function to have a return value, as the number of blocks will generally not be too large, unlike the number of indices.

The block function will be executed once for each block, but the blocks are managed by the thread pool, with the user only able to select the number of blocks, but not the range of each block. Therefore, there is limited usability in returning one value per block. However, for cases where this is desired, such as for summation or some sorting algorithms, `submit_blocks()` does accept functions with return values, in which case it returns a `BS::multi_future<T>` object where `T` is the type of the return values.

Here's an example of a function template summing all elements of type `T` in a given range:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <cstdint>            // std::uint64_t
#include <future>             // std::future
#include <iostream>           // std::cout

BS::thread_pool pool;

template <typename T>
T sum(T min, T max)
{
    BS::multi_future<T> loop_future = pool.submit_blocks<T>(
        min, max + 1,
        [](const T start, const T end)
        {
            T block_total = 0;
            for (T i = start; i < end; ++i)
                block_total += i;
            return block_total;
        },
        100);
    T result = 0;
    for (std::future<T>& future : loop_future)
        result += future.get();
    return result;
}

int main()
{
    std::cout << sum<std::uint64_t>(1, 1'000'000);
}
```

Here we used the fact that `BS::multi_future<T>` is a specialization of `std::vector<std::future<T>>`, so we can use a range-based `for` loop to iterate over the futures, and use the `get()` member function of each future to get its value. The values of the futures will be the partial sums from each block, so when we add them up, we will get the total sum. Note that we divided the loop into 100 blocks, so there will be 100 futures in total, each with the partial sum of 10,000 numbers.

The range-based `for` loop will likely start before the loop finished executing, and each time it calls a future, it will get the value of that future if it is ready, or it will wait until the future is ready and then get the value. This increases performance, since we can start summing the results without waiting for the entire loop to finish executing first - we only need to wait for individual blocks.

If we did want to wait until the entire loop finishes before summing the results, we could have used the `get()` member function of the `BS::multi_future<T>` object itself, which returns an `std::vector<T>` with the values obtained from each future. In that case, the sum could be obtained after calling `submit_blocks()` as follows:

```cpp
std::vector<T> partial_sums = loop_future.get();
T result = std::reduce(partial_sums.begin(), partial_sums.end());
return result;
```

### Parallelizing sequences

The member functions `detach_loop()`, `submit_loop()`, `detach_blocks()`, and `submit_blocks()` parallelize a loop by splitting it into blocks, and submitting each block as an individual task to the queue, with each such task iterating over all the indices in the corresponding block's range, which can be numerous. However, sometimes we have loops with few indices, or more generally, a sequence of tasks enumerated by some index. In such cases, we can avoid the overhead of splitting into blocks and simply submit each individual index as its own independent task to the pool's queue.

This can be done with `detach_sequence()` and `submit_sequence()`. The syntax of these functions is similar to `detach_loop()` and `submit_loop()`, except that they don't have the `num_blocks` argument at the end. The sequence function must take only one argument, the index. As usual, `detach_sequence()` detaches the tasks and does not return a future, while `submit_sequence()` returns a `BS::multi_future`. If the tasks in the sequence return values, then the futures will contain those values, otherwise they will be `void` futures.

Here is a simple example:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <cstdint>            // std::uint64_t
#include <iostream>           // std::cout
#include <vector>             // std::vector

using ui64 = std::uint64_t;

ui64 factorial(const ui64 n)
{
    ui64 result = 1;
    for (ui64 i = 2; i <= n; ++i)
        result *= i;
    return result;
}

int main()
{
    BS::thread_pool pool;
    constexpr ui64 max = 20;
    BS::multi_future<ui64> sequence_future = pool.submit_sequence<ui64>(0, max + 1, factorial);
    std::vector<ui64> factorials = sequence_future.get();
    for (ui64 i = 0; i < max + 1; ++i)
        std::cout << i << "! = " << factorials[i] << '\n';
}
```

### More about `BS::multi_future<T>`

The helper class template `BS::multi_future<T>`, which we have been using throughout this section, provides a convenient way to collect and access groups of futures. This class is a specialization of `std::vector<T>`, so it should be used in a similar way:

* When you create a new object, either use the default constructor to create an empty object and add futures to it later, or pass the desired number of futures to the constructor in advance.
* Use the `[]` operator to access the future at a specific index, or the `push_back()` member function to append a new future to the list.
* The `size()` member function tells you how many futures are currently stored in the object.

However, `BS::multi_future<T>` also has additional member functions that are aimed specifically at handling futures:

* Once all the futures are stored, you can use `wait()` to wait for all of them at once or `get()` to get an `std::vector<T>` with the results from all of them.
* You can check how many futures are ready using `ready_count()`.
* You can check if all the stored futures are valid using `valid()`.
* You can wait for all the stored futures for a specific duration with `wait_for()` or wait until a specific time with `wait_until()`. These functions return `true` if all futures have been waited for before the duration expired or the time point was reached, and `false` otherwise.

Aside from using `BS::multi_future<T>` to track the execution of parallelized loops, it can also be used, for example, whenever you have several different groups of tasks and you want to track the execution of each group individually.

## Utility classes

The optional header file `BS_thread_pool_utils.hpp` contains several useful utility classes. These are not necessary for using the thread pool itself; `BS_thread_pool.hpp` is the only header file required. However, the utility classes can make writing multithreading code more convenient.

As with [the main header file](#finding-the-version-of-the-library), the version of the utilities header file can be found by checking three macros:

* `BS_THREAD_POOL_UTILS_VERSION_MAJOR` - indicates the major version.
* `BS_THREAD_POOL_UTILS_VERSION_MINOR` - indicates the minor version.
* `BS_THREAD_POOL_UTILS_VERSION_PATCH` - indicates the patch version.

### Synchronizing printing to a stream with `BS::synced_stream`

When printing to an output stream from multiple threads in parallel, the output may become garbled. For example, consider this code:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <iostream>           // std::cout

BS::thread_pool pool;

int main()
{
    pool.detach_sequence(0, 5,
        [](int i)
        {
            std::cout << "Task no. " << i << " executing.\n";
        });
}
```

The output will be a mess similar to this:

```none
Task no. Task no. Task no. 3 executing.
0 executing.
Task no. 41 executing.
Task no. 2 executing.
 executing.
```

The reason is that, although each **individual** insertion to `std::cout` is thread-safe, there is no mechanism in place to ensure subsequent insertions from the same thread are printed contiguously.

The utility class `BS::synced_stream` is designed to eliminate such synchronization issues. The constructor takes one optional argument, specifying the output stream to print to. If no argument is supplied, `std::cout` will be used:

```cpp
// Construct a synced stream that will print to std::cout.
BS::synced_stream sync_out;
// Construct a synced stream that will print to the output stream my_stream.
BS::synced_stream sync_out(my_stream);
```

The member function `print()` takes an arbitrary number of arguments, which are inserted into the stream one by one, in the order they were given. `println()` does the same, but also prints a newline character `\n` at the end, for convenience. A mutex is used to synchronize this process, so that any other calls to `print()` or `println()` using the same `BS::synced_stream` object must wait until the previous call has finished.

As an example, this code:

```cpp
#include "BS_thread_pool.hpp"       // BS::thread_pool
#include "BS_thread_pool_utils.hpp" // BS::synced_stream

BS::synced_stream sync_out;
BS::thread_pool pool;

int main()
{
    pool.detach_sequence(0, 5,
        [](int i)
        {
            sync_out.println("Task no. ", i, " executing.");
        });
}
```

Will print out:

```none
Task no. 0 executing.
Task no. 1 executing.
Task no. 2 executing.
Task no. 3 executing.
Task no. 4 executing.
```

**Warning:** Always create the `BS::synced_stream` object **before** the `BS::thread_pool` object, as we did in this example. When the `BS::thread_pool` object goes out of scope, it waits for the remaining tasks to be executed. If the `BS::synced_stream` object goes out of scope before the `BS::thread_pool` object, then any tasks using the `BS::synced_stream` will crash. Since objects are destructed in the opposite order of construction, creating the `BS::synced_stream` object before the `BS::thread_pool` object ensures that the `BS::synced_stream` is always available to the tasks, even while the pool is destructing.

Most stream manipulators defined in the headers `<ios>` and `<iomanip>`, such as `std::setw` (set the character width of the next output), `std::setprecision` (set the precision of floating point numbers), and `std::fixed` (display floating point numbers with a fixed number of digits), can be passed to `print()` and `println()` just as you would pass them to a stream.

The only exceptions are the flushing manipulators `std::endl` and `std::flush`, which will not work because the compiler will not be able to figure out which template specializations to use. Instead, use `BS::synced_stream::endl` and `BS::synced_stream::flush`. Here is an example:

```cpp
#include "BS_thread_pool.hpp"       // BS::thread_pool
#include "BS_thread_pool_utils.hpp" // BS::synced_stream
#include <cmath>                    // std::sqrt
#include <iomanip>                  // std::setprecision, std::setw
#include <ios>                      // std::fixed

BS::synced_stream sync_out;
BS::thread_pool pool;

int main()
{
    sync_out.print(std::setprecision(10), std::fixed);
    pool.detach_sequence(0, 16,
        [](int i)
        {
            sync_out.print("The square root of ", std::setw(2), i, " is ", std::sqrt(i), ".", BS::synced_stream::endl);
        });
}
```

Note, however, that `BS::synced_stream::endl` should only be used if flushing is desired; otherwise, a newline character should be used instead.

### Measuring execution time with `BS::timer`

If you are using a thread pool, then your code is most likely performance-critical. Achieving maximum performance requires performing a considerable amount of benchmarking to determine the optimal settings and algorithms. Therefore, it is important to be able to measure the execution time of various computations and operations under different conditions.

The helper class `BS::timer` provides a simple way to measure execution time. It is very straightforward to use:

1. Create a new `BS::timer` object.
2. Immediately before you execute the computation that you want to time, call the `start()` member function.
3. Immediately after the computation ends, call the `stop()` member function.
4. Use the member function `ms()` to obtain the elapsed time for the computation in milliseconds.
5. Alternatively, use the member function `current_ms()` to obtain the elapsed time so far but keep the timer ticking.

For example:

```cpp
BS::timer tmr;
tmr.start();
do_something();
tmr.stop();
std::cout << "The elapsed time was " << tmr.ms() << " ms.\n";
```

A practical application of the `BS::timer` class can be found in the benchmark portion of the test program `BS_thread_pool_test.cpp`.

### Sending simple signals between threads with `BS::signaller`

`BS::signaller` is a utility class which can be used to allow simple signalling between threads.  To use it, construct an object and then pass it to the different threads. Multiple threads can call the `wait()` member function of the signaller. When another thread calls the `ready()` member function, the waiting threads will stop waiting.

That's really all there is to it; `BS::signaller` is really just a convenient wrapper around [`std::promise`](https://en.cppreference.com/w/cpp/thread/promise), which contains both the promise and its future. For usage examples, please see the test program `BS_thread_pool_test.cpp`.

## Managing tasks

### Monitoring the tasks

Sometimes you may wish to monitor what is happening with the tasks you submitted to the pool. This may be done using three member functions:

* `get_tasks_queued()` gets the number of tasks currently waiting in the queue to be executed by the threads.
* `get_tasks_running()` gets the number of tasks currently being executed by the threads.
* `get_tasks_total()` gets the total number of unfinished tasks: either still in the queue, or running in a thread.
* Note that `get_tasks_total() == get_tasks_queued() + get_tasks_running()`.

These functions are demonstrated in the following program:

```cpp
#include "BS_thread_pool.hpp"       // BS::thread_pool
#include "BS_thread_pool_utils.hpp" // BS::synced_stream
#include <chrono>                   // std::chrono
#include <thread>                   // std::this_thread

BS::synced_stream sync_out;
BS::thread_pool pool(4);

void sleep_half_second(const int i)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    sync_out.println("Task ", i, " done.");
}

void monitor_tasks()
{
    sync_out.println(pool.get_tasks_total(), " tasks total, ", pool.get_tasks_running(), " tasks running, ", pool.get_tasks_queued(), " tasks queued.");
}

int main()
{
    pool.wait();
    pool.detach_sequence(0, 12, sleep_half_second);
    monitor_tasks();
    std::this_thread::sleep_for(std::chrono::milliseconds(750));
    monitor_tasks();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    monitor_tasks();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    monitor_tasks();
}
```

Assuming you have at least 4 hardware threads (so that 4 tasks can run concurrently), the output should be similar to:

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

The reason we called `pool.wait()` in the beginning is that when the thread pool is created, an initialization task runs in each thread, so if we don't wait, the first line will say there are 16 tasks in total, including the 4 initialization tasks. See [below](#thread-pool-initialization-functions) for more details.

### Purging tasks

Consider a situation where the user cancels a multithreaded operation while it is still ongoing. Perhaps the operation was split into multiple tasks, and half of the tasks are currently being executed by the pool's threads, but the other half are still waiting in the queue.

The thread pool cannot terminate the tasks that are already running, as the C++17 standard does not provide that functionality (and in any case, abruptly terminating a task while it's running could have extremely bad consequences, such as memory leaks and data corruption). However, the tasks that are still waiting in the queue can be purged using the `purge()` member function.

Once `purge()` is called, any tasks still waiting in the queue will be discarded, and will never be executed by the threads. Please note that there is no way to restore the purged tasks; they are gone forever.

Consider for example the following program:

```cpp
#include "BS_thread_pool.hpp"       // BS::thread_pool
#include "BS_thread_pool_utils.hpp" // BS::synced_stream
#include <chrono>                   // std::chrono
#include <thread>                   // std::this_thread

BS::synced_stream sync_out;
BS::thread_pool pool(4);

int main()
{
    for (size_t i = 0; i < 8; ++i)
    {
        pool.detach_task(
            [i]
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                sync_out.println("Task ", i, " done.");
            });
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    pool.purge();
    pool.wait();
}
```

The program submit 8 tasks to the queue. Each task waits 100 milliseconds and then prints a message. The thread pool has 4 threads, so it will execute the first 4 tasks in parallel, and then the remaining 4. We wait 50 milliseconds, to ensure that the first 4 tasks have all started running. Then we call `purge()` to purge the remaining 4 tasks. As a result, these tasks never get executed. However, since the first 4 tasks are still running when `purge()` is called, they will finish uninterrupted; `purge()` only discards tasks that have not yet started running. The output of the program therefore only contains the messages from the first 4 tasks:

```none
Task 0 done.
Task 1 done.
Task 2 done.
Task 3 done.
```

### Exception handling

`submit_task()` catches any exceptions thrown by the submitted task and forwards them to the corresponding future. They can then be caught when invoking the `get()` member function of the future. For example:

```cpp
#include "BS_thread_pool.hpp"

BS::synced_stream sync_out;
BS::thread_pool pool;

double inverse(const double x)
{
    if (x == 0)
        throw std::runtime_error("Division by zero!");
    else
        return 1 / x;
}

int main()
{
    constexpr double num = 0;
    std::future<double> my_future = pool.submit_task(inverse, num);
    try
    {
        const double result = my_future.get();
        sync_out.println("The inverse of ", num, " is ", result, ".");
    }
    catch (const std::exception& e)
    {
        sync_out.println("Caught exception: ", e.what());
    }
}
```

The output will be:

```none
Caught exception: Division by zero!
```

However, if you change `num` to any non-zero number, no exceptions will be thrown and the inverse will be printed.

It is important to note that `wait()` does not throw any exceptions; only `get()` does. Therefore, even if your task does not return anything, i.e. your future is an `std::future<void>`, you must still use `get()` on the future obtained from it if you want to catch exceptions thrown by it. Here is an example:

```cpp
#include "BS_thread_pool.hpp"

BS::synced_stream sync_out;
BS::thread_pool pool;

void print_inverse(const double x)
{
    if (x == 0)
        throw std::runtime_error("Division by zero!");
    else
        sync_out.println("The inverse of ", x, " is ", 1 / x, ".");
}

int main()
{
    constexpr double num = 0;
    std::future<void> my_future = pool.submit_task(print_inverse, num);
    try
    {
        my_future.get();
    }
    catch (const std::exception& e)
    {
        sync_out.println("Caught exception: ", e.what());
    }
}
```

When using `BS::multi_future` to handle multiple futures at once, exception handling works the same way: if any of the futures may throw exceptions, you may catch these exceptions when calling `get()`, even in the case of `BS::multi_future<void>`.

### Getting information about the threads

`BS::thread_pool` comes with a variety of methods to obtain information about the threads in the pool:

1. The namespace `BS::this_thread` provides functionality similar to `std::this_thread`. If the current thread belongs to a `BS::thread_pool` object, then `BS::this_thread::get_index()` can be used to get the index of the current thread, and `BS::this_thread::get_pool()` can be used to get the pointer to the thread pool that owns the current thread. Please see [the reference below](#the-bsthis_thread-namespace) for more details.
2. The member function `get_thread_ids()` returns a vector containing the unique identifiers for each of the pool's threads, as obtained by `std::thread::get_id()`. These values are not so useful on their own, but can be used for whatever the user wants to use them for.
3. The optional member function `get_native_handles()`, if enabled, returns a vector containing the underlying implementation-defined thread handles for each of the pool's threads, as obtained by `std::thread::native_handle()`. For more information, see [the relevant section below](#accessing-native-thread-handles).

### Thread pool initialization functions

Sometimes, it is necessary to initialize the threads before they run any tasks. This can be done by submitting a proper initialization function to the constructor or to `reset()`, either as the only argument or as the second argument after the desired number of threads. The thread initialization must take no arguments and have no return value. However, if needed, the function can use `BS::this_thread::get_index()` and `BS::this_thread::get_pool()` to figure out which thread and pool it belongs to.

The thread initialization function is submitted as a set of special tasks, one per thread, which bypass the queue, but still count towards the number of running tasks, which means `get_tasks_total()` and `get_tasks_running()` will report that these tasks are running if they are checked immediately after the pool is initialized.

This is done so that the user has the option to either wait for the initialization tasks to finish, by calling `wait()` on the pool, or just keep going. In either case, the initialization tasks will **always** finish executing before any tasks are picked out of the queue, so there is no reason to wait for them to finish unless they have some side-effects that affect the main thread.

Here is a simple example:

```cpp
#include "BS_thread_pool.hpp"       // BS::thread_pool
#include "BS_thread_pool_utils.hpp" // BS::synced_stream
#include <random>                   // std::mt19937_64, std::random_device

BS::synced_stream sync_out;
thread_local std::mt19937_64 twister;

int main()
{
    BS::thread_pool pool(
        []
        {
            twister.seed(std::random_device()());
        });
    pool.submit_sequence(0, 4,
            [](int)
            {
                sync_out.println("I generated a random number: ", twister());
            })
        .wait();
}
```

In this example, we create a `thread_local` Mersenne twister engine, meaning that each thread has its own independent engine. However, we did not seed the engine, so each thread will generate the exact same sequence of pseudo-random numbers. To remedy this, we pass an initialization function to the `BS::thread_pool` constructor which seeds the twister in each thread with the (hopefully) non-deterministic random number generator `std::random_device`.

### Passing task arguments by constant reference

In C++, it is often crucial to pass function arguments by reference or constant reference, instead of by value. This allows the function to access the object being passed directly, rather than creating a new copy of the object. We have already seen that submitting an argument by reference is a simple matter of capturing it with a `&` in the lambda capture list. To submit as constant reference, we can use `std::as_const` as in the following example:

```cpp
#include "BS_thread_pool.hpp"       // BS::thread_pool
#include "BS_thread_pool_utils.hpp" // BS::synced_stream
#include <utility>                  // std::as_const

BS::synced_stream sync_out;

void increment(int& x)
{
    ++x;
}

void print(const int& x)
{
    sync_out.println(x);
}

int main()
{
    BS::thread_pool pool;
    int n = 0;
    pool.submit_task(
            [&n]
            {
                increment(n);
            })
        .wait();
    pool.submit_task(
            [&n = std::as_const(n)]
            {
                print(n);
            })
        .wait();
}
```

The `increment()` function takes a **reference** to an integer, and increments that integer. Passing the argument by reference guarantees that `n` itself, in the scope of `main()`, will be incremented - rather than a copy of it in the scope of `increment()`.

Similarly, the `print()` function takes a **constant reference** to an integer, and prints that integer. Passing the argument by constant reference guarantees that the variable will not be accidentally modified by the function, even though we are accessing `n` itself, rather than a copy. If we replace `print` with `increment`, the program won't compile, as `increment` cannot take constant references.

Generally, it is not really necessary to pass arguments by constant reference, but it is more "correct" to do so, if we would like to guarantee that the variable being referenced is indeed never modified. This section is therefore included here for completeness.

## Optional features

### Pausing the pool

Sometimes you may wish to temporarily pause the execution of tasks, or perhaps you want to submit tasks to the queue in advance and only start executing them at a later time. You can do this using the member functions `pause()`, `unpause()`, and `is_paused()`.

However, these functions are disabled by default, and must be explicitly enabled by defining the macro `BS_THREAD_POOL_ENABLE_PAUSE` before including `BS_thread_pool.hpp`. The reason is that pausing the pool adds additional checks to the waiting and worker functions, which have a very small but non-zero overhead.

When you call `pause()`, the workers will temporarily stop retrieving new tasks out of the queue. However, any tasks already executed will keep running until they are done, since the thread pool has no control over the internal code of your tasks. If you need to pause a task in the middle of its execution, you must do that manually by programming your own pause mechanism into the task itself. To resume retrieving tasks, call `unpause()`. To check whether the pool is currently paused, call `is_paused()`.

Here is an example:

```cpp
#define BS_THREAD_POOL_ENABLE_PAUSE
#include "BS_thread_pool.hpp"       // BS::thread_pool
#include "BS_thread_pool_utils.hpp" // BS::synced_stream
#include <chrono>                   // std::chrono
#include <thread>                   // std::this_thread

BS::synced_stream sync_out;
BS::thread_pool pool(4);

void sleep_half_second(const int i)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    sync_out.println("Task ", i, " done.");
}

void check_if_paused()
{
    if (pool.is_paused())
        sync_out.println("Pool paused.");
    else
        sync_out.println("Pool unpaused.");
}

int main()
{
    pool.detach_sequence(0, 8, sleep_half_second);
    sync_out.println("Submitted 8 tasks.");
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    pool.pause();
    check_if_paused();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    sync_out.println("Still paused...");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    pool.detach_sequence(8, 12, sleep_half_second);
    sync_out.println("Submitted 4 more tasks.");
    sync_out.println("Still paused...");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    pool.unpause();
    check_if_paused();
}
```

Assuming you have at least 4 hardware threads, the output should be similar to:

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
Pool unpaused.
Task 4 done.
Task 5 done.
Task 6 done.
Task 7 done.
Task 8 done.
Task 9 done.
Task 10 done.
Task 11 done.
```

Here is what happened. We initially submitted a total of 8 tasks to the queue. Since we waited for 250ms before pausing, the first 4 tasks have already started running, so they kept running until they finished. While the pool was paused, we submitted 4 more tasks to the queue, but they just waited at the end of the queue. When we unpaused, the remaining 4 initial tasks were executed, followed by the 4 new tasks.

While the workers are paused, `wait()` will wait for the running tasks instead of all tasks (otherwise it would wait forever). This is demonstrated by the following program:

```cpp
#define BS_THREAD_POOL_ENABLE_PAUSE
#include "BS_thread_pool.hpp"       // BS::thread_pool
#include "BS_thread_pool_utils.hpp" // BS::synced_stream
#include <chrono>                   // std::chrono
#include <thread>                   // std::this_thread

BS::synced_stream sync_out;
BS::thread_pool pool(4);

void sleep_half_second(const int i)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    sync_out.println("Task ", i, " done.");
}

void check_if_paused()
{
    if (pool.is_paused())
        sync_out.println("Pool paused.");
    else
        sync_out.println("Pool unpaused.");
}

int main()
{
    pool.detach_sequence(0, 8, sleep_half_second);
    sync_out.println("Submitted 8 tasks. Waiting for them to complete.");
    pool.wait();
    pool.detach_sequence(8, 20, sleep_half_second);
    sync_out.println("Submitted 12 more tasks.");
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    pool.pause();
    check_if_paused();
    sync_out.println("Waiting for the ", pool.get_tasks_running(), " running tasks to complete.");
    pool.wait();
    sync_out.println("All running tasks completed. ", pool.get_tasks_queued(), " tasks still queued.");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    sync_out.println("Still paused...");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    sync_out.println("Still paused...");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    pool.unpause();
    check_if_paused();
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    sync_out.println("Waiting for the remaining ", pool.get_tasks_total(), " tasks (", pool.get_tasks_running(), " running and ", pool.get_tasks_queued(), " queued) to complete.");
    pool.wait();
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
Pool paused.
Waiting for the 4 running tasks to complete.
Task 8 done.
Task 9 done.
Task 10 done.
Task 11 done.
All running tasks completed. 8 tasks still queued.
Still paused...
Still paused...
Pool unpaused.
Waiting for the remaining 8 tasks (4 running and 4 queued) to complete.
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

The first `wait()`, which was called while the pool was not paused, waited for all 8 tasks, both running and queued. The second `wait()`, which was called after pausing the pool, only waited for the 4 running tasks, while the other 8 tasks remained queued, and were not executed since the pool was paused. Finally, the third `wait()`, which was called after unpausing the pool, waited for the remaining 8 tasks, both running and queued.

**Warning:** If the thread pool is destroyed while paused, any tasks still in the queue will never be executed!

### Avoiding wait deadlocks

Consider the following program:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <iostream>           // std::cout

int main()
{
    BS::thread_pool pool;
    pool.detach_task(
        [&pool]
        {
            pool.wait();
            std::cout << "Done waiting.\n";
        });
}
```

This program creates a thread pool, and then detaches a task that waits for tasks in the same thread pool to complete. If you run this program, it will never print the message "Done waiting", because the task will wait for **itself** to complete. This causes a **deadlock**, and the program will wait forever.

Usually, in simple programs, this will never happen. However, in more complicated programs, perhaps ones running multiple thread pools in parallel, wait deadlocks could potentially occur. In such cases, the macro `BS_THREAD_POOL_ENABLE_WAIT_DEADLOCK_CHECK` can be defined before including `BS_thread_pool.hpp`. `wait()` will then check whether the user tried to call it from within a thread of the same pool, and if so, it will throw the exception `BS::thread_pool::wait_deadlock` instead of waiting. This check is disabled by default because wait deadlocks are not something that happens often, and the check adds a small but non-zero overhead every time `wait()` is called.

Here is an example:

```cpp
#define BS_THREAD_POOL_ENABLE_WAIT_DEADLOCK_CHECK
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <iostream>           // std::cout

int main()
{
    BS::thread_pool pool;
    pool.detach_task(
        [&pool]
        {
            try
            {
                pool.wait();
                std::cout << "Done waiting.\n";
            }
            catch (const BS::thread_pool::wait_deadlock&)
            {
                std::cout << "Error: Deadlock!\n";
            }
        });
}
```

This time, `wait()` will detect the deadlock, and will throw an exception, causing the output to be `"Error: Deadlock!"`.

### Accessing native thread handles

The `BS::thread_pool` member function `get_native_handles()` returns a vector containing the underlying implementation-defined thread handles for each of the pool's threads. These can then be used in an implementation-specific way to manage the threads at the OS level

However, note that this will generally **not** be portable code. Furthermore, this feature uses [std::thread::native_handle()](https://en.cppreference.com/w/cpp/thread/thread/native_handle), which is in the C++ standard library, but is **not** guaranteed to be present on all systems. Therefore, this feature is turned off by default, and must be turned on by defining the macro `BS_THREAD_POOL_ENABLE_NATIVE_HANDLES` before including `BS_thread_pool.hpp`.

Here is an example:

```cpp
#define BS_THREAD_POOL_ENABLE_NATIVE_HANDLES
#include "BS_thread_pool.hpp"       // BS::thread_pool
#include "BS_thread_pool_utils.hpp" // BS::synced_stream
#include <thread>                   // std::thread
#include <vector>                   // std::vector

BS::synced_stream sync_out;
BS::thread_pool pool(4);

int main()
{
    std::vector<std::thread::native_handle_type> handles = pool.get_native_handles();
    for (BS::concurrency_t i = 0; i < handles.size(); ++i)
        sync_out.println("Thread ", i, " native handle: ", handles[i]);
}
```

The output will depend on your compiler and operating system. Here is an example:

```none
Thread 0 native handle: 000000F4
Thread 1 native handle: 000000F8
Thread 2 native handle: 000000EC
Thread 3 native handle: 000000FC
```

### Setting task priority

Defining the macro `BS_THREAD_POOL_ENABLE_PRIORITY` before including `BS_thread_pool.hpp` enables task priority. The priority of a task or group of tasks may then be specified as an additional argument (at the end of the argument list) to `detach_task()`, `submit_task()`, `detach_blocks()`, `submit_blocks()`, `detach_loop()`, `submit_loop()`, `detach_sequence()`, and `submit_sequence()`. If the priority is not specified, the default value will be 0.

The priority is a number of type `BS::priority_t`, which is a signed 16-bit integer, so it can have any value between -32,768 and 32,767. The tasks will be executed in priority order from highest to lowest. If priority is assigned to the block/loop/sequence parallelization functions, which submit multiple tasks, then all of these tasks will have the same priority.

The namespace `BS::pr` contains some pre-defined priorities for users who wish to avoid magic numbers and enjoy better future-proofing. In order of decreasing priority, the pre-defined priorities are: `BS::pr::highest`, `BS::pr::high`, `BS::pr::normal`, `BS::pr::low`, and `BS::pr::lowest`.

Here is a simple example:

```cpp
#define BS_THREAD_POOL_ENABLE_PRIORITY
#include "BS_thread_pool.hpp"       // BS::thread_pool
#include "BS_thread_pool_utils.hpp" // BS::synced_stream

BS::synced_stream sync_out;
BS::thread_pool pool(1);

int main()
{
    pool.detach_task([] { sync_out.println("This task will execute third."); }, BS::pr::normal);
    pool.detach_task([] { sync_out.println("This task will execute fifth."); }, BS::pr::lowest);
    pool.detach_task([] { sync_out.println("This task will execute second."); }, BS::pr::high);
    pool.detach_task([] { sync_out.println("This task will execute first."); }, BS::pr::highest);
    pool.detach_task([] { sync_out.println("This task will execute fourth."); }, BS::pr::low);
}
```

This program will print out the tasks in the correct priority order. Note that for simplicity, we used a pool with just one thread, so the tasks will run one at a time. In a pool with 5 or more threads, all 5 tasks will actually run more or less at the same time, because, for example, the task with the second-highest priority will be picked up by another thread while the task with the highest priority is still running.

Of course, this is just a pedagogical example. In a realistic use case we may want, for example, to submit tasks that must be completed immediately with high priority so they skip over other tasks already in the queue, or background non-urgent tasks with low priority so they evaluate only after higher-priority tasks are done.

Here are some subtleties to note when using task priority:

* Task priority is facilitated using [`std::priority_queue`](https://en.cppreference.com/w/cpp/container/priority_queue), which has O(log n) complexity for storing new tasks, but only O(1) complexity for retrieving the next (i.e. highest-priority) task. This is in contrast with [`std::queue`](https://en.cppreference.com/w/cpp/container/queue), used if priority is disabled, which both stores and retrieves with O(1) complexity.
* Due to this, enabling the priority queue can incur a very slight decrease in performance, depending on the specific use case, which is why this feature is disabled by default. As usual, there is a trade-off here, where you get functionality in exchange for performance. However, the difference in performance is never substantial, and compiler optimizations can often reduce it to a negligible amount.
* When using the priority queue, tasks will not necessarily be executed in the same order they were submitted, **even if they all have the same priority**. This is due to the implementation of `std::priority_queue` as a [binary heap](https://en.wikipedia.org/wiki/Binary_heap), which means tasks are stored as a binary tree instead of sequentially. To execute tasks in submission order, give them monotonically decreasing priorities.
* Technically, `BS::priority_t` is defined to be ([`std::int_least16_t`](https://en.cppreference.com/w/cpp/types/integer)), since this type is guaranteed to be present on all systems, rather than `std::int16_t`, which is optional in the C++ standard. This means that on some exotic systems `BS::priority_t` may actually have more than 16 bits. However, the pre-defined priorities are 100% portable, and will always have the same values (e.g.: `BS::pr::highest = 32767`) regardless of the actual bit width.

## Testing the library

### Automated tests

The file `BS_thread_pool_test.cpp` in the `tests` folder of the GitHub repository will perform automated tests of all aspects of the library. The output will be printed both to `std::cout` and to a file with the same name as the executable and the suffix `-yyyy-mm-dd_hh.mm.ss.log` based on the current date and time. In addition, the code is meant to serve as an extensive example of how to properly use the library.

Please make sure to:

1. [Compile](#compiling-and-compatibility) `BS_thread_pool_test.cpp` with optimization flags enabled (e.g. `-O3` on GCC / Clang or `/O2` on MSVC).
2. Run the test without any other applications, especially multithreaded applications, running in parallel.

The test program also takes command line arguments for automation purposes:

* `help`: Show a help message and exit. Any other arguments will be ignored.
* `log`: Create a log file.
* `tests`: Perform standard tests.
* `deadlock` Perform long deadlock tests.
* `benchmarks`: Perform benchmarks.

If no options are entered, the default is: `log tests benchmarks`.

By default, the test program enables all the optional features by defining the suitable macros, so it can test them. However, if the macro `BS_THREAD_POOL_LIGHT_TEST` is defined during compilation, the optional features will not be tested.

A PowerShell script, `BS_thread_pool_test.ps1`, is provided for your convenience in the `tests` folder to make running the test on multiple compilers and operating systems easier. Since it is written in PowerShell, it is fully portable and works on Windows, Linux, and macOS. The script will automatically detect if Clang, GCC, and/or MSVC are available, and compile the test program using each available compiler twice - with and without all the optional features. It will then run each compiled test program and report on any errors.

If any of the tests fail, please [submit a bug report](https://github.com/bshoshany/thread-pool/issues) including the exact specifications of your system (OS, CPU, compiler, etc.) and the generated log file.

### Performance tests

If all checks passed, `BS_thread_pool_test.cpp` performs simple benchmarks by filling a very large vector with values using `detach_blocks()`. The program decides what the size of the vector should be by testing how many elements are needed to reach a certain target duration when parallelizing using a number of blocks equal to the number of threads. This ensures that the test takes approximately the same amount of time on all systems, and is thus more consistent and portable.

Once the appropriate size of the vector has been determined, the program allocates the vector and fills it with values, calculated according to a fixed prescription. This operation is performed both single-threaded and multithreaded, with the multithreaded computation spread across multiple tasks submitted to the pool.

Several different multithreaded tests are performed, with the number of tasks either equal to, smaller than, or larger than the pool's thread count. Each test is repeated multiple times, with the run times averaged over all runs of the same test. The program keeps increasing the number of blocks by a factor of 2 until diminishing returns are encountered. The run times of the tests are compared, and the maximum speedup obtained is calculated.

As an example, here are the results of the benchmarks from a [Digital Research Alliance of Canada](https://alliancecan.ca/en) node equipped with two 20-core / 40-thread Intel Xeon Gold 6148 CPUs (for a total of 40 cores and 80 threads), running CentOS Linux 7.9.2009. The tests were compiled using GCC v13.2.0 with the `-O3` and `-march=native` flags. The output was as follows:

```none
======================
Performing benchmarks:
======================
Using 80 threads.
Determining the number of elements to generate in order to achieve an approximate mean execution time of 50 ms with 80 tasks...
Each test will be repeated up to 30 times to collect reliable statistics.
Generating 27962000 elements:
[......]
Single-threaded, mean execution time was 2815.2 ms with standard deviation  3.5 ms.
[......]
With    2 tasks, mean execution time was 1431.3 ms with standard deviation 10.1 ms.
[.......]
With    4 tasks, mean execution time was  722.1 ms with standard deviation 11.4 ms.
[..............]
With    8 tasks, mean execution time was  364.9 ms with standard deviation 10.9 ms.
[............................]
With   16 tasks, mean execution time was  181.9 ms with standard deviation  8.0 ms.
[..............................]
With   32 tasks, mean execution time was  110.6 ms with standard deviation  1.8 ms.
[..............................]
With   64 tasks, mean execution time was   64.0 ms with standard deviation  6.3 ms.
[..............................]
With  128 tasks, mean execution time was   59.8 ms with standard deviation  0.8 ms.
[..............................]
With  256 tasks, mean execution time was   59.0 ms with standard deviation  0.0 ms.
[..............................]
With  512 tasks, mean execution time was   52.8 ms with standard deviation  0.4 ms.
[..............................]
With 1024 tasks, mean execution time was   50.7 ms with standard deviation  0.9 ms.
[..............................]
With 2048 tasks, mean execution time was   50.0 ms with standard deviation  0.5 ms.
[..............................]
With 4096 tasks, mean execution time was   49.4 ms with standard deviation  0.5 ms.
[..............................]
With 8192 tasks, mean execution time was   50.2 ms with standard deviation  0.4 ms.
Maximum speedup obtained by multithreading vs. single-threading: 56.9x, using 4096 tasks.

+++++++++++++++++++++++++++++++++++++++
Thread pool performance test completed!
+++++++++++++++++++++++++++++++++++++++
```

These two CPUs have 40 physical cores in total, with each core providing two separate logical cores via hyperthreading, for a total of 80 threads. Without hyperthreading, we would expect a maximum theoretical speedup of 40x. With hyperthreading, one might naively expect to achieve up to an 80x speedup, but this is in fact impossible, as each pair of hyperthreaded logical cores share the same physical core's resources. However, generally we would expect at most an estimated 30% additional speedup from hyperthreading, which amounts to around 52x in this case. The speedup of 56.9x in our performance test exceeds this estimate.

## Installing the library using package managers

### Installing using vcpkg

If you are using the [vcpkg](https://vcpkg.io/) C/C++ package manager, you can easily install `BS::thread_pool` with the following commands:

On Linux/macOS:

```none
./vcpkg install bshoshany-thread-pool
```

On Windows:

```none
.\vcpkg install bshoshany-thread-pool:x86-windows bshoshany-thread-pool:x64-windows
```

To update the package to the latest version, run:

```none
vcpkg upgrade
```

### Installing using Conan

If you are using the [Conan](https://conan.io/) C/C++ package manager, you can easily integrate `BS::thread_pool` into your project by adding the following lines to your `conanfile.txt`:

```ini
[requires]
bshoshany-thread-pool/4.0.1
```

To update the package to the latest version, simply change the version number. Please refer to [this package's page on ConanCenter](https://conan.io/center/recipes/bshoshany-thread-pool) for more information.

### Installing using Meson

If you are using the [Meson](https://mesonbuild.com/) build system, you can install `BS::thread_pool` from [WrapDB](https://mesonbuild.com/Wrapdb-projects.html). To do so, create a `subprojects` folder in your project (if it does not already exist) and run the following command:

```none
meson wrap install bshoshany-thread-pool
```

Then, use `dependency('bshoshany-thread-pool')` in your `meson.build` file to include the package. To update the package to the latest version, run:

```none
meson wrap update bshoshany-thread-pool
```

### Installing using CMake with CPM

If you are using [CMake](https://cmake.org/), you can install `BS::thread_pool` with [CPM](https://github.com/cpm-cmake/CPM.cmake). If CPM is already installed, simply add the following to your project's `CMakeLists.txt`:

```cmake
CPMAddPackage(
    NAME BS_thread_pool
    GITHUB_REPOSITORY bshoshany/thread-pool
    VERSION 4.0.1)
add_library(BS_thread_pool INTERFACE)
target_include_directories(BS_thread_pool INTERFACE ${BS_thread_pool_SOURCE_DIR}/include)
```

This will automatically download the indicated version of the package from this GitHub repository and include it in your project.

It is also possible to use CPM without installing it first, by adding the following lines to `CMakeLists.txt` before `CPMAddPackage`:

```cmake
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM.cmake")
if(NOT(EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake")
    file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()
include(${CPM_DOWNLOAD_LOCATION})
```

Here is an example of a complete `CMakeLists.txt` for a project named `my_project` consisting of a single source file `main.cpp` which uses `BS_thread_pool.hpp`:

```cmake
cmake_minimum_required(VERSION 3.19)
project(my_project LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM.cmake")
if(NOT(EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake")
    file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()
include(${CPM_DOWNLOAD_LOCATION})
CPMAddPackage(
    NAME BS_thread_pool
    GITHUB_REPOSITORY bshoshany/thread-pool
    VERSION 4.0.1)
add_library(BS_thread_pool INTERFACE)
target_include_directories(BS_thread_pool INTERFACE ${BS_thread_pool_SOURCE_DIR}/include)
add_executable(my_project main.cpp)
target_link_libraries(my_project BS_thread_pool)
```

With both `CMakeLists.txt` and `main.cpp` in the same folder, type the following commands to build the project:

```none
cmake -S . -B build
cmake --build build
```

## Complete library reference

This section provides a complete reference to classes, member functions, objects, and macros available in this library, along with other important information. Member functions are given here with simplified prototypes (e.g. removing `const`) for ease of reading.

More information can be found in the provided [Doxygen](https://www.doxygen.nl/) comments. Any modern IDE, such as [Visual Studio Code](https://code.visualstudio.com/), can use the Doxygen comments to provide automatic documentation for any class and member function in this library when hovering over code with the mouse or using auto-complete.

### Main thread pool header file (`BS_thread_pool.hpp`)

#### The `BS::thread_pool` class

The class `BS::thread_pool` is the main thread pool class. It can be used to create a pool of threads and submit tasks to a queue. When a thread becomes available, it takes a task from the queue and executes it. The member functions that are available by default, when no macros are defined, are:

* Constructors:
    * `thread_pool()`: Construct a new thread pool. The number of threads will be the total number of hardware threads available, as reported by the implementation. This is usually determined by the number of cores in the CPU. If a core is hyperthreaded, it will count as two threads.
    * `thread_pool(BS::concurrency_t num_threads)`: Construct a new thread pool with the specified number of threads.
    * `thread_pool(std::function<void()>& init_task)`: Construct a new thread pool with the specified initialization function.
    * `thread_pool(BS::concurrency_t num_threads, std::function<void()>& init_task)`: Construct a new thread pool with the specified number of threads and initialization function.
* Resetters:
    * `void reset()`: Reset the pool with the total number of hardware threads available, as reported by the implementation. Waits for all currently running tasks to be completed, then destroys all threads in the pool and creates a new thread pool with the new number of threads. Any tasks that were waiting in the queue before the pool was reset will then be executed by the new threads. If the pool was paused before resetting it, the new pool will be paused as well.
    * `void reset(BS::concurrency_t num_threads)`: Reset the pool with a new number of threads.
    * `void reset(std::function<void()>& init_task)` Reset the pool with the total number of hardware threads available, as reported by the implementation, and a new initialization function.
    * `void reset(BS::concurrency_t num_threads, std::function<void()>& init_task)`: Reset the pool with a new number of threads and a new initialization function.
* Getters:
    * `size_t get_tasks_queued()`: Get the number of tasks currently waiting in the queue to be executed by the threads.
    * `size_t get_tasks_running()`: Get the number of tasks currently being executed by the threads.
    * `size_t get_tasks_total()`: Get the total number of unfinished tasks: either still waiting in the queue, or running in a thread. Note that `get_tasks_total() == get_tasks_queued() + get_tasks_running()`.
    * `BS::concurrency_t get_thread_count()`: Get the number of threads in the pool.
    * `std::vector<std::thread::id> get_thread_ids()`: Get a vector containing the unique identifiers for each of the pool's threads, as obtained by `std::thread::get_id()`.
* Task submission without futures (`T` and `F` are template parameters):
    * `void detach_task(F&& task)`: Submit a function with no arguments and no return value into the task queue. To push a function with arguments, enclose it in a lambda expression. Does not return a future, so the user must use `wait()` or some other method to ensure that the task finishes executing, otherwise bad things will happen.
    * `void detach_blocks(T first_index, T index_after_last, F&& block, size_t num_blocks = 0)`: Parallelize a loop by automatically splitting it into blocks and submitting each block separately to the queue. The `block` function takes two arguments, the start and end of the block, so that it is only called only once per block, but it is up to the user make sure the block function correctly deals with all the indices in each block. Does not return a `BS::multi_future`, so the user must use `wait()` or some other method to ensure that the loop finishes executing, otherwise bad things will happen.
    * `void detach_loop(T first_index, T index_after_last, F&& loop, size_t num_blocks = 0)`: Parallelize a loop by automatically splitting it into blocks and submitting each block separately to the queue. The `loop` function takes one argument, the loop index, so that it is called many times per block. Does not return a `BS::multi_future`, so the user must use `wait()` or some other method to ensure that the loop finishes executing, otherwise bad things will happen.
    * `void detach_sequence(T first_index, T index_after_last, F&& sequence)`: Submit a sequence of tasks enumerated by indices to the queue. Does not return a `BS::multi_future`, so the user must use `wait()` or some other method to ensure that the sequence finishes executing, otherwise bad things will happen.
* Task submission with futures (`T`, `F`, and `R` are template parameters):
    * `std::future<R> submit_task(F&& task)`: Submit a function with no arguments into the task queue. To submit a function with arguments, enclose it in a lambda expression. If the function has a return value, get a future for the eventual returned value. If the function has no return value, get an `std::future<void>` which can be used to wait until the task finishes.
    * `BS::multi_future<R> submit_blocks(T first_index, T index_after_last, F&& block, size_t num_blocks = 0)`: Parallelize a loop by automatically splitting it into blocks and submitting each block separately to the queue. The `block` function takes two arguments, the start and end of the block, so that it is only called only once per block, but it is up to the user make sure the block function correctly deals with all the indices in each block. Returns a `BS::multi_future` that contains the futures for all of the blocks.
    * `BS::multi_future<void> submit_loop(T first_index, T index_after_last, F&& loop, size_t num_blocks = 0)`: Parallelize a loop by automatically splitting it into blocks and submitting each block separately to the queue. The `loop` function takes one argument, the loop index, so that it is called many times per block. It must have no return value. Returns a `BS::multi_future` that contains the futures for all of the blocks.
    * `BS::multi_future<R> submit_sequence(T first_index, T index_after_last, F&& sequence)`: Submit a sequence of tasks enumerated by indices to the queue. Returns a `BS::multi_future` that contains the futures for all of the tasks.
* Task management:
    * `void purge()`: Purge all the tasks waiting in the queue. Tasks that are currently running will not be affected, but any tasks still waiting in the queue will be discarded, and will never be executed by the threads. Please note that there is no way to restore the purged tasks.
* Waiting for tasks (`R` and `P`, `C`, and `D` are template parameters):
    * `void wait()`: Wait for tasks to be completed. Normally, this function waits for all tasks, both those that are currently running in the threads and those that are still waiting in the queue. However, if the pool is paused, this function only waits for the currently running tasks (otherwise it would wait forever). Note: To wait for just one specific task, use `submit_task()` instead, and call the `wait()` member function of the generated future.
    * `bool wait_for(std::chrono::duration<R, P>& duration)`: Wait for tasks to be completed, but stop waiting after the specified duration has passed. Returns `true` if all tasks finished running, `false` if the duration expired but some tasks are still running.
    * `bool wait_until(std::chrono::time_point<C, D>& timeout_time)`: Wait for tasks to be completed, but stop waiting after the specified time point has been reached. Returns `true` if all tasks finished running, `false` if the time point was reached but some tasks are still running.

#### Optional features for the `BS::thread_pool` class

The thread pool has several optional features that must be explicitly enabled using macros.

* **Task priority:** Enabled by defining the macro `BS_THREAD_POOL_ENABLE_PRIORITY`.
    * When enabled, the priority of a task or group of tasks may be specified as an additional argument (at the end of the argument list) to `detach_task()`, `submit_task()`, `detach_blocks()`, `submit_blocks()`, `detach_loop()`, `submit_loop()`, `detach_sequence()`, and `submit_sequence()`. If the priority is not specified, the default value will be 0.
    * The priority is a number of type `BS::priority_t`, which is a signed 16-bit integer, so it can have any value between -32,768 and 32,767. The tasks will be executed in priority order from highest to lowest.
    * The namespace `BS::pr` contains some pre-defined priorities: `BS::pr::highest`, `BS::pr::high`, `BS::pr::normal`, `BS::pr::low`, and `BS::pr::lowest`.
* **Pausing:** Enabled by defining the macro `BS_THREAD_POOL_ENABLE_PAUSE`. Adds the following member functions:
    * `void pause()`: Pause the pool. The workers will temporarily stop retrieving new tasks out of the queue, although any tasks already executed will keep running until they are finished.
    * `void unpause()`: Unpause the pool. The workers will resume retrieving new tasks out of the queue.
    * `bool is_paused()`: Check whether the pool is currently paused.
* **Getting the native handles of the threads:** Enabled by defining the macro `BS_THREAD_POOL_ENABLE_NATIVE_HANDLES`. Adds the following member function:
    * `std::vector<std::thread::native_handle_type> get_native_handles()`: Get a vector containing the underlying implementation-defined thread handles for each of the pool's threads.
* **Wait deadlock checks:** Enabled by defining the macro `BS_THREAD_POOL_ENABLE_WAIT_DEADLOCK_CHECK`.
    * When enabled, `wait()`, `wait_for()`, and `wait_until()` will check whether the user tried to call them from within a thread of the same pool, which would result in a deadlock. If so, they will throw the exception `BS::thread_pool::wait_deadlock` instead of waiting.

#### The `BS::this_thread` namespace

The namespace `BS::this_thread` provides functionality similar to `std::this_thread`. It contains the following function objects:

* `BS::this_thread::get_index()` can be used to get the index of the current thread. If this thread belongs to a `BS::thread_pool` object, it will have an index from 0 to `BS::thread_pool::get_thread_count() - 1`. Otherwise, for example if this thread is the main thread or an independent [`std::thread`](https://en.cppreference.com/w/cpp/thread/thread), [`std::nullopt`](https://en.cppreference.com/w/cpp/utility/optional/nullopt) will be returned.
* `BS::this_thread::get_pool()` can be used to get the pointer to the thread pool that owns the current thread. If this thread belongs to a `BS::thread_pool` object, a pointer to that object will be returned. Otherwise, `std::nullopt` will be returned.
* In both cases, an [`std::optional`](https://en.cppreference.com/w/cpp/utility/optional) object will be returned, of type `BS::this_thread::optional_index` or `BS::this_thread::optional_pool` respectively. Unless you are 100% sure this thread is in a pool, first use [`std::optional::has_value()`](https://en.cppreference.com/w/cpp/utility/optional/operator_bool) to check if it contains a value, and if so, use [`std::optional::value()`](https://en.cppreference.com/w/cpp/utility/optional/value) to obtain that value.

#### The `BS::multi_future<T>` class

`BS::multi_future<T>` is a helper class used to facilitate waiting for and/or getting the results of multiple futures at once. It is defined as a specialization of `std::vector<std::future<T>>`. This means that all of the member functions that can be used on an [`std::vector`](https://en.cppreference.com/w/cpp/container/vector) can also be used on a `BS::multi_future`. For example, you may use a range-based for loop with a `BS::multi_future`, since it has iterators.

In addition to inherited member functions, `BS::multi_future` has the following specialized member functions (`R` and `P`, `C`, and `D` are template parameters):

* `[void or std::vector<T>] get()`: Get the results from all the futures stored in this `BS::multi_future`, rethrowing any stored exceptions. If the futures return `void`, this function returns `void` as well. If the futures return a type `T`, this function returns a vector containing the results.
* `size_t ready_count()`: Check how many of the futures stored in this `BS::multi_future` are ready.
* `bool valid()`: Check if all the futures stored in this `BS::multi_future` are valid.
* `void wait()`: Wait for all the futures stored in this `BS::multi_future`.
* `bool wait_for(std::chrono::duration<R, P>& duration)`: Wait for all the futures stored in this `BS::multi_future`, but stop waiting after the specified duration has passed. Returns `true` if all futures have been waited for before the duration expired, `false` otherwise.
* `bool wait_until(std::chrono::time_point<C, D>& timeout_time)`: Wait for all the futures stored in this `multi_future` object, but stop waiting after the specified time point has been reached. Returns `true` if all futures have been waited for before the time point was reached, `false` otherwise.

### Utility header file (`BS_thread_pool_utils.hpp`)

#### The `BS::signaller` class

`BS::signaller` is a utility class which can be used to allow simple signalling between threads. This class is really just a convenient wrapper around [`std::promise`](https://en.cppreference.com/w/cpp/thread/promise), which contains both the promise and its future. It has the following member functions:

* `signaller()`: Construct a new signaller.
* `void wait()`: Wait until the signaller is ready.
* `void ready()`: Inform any waiting threads that the signaller is ready.

#### The `BS::synced_stream` class

`BS::synced_stream` is a utility class which can be used to synchronize printing to an output stream by different threads. It has the following member functions (`T` is a template parameter pack):

* `synced_stream(std::ostream& stream = std::cout)`: Construct a new synced stream which prints to the given output stream.
* `void print(T&&... items)`: Print any number of items into the output stream. Ensures that no other threads print to this stream simultaneously, as long as they all exclusively use the same `synced_stream` object to print.
* `void println(T&&... items)`: Print any number of items into the output stream, followed by a newline character.

In addition, the class comes with two stream manipulators, which are meant to help the compiler figure out which template specializations to use with the class:

* `BS::synced_stream::endl`: An explicit cast of `std::endl`. Prints a newline character to the stream, and then flushes it. Should only be used if flushing is desired, otherwise a newline character should be used instead.
* `BS::synced_stream::flush`: An explicit cast of `std::flush`. Used to flush the stream.

#### The `BS::timer` class

`BS::timer` is a utility class which can be used to measure execution time for benchmarking purposes. It has the following member functions:

* `timer()`: Construct a new timer and immediately start measuring time.
* `void start()`: Start (or restart) measuring time. Note that the timer starts ticking as soon as the object is created, so this is only necessary if we want to restart the clock later.
* `void stop()`: Stop measuring time and store the elapsed time since the object was constructed or since `start()` was last called.
* `std::chrono::milliseconds::rep current_ms()`: Get the number of milliseconds that have elapsed since the object was constructed or since `start()` was last called, but keep the timer ticking.
* `std::chrono::milliseconds::rep ms()`: Get the number of milliseconds stored when `stop()` was last called.

## About the project

### Issue and pull request policy

This library is under continuous and active development. If you encounter any bugs, or if you would like to request any additional features, please feel free to [open a new issue on GitHub](https://github.com/bshoshany/thread-pool/issues) and I will look into it as soon as I can.

Contributions are always welcome. However, I release my projects in cumulative updates after editing and testing them locally on my system, so my policy is not to accept any pull requests. If you open a pull request, and I decide to incorporate your suggestion into the project, I will first modify your code to comply with the project's coding conventions (formatting, syntax, naming, comments, programming practices, etc.), and perform some tests to ensure that the change doesn't break anything. I will then merge it into the next release of the project, possibly together with some other changes. The new release will also include a note in `CHANGELOG.md` with a link to your pull request, and modifications to the documentation in `README.md` as needed.

### Acknowledgements

Many GitHub users have helped improve this project, directly or indirectly, via issues, pull requests, comments, and/or personal correspondence. Please see `CHANGELOG.md` for links to specific issues and pull requests that have been the most helpful. Thank you all for your contribution! :)

### Starring the repository

If you found this project useful, please consider [starring it on GitHub](https://github.com/bshoshany/thread-pool/stargazers)! This allows me to see how many people are using my code, and motivates me to keep working to improve it.

### Copyright and citing

Copyright (c) 2023 [Barak Shoshany](http://baraksh.com). Licensed under the [MIT license](LICENSE.txt).

If you use this C++ thread pool library in software of any kind, please provide a link to [the GitHub repository](https://github.com/bshoshany/thread-pool) in the source code and documentation.

If you use this library in published research, please cite it as follows:

* Barak Shoshany, *"A C++17 Thread Pool for High-Performance Scientific Computing"*, [doi:10.5281/zenodo.4742687](https://doi.org/10.5281/zenodo.4742687), [arXiv:2105.00613](https://arxiv.org/abs/2105.00613) (May 2021)

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

Please note that the [companion paper on arXiv](https://arxiv.org/abs/2105.00613) is updated infrequently. The paper is intended to facilitate discovery of the library by scientists who may find it useful for scientific computing purposes and to allow citing the library in scientific research, but most users should read the `README.md` file on [the GitHub repository](https://github.com/bshoshany/thread-pool) instead, as it is guaranteed to always be up to date.

### Learning more about C++

Beginner C++ programmers may be interested in [my lecture notes](https://baraksh.com/CSE701/notes.php) for a course taught at McMaster University, which teach modern C and C++ from scratch, including some of the advanced techniques and programming practices used in developing this library.
