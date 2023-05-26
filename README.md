[![DOI:10.5281/zenodo.4742687](https://zenodo.org/badge/DOI/10.5281/zenodo.4742687.svg)](https://doi.org/10.5281/zenodo.4742687)
[![arXiv:2105.00613](https://img.shields.io/badge/arXiv-2105.00613-b31b1b.svg)](https://arxiv.org/abs/2105.00613)
[![License: MIT](https://img.shields.io/github/license/bshoshany/thread-pool)](https://github.com/bshoshany/thread-pool/blob/master/LICENSE.txt)
![Language: C++17](https://img.shields.io/badge/Language-C%2B%2B17-yellow)
![File size in bytes](https://img.shields.io/github/size/bshoshany/thread-pool/BS_thread_pool.hpp)
![GitHub last commit](https://img.shields.io/github/last-commit/bshoshany/thread-pool)
[![GitHub repo stars](https://img.shields.io/github/stars/bshoshany/thread-pool?style=social)](https://github.com/bshoshany/thread-pool)
[![Open in Visual Studio Code](https://img.shields.io/badge/-Open%20in%20Visual%20Studio%20Code-007acc)](https://vscode.dev/github/bshoshany/thread-pool)

# `BS::thread_pool`: a fast, lightweight, and easy-to-use C++17 thread pool library

By Barak Shoshany\
Email: <baraksh@gmail.com>\
Website: <https://baraksh.com/>\
GitHub: <https://github.com/bshoshany>

This is the complete documentation for v3.5.0 of the library, released on 2023-05-25.

* [Introduction](#introduction)
    * [Motivation](#motivation)
    * [Overview of features](#overview-of-features)
    * [Compiling and compatibility](#compiling-and-compatibility)
    * [Installing using vcpkg](#installing-using-vcpkg)
    * [Installing using Conan](#installing-using-conan)
* [Getting started](#getting-started)
    * [Including the library](#including-the-library)
    * [Constructors](#constructors)
    * [Getting and resetting the number of threads in the pool](#getting-and-resetting-the-number-of-threads-in-the-pool)
    * [Finding the version of the package](#finding-the-version-of-the-package)
* [Submitting and waiting for tasks](#submitting-and-waiting-for-tasks)
    * [Submitting tasks to the queue with futures](#submitting-tasks-to-the-queue-with-futures)
    * [Submitting tasks to the queue without futures](#submitting-tasks-to-the-queue-without-futures)
    * [Manually waiting for all tasks to complete](#manually-waiting-for-all-tasks-to-complete)
    * [Waiting with a timeout](#waiting-with-a-timeout)
    * [Submitting class member functions to the queue](#submitting-class-member-functions-to-the-queue)
    * [Passing task arguments by reference](#passing-task-arguments-by-reference)
* [Parallelizing loops](#parallelizing-loops)
    * [Automatic parallelization of loops](#automatic-parallelization-of-loops)
    * [Loops with return values](#loops-with-return-values)
    * [Parallelizing loops without futures](#parallelizing-loops-without-futures)
* [Helper classes](#helper-classes)
    * [Synchronizing printing to an output stream](#synchronizing-printing-to-an-output-stream)
    * [Handling multiple futures at once](#handling-multiple-futures-at-once)
    * [Measuring execution time](#measuring-execution-time)
* [Other features](#other-features)
    * [Monitoring the tasks](#monitoring-the-tasks)
    * [Pausing the workers](#pausing-the-workers)
    * [Purging tasks](#purging-tasks)
    * [Exception handling](#exception-handling)
* [Testing the package](#testing-the-package)
    * [Automated tests](#automated-tests)
    * [Performance tests](#performance-tests)
* [The light version of the package](#the-light-version-of-the-package)
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

The design of this package was guided by four important principles. First, *compactness*: the entire library consists of just one small self-contained header file, with no other components or dependencies. Second, *portability*: the package only utilizes the C\+\+17 standard library, without relying on any compiler extensions or 3rd-party libraries, and is therefore compatible with any modern standards-conforming C\+\+17 compiler on any platform. Third, *ease of use*: the package is extensively documented, and programmers of any level should be able to use it right out of the box.

The fourth and final guiding principle is *performance*: each and every line of code in this library was carefully designed with maximum performance in mind, and performance was tested and verified on a variety of compilers and platforms. Indeed, the library was originally designed for use in the author's own computationally-intensive scientific computing projects, running both on high-end desktop/laptop computers and high-performance computing nodes.

Other, more advanced multithreading libraries may offer more features and/or higher performance. However, they typically consist of a vast codebase with multiple components and dependencies, and involve complex APIs that require a substantial time investment to learn. This library is not intended to replace these more advanced libraries; instead, it was designed for users who don't require very advanced features, and prefer a simple and lightweight package that is easy to learn and use and can be readily incorporated into existing or new projects.

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
    * Only 247 lines of code, excluding comments, blank lines, and lines containing only a single brace.
    * A stand-alone "light version" of the C++ thread pool is also available in the `BS_thread_pool_light.hpp` header file, with only 115 lines of code.
* **Easy to use:**
    * Very simple operation, using a handful of member functions.
    * Every task submitted to the queue using the `submit()` member function automatically generates an `std::future`, which can be used to wait for the task to finish executing and/or obtain its eventual return value.
    * Loops can be automatically parallelized into any number of parallel tasks using the `parallelize_loop()` member function, which returns a `BS::multi_future` (see below) that can be used to track the execution of all parallel tasks at once.
    * If futures are not needed, tasks may be submitted using `push_task()`, and loops can be parallelized using `push_loop()` - sacrificing convenience for even greater performance.
    * The code is thoroughly documented using Doxygen comments - not only the interface, but also the implementation, in case the user would like to make modifications.
    * The included test program `BS_thread_pool_test.cpp` can be used to perform exhaustive automated tests and benchmarks, and also serves as a comprehensive example of how to properly use the package.
* **Helper classes:**
    * Track the execution of multiple futures at once using the `BS::multi_future` helper class.
    * Synchronize output to a stream from multiple threads in parallel using the `BS::synced_stream` helper class.
    * Easily measure execution time for benchmarking purposes using the `BS::timer` helper class.
* **Additional features:**
    * Easily wait for all tasks in the queue to complete using the `wait_for_tasks()`, `wait_for_tasks_duration()`, and `wait_for_tasks_until()` member functions.
    * Change the number of threads in the pool safely and on-the-fly as needed using the `reset()` member function.
    * Monitor the number of queued and/or running tasks using the `get_tasks_queued()`, `get_tasks_running()`, and `get_tasks_total()` member functions.
    * Freely pause and resume the pool using the `pause()`, `unpause()`, and `is_paused()` member functions; when paused, threads do not retrieve new tasks out of the queue.
    * Purge all tasks currently waiting in the queue with the `purge()` member function.
    * Catch exceptions thrown by tasks submitted using `submit()` or `parallelize_loop()` from the main thread through their futures.
    * Submit class member functions to the pool, either applied to a specific object or from within the object itself.
    * Pass arguments to tasks by value, reference, or constant reference.
    * Under continuous and active development. Bug reports and feature requests are welcome, and should be made via [GitHub issues](https://github.com/bshoshany/thread-pool/issues).

### Compiling and compatibility

This library should successfully compile on any C++17 standard-compliant compiler, on all operating systems and architectures for which such a compiler is available. Compatibility was verified with a 24-core (8P+16E) / 32-thread Intel i9-13900K CPU using the following compilers and platforms:

* Windows 11 build 22621.1702:
    * [Clang](https://clang.llvm.org/) v16.0.4
    * [GCC](https://gcc.gnu.org/) v13.1.0 ([WinLibs build](https://winlibs.com/))
    * [MSVC](https://docs.microsoft.com/en-us/cpp/) v19.36.32532
* Ubuntu 23.04:
    * [Clang](https://clang.llvm.org/) v16.0.0
    * [GCC](https://gcc.gnu.org/) v13.0.1

In addition, this library was tested on a [Digital Research Alliance of Canada](https://alliancecan.ca/en) node equipped with two 20-core / 40-thread Intel Xeon Gold 6148 CPUs (for a total of 40 cores and 80 threads), running CentOS Linux 7.9.2009, using [GCC](https://gcc.gnu.org/) v12.2.0.

The test program `BS_thread_pool_test.cpp` was compiled without warnings (with the warning flags `-Wall -Wextra -Wconversion -Wsign-conversion -Wpedantic -Weffc++ -Wshadow` in GCC/Clang and `/W4` in MSVC), executed, and successfully completed all [automated tests](#testing-the-package) and benchmarks using all of the compilers and systems mentioned above.

As this library requires C\+\+17 features, the code must be compiled with C\+\+17 support:

* For Clang or GCC, use the `-std=c++17` flag. On Linux, you will also need to use the `-pthread` flag to enable the POSIX threads library.
* For MSVC, use `/std:c++17`, and preferably also `/permissive-` to ensure standards conformance.

For maximum performance, it is recommended to compile with all available compiler optimizations:

* For Clang or GCC, use the `-O3` flag.
* For MSVC, use `/O2`.

As an example, to compile the test program `BS_thread_pool_test.cpp` with warnings and optimizations, it is recommended to use the following commands:

* On Linux with GCC: `g++ BS_thread_pool_test.cpp -std=c++17 -O3 -Wall -Wextra -Wconversion -Wsign-conversion -Wpedantic -Weffc++ -Wshadow -pthread -o BS_thread_pool_test`
* On Linux with Clang: replace `g++` with `clang++`.
* On Windows with GCC or Clang: replace `-o BS_thread_pool_test` with `-o BS_thread_pool_test.exe` and remove `-pthread`.
* On Windows with MSVC: `cl BS_thread_pool_test.cpp /std:c++17 /permissive- /O2 /W4 /EHsc /Fe:BS_thread_pool_test.exe`

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

The C++ thread pool will then be available automatically in the build system you integrated vcpkg with (e.g. MSBuild or CMake). Simply write `#include "BS_thread_pool.hpp"` in any project to use the thread pool, without having to copy to file into the project first. I will update the vcpkg port with each new release, so it will be updated automatically when you run `vcpkg upgrade`.

Please see the [vcpkg repository](https://github.com/microsoft/vcpkg) for more information on how to use vcpkg.

### Installing using Conan

If you are using the [Conan](https://conan.io/) C/C++ package manager, please refer to [this package's page on ConanCenter](https://conan.io/center/bshoshany-thread-pool) to learn how to use Conan to include this package in your project with various build systems.

## Getting started

### Including the library

If you are not using a C++ library manager (such as vcpkg), simply download the [latest release](https://github.com/bshoshany/thread-pool/releases) from the GitHub repository, place the single header file `BS_thread_pool.hpp` from the `include` folder of the repository in the desired folder, and include it in your program:

```cpp
#include "BS_thread_pool.hpp"
```

The thread pool will now be accessible via the `BS::thread_pool` class.

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

If your program's main thread only submits tasks to the thread pool and waits for them to finish, and does not perform any computationally intensive tasks on its own, then it is recommended to use the default value for the number of threads. This ensures that all of the threads available in the hardware will be put to work while the main thread waits.

However, if your main thread does perform computationally intensive tasks on its own, then it is recommended to use the value `std::thread::hardware_concurrency() - 1` for the number of threads. In this case, the main thread plus the thread pool will together take up exactly all the threads available in the hardware.

### Getting and resetting the number of threads in the pool

The member function `get_thread_count()` returns the number of threads in the pool. This will be equal to `std::thread::hardware_concurrency()` if the default constructor was used.

It is generally unnecessary to change the number of threads in the pool after it has been created, since the whole point of a thread pool is that you only create the threads once. However, if needed, this can be done, safely and on-the-fly, using the `reset()` member function.

`reset()` will wait for all currently running tasks to be completed, but will leave the rest of the tasks in the queue. Then it will destroy the thread pool and create a new one with the desired new number of threads, as specified in the function's argument (or the hardware concurrency if no argument is given). The new thread pool will then resume executing the tasks that remained in the queue and any new submitted tasks.

### Finding the version of the package

If desired, the version of this package may be read during compilation time from the macro `BS_THREAD_POOL_VERSION`. The value will be a string containing the version number and release date. For example:

```cpp
std::cout << "Thread pool library version is " << BS_THREAD_POOL_VERSION << ".\n";
```

Sample output:

```none
Thread pool library version is v3.5.0 (2023-05-25).
```

This can be used, for example, to allow the same code to work with several incompatible versions of the library.

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

If the submitted function has a return value of type `T`, then the future will be of type `std::future<T>`, and will be set to the return value when the function finishes its execution. If the submitted function does not have a return value, then the future will be an `std::future<void>`, which will not return any value but may still be used to wait for the function to finish.

Using `auto` for the return value of `submit()` means the compiler will automatically detect which instance of the template `std::future` to use. However, specifying the particular type `std::future<T>`, as in the examples below, is recommended for increased readability.

To wait until the task finishes, use the member function `wait()` of the future. To obtain the return value, use the member function `get()`, which will also automatically wait for the task to finish if it hasn't yet. For example:

```cpp
// Submit a task and get a future.
auto my_future = pool.submit(task);
// Do some other stuff while the task is executing.
do_stuff();
// Get the task's return value from the future, waiting for it to finish running if needed.
auto my_return_value = my_future.get();
```

Here are some more concrete examples. The following program will print out `42`:

```cpp
#include "BS_thread_pool.hpp"

int main()
{
    BS::thread_pool pool;
    std::future<int> my_future = pool.submit([] { return 42; });
    std::cout << my_future.get();
}
```

Here we used a [lambda expression](https://en.cppreference.com/w/cpp/language/lambda) to quickly define the function on-the-fly. However, we can also use a previously-defined function:

```cpp
#include "BS_thread_pool.hpp"

int the_answer()
{
    return 42;
}

int main()
{
    BS::thread_pool pool;
    std::future<int> my_future = pool.submit(the_answer);
    std::cout << my_future.get();
}
```

The following is an example of submitting a function with arguments:

```cpp
#include "BS_thread_pool.hpp"

int multiply(const int a, const int b)
{
    return a * b;
}

int main()
{
    BS::thread_pool pool;
    std::future<int> my_future = pool.submit(multiply, 6, 7);
    std::cout << my_future.get();
}
```

Finally, here is an example of submitting a function with no return value and then using the future to wait for it to finish executing:

```cpp
#include "BS_thread_pool.hpp"

void sleep()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

int main()
{
    BS::thread_pool pool;
    std::future<void> my_future = pool.submit(sleep);
    std::cout << "Waiting... ";
    my_future.wait();
    std::cout << "Done.";
}
```

Here, the command `std::this_thread::sleep_for(std::chrono::milliseconds(1000))` instructs the thread to sleep for 1 second.

### Submitting tasks to the queue without futures

Usually, it is best to submit a task to the queue using `submit()`. This allows you to wait for the task to finish and/or get its return value later. However, sometimes a future is not needed, for example when you just want to "set and forget" a certain task, or if the task already communicates with the main thread or with other tasks without using futures, such as via condition variables. In such cases, you may wish to avoid the overhead involved in assigning a future to the task in order to increase performance.

The member function `push_task()` allows you to submit a task to the queue without generating a future for it. The task can have any number of arguments, but it cannot have a return value. For example:

```cpp
// Submit a task without arguments or return value to the queue.
pool.push_task(task);
// Submit a task with one argument and no return value to the queue.
pool.push_task(task, arg);
// Submit a task with two arguments and no return value to the queue.
pool.push_task(task, arg1, arg2);
```

**Warning:** Since `push_task()` does not return a future, there is no built-in way for the user to know when the task finishes executing. You must use either `wait_for_tasks()` (see below), or some other method such as condition variables, to ensure that the task finishes executing before trying to use anything that depends on its output. Otherwise, bad things will happen!

### Manually waiting for all tasks to complete

To wait for a **single** submitted task to complete, use `submit()` and then use the `wait()` or `get()` member functions of the obtained future. However, in cases where you need to wait until **all** submitted tasks finish their execution, or if the tasks have been submitted without futures using `push_task()`, you can use the member function `wait_for_tasks()`.

Consider, for example, the following code:

```cpp
#include "BS_thread_pool.hpp"

int main()
{
    BS::thread_pool pool(5);
    int squares[100];
    for (int i = 0; i < 100; ++i)
        pool.push_task(
            [&squares, i]
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                squares[i] = i * i;
            });
    std::cout << squares[50];
}
```

The output will most likely be garbage, since the task that modifies `squares[50]` has not yet finished executing by the time we try to access that element - it's still waiting in the queue. One solution would be to use `submit()` instead of `push_task()`, but perhaps we don't want the overhead of generating 100 different futures. Instead, simply adding the line

```cpp
pool.wait_for_tasks();
```

after the `for` loop will ensure - as efficiently as possible - that all tasks have finished running before we attempt to access any elements of the array `squares`, and the code will print out the value `2500` as expected.

Note, however, that `wait_for_tasks()` will wait for **all** the tasks in the queue, including those that are unrelated to the `for` loop. Using [`parallelize_loop()`](#parallelizing-loops) would make much more sense in this particular case, as it will allow waiting only for the tasks related to the loop.

**Warning:** Never call `wait_for_tasks()` from within a thread of the same thread pool, for example `pool.push_task([] { pool.wait_for_tasks(); })`, as that will cause it to wait forever!

### Waiting with a timeout

Sometimes you may wish to wait for the tasks to complete, but only for a certain amount of time, or until a specific point in time. For example, if the tasks have not yet completed after some time, you may wish to let the user know that there is a delay. This can be achieved using two member functions:

* `wait_for_tasks_duration()` waits for the tasks to be completed, but stops waiting after the specified duration, given as an argument of type `std::chrono::duration`, has passed.
* `wait_for_tasks_until()` waits for the tasks to be completed, but stops waiting after the specified time point, given as an argument of type `std::chrono::time_point`, has been reached.

Here is an example:

```cpp
#include "BS_thread_pool.hpp"

int main()
{
    BS::thread_pool pool;
    std::atomic<bool> done = false;
    pool.push_task(
        [&done]
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            done = true;
        });
    while (true)
    {
        pool.wait_for_tasks_duration(std::chrono::milliseconds(200));
        if (!done)
            std::cout << "Sorry, task is not done yet.\n";
        else
            break;
    }
    std::cout << "Task done!\n";
}
```

The output is:

```none
Sorry, task is not done yet.
Sorry, task is not done yet.
Sorry, task is not done yet.
Sorry, task is not done yet.
Task done!
```

### Submitting class member functions to the queue

Consider the following program:

```cpp
#include "BS_thread_pool.hpp"

BS::thread_pool pool;

class flag_class
{
public:
    bool get_flag() const
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

This program creates a new object `flag_object` of the class `flag_class`, sets the flag to `true` using the member function `set_flag()`, and then prints out the flag's value. But what if you want to submit the member function `set_flag()` as a task to the thread pool?

To submit member functions to the pool, use the following general syntax:

```cpp
pool.push_task(&class::function, &object, args);
```

The same syntax also works with `submit()`. Note that, in the second argument, you must specify the object on which the member function will be executed (unless it's a static member function, in which case you just submit it like any other function). Also note that both the first and second arguments must be **pointers**, so they must be preceded by the `&` operator.

If you remove the `&` from the first argument, the code won't work with most compilers, and if you remove the `&` from the second argument, the function will act on a **copy** of the object, rather than on the object itself, so any changes made to the object will not be saved. Therefore, it's important to ensure that both arguments are pointers.

To make the above program submit the member function `set_flag()` to the thread pool, simply replace the line:

```cpp
flag_object.set_flag(true);
```

with:

```cpp
pool.push_task(&flag_class::set_flag, &flag_object, true);
pool.wait_for_tasks();
```

Here the class is `flag_class`, the name of the function is `set_flag`, the object we want the function to act on is `flag_object`, and the argument to pass to the function is `true`.

Another thing you might want to do is call a member function from within the object itself, that is, from another member function. This follows a similar syntax, except that you don't need to specify the class, and you use `this` to get a pointer to the current object (no `&` necessary, since `this` is already a pointer). Here is an example, this time using `submit()`:

```cpp
#include "BS_thread_pool.hpp"

BS::thread_pool pool;

class flag_class
{
public:
    bool get_flag() const
    {
        return flag;
    }

    void set_flag(const bool arg)
    {
        flag = arg;
    }

    void set_flag_to_true()
    {
        pool.submit(&flag_class::set_flag, this, true).wait();
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

### Passing task arguments by reference

In C++, it is often crucial to pass function arguments by reference or constant reference, instead of by value. This allows the function to access the object being passed directly, rather than creating a new copy of the object.

When submitting a task using `push_task()` or `submit()`, the task's arguments are always passed by value by default. To pass arguments to the task by reference, you must wrap them with `std::ref()`. Similarly, to pass arguments by constant reference, you must wrap them with `std::cref()`. Here is an example:

```cpp
#include "BS_thread_pool.hpp"

BS::thread_pool pool;

void increment(int& x)
{
    ++x;
}

void print(const int& x)
{
    std::cout << x;
}

int main()
{
    int n = 0;
    pool.submit(increment, std::ref(n)).wait();
    pool.submit(print, std::cref(n)).wait();
}
```

The `increment()` function takes a **reference** to an integer, and increments that integer. Passing the argument by reference guarantees that `n` itself, in the scope of `main()`, will be incremented - rather than a copy of it in the scope of `increment()`. To pass `n` by reference, we wrapped it inside `std::ref()`. Note that the program will not compile otherwise, since `increment()` only accepts arguments by reference.

Similarly, the `print()` function takes a **constant reference** to an integer, and prints that integer. Passing the argument by constant reference guarantees that the variable will not be accidentally modified by the function, even though we are accessing `n` itself, rather than a copy. To pass `n` by constant reference, we wrapped it inside `std::cref()`.

## Parallelizing loops

### Automatic parallelization of loops

One of the most common and effective methods of parallelization is splitting a loop into smaller loops and running them in parallel. It is most effective in "embarrassingly parallel" computations, such as vector or matrix operations, where each iteration of the loop is completely independent of every other iteration. For example, if we are summing up two vectors of 1000 elements each, and we have 10 threads, we could split the summation into 10 blocks of 100 elements each, and run all the blocks in parallel, potentially increasing performance by up to a factor of 10.

`BS::thread_pool` can automatically parallelize loops. To see how this works, consider the following generic loop:

```cpp
for (T i = start; i < end; ++i)
    do_something(i);
```

where:

* `T` is any signed or unsigned integer type.
* The loop is over the range `[start, end)`, i.e. inclusive of `start` but exclusive of `end`.
* `do_something()` is an operation performed for each loop index `i`, such as modifying an array with `end - start` elements.

This loop may be automatically parallelized and submitted to the thread pool's queue using the member function `parallelize_loop()`, which has the follows syntax:

```cpp
pool.parallelize_loop(start, end, loop, num_blocks);
```

where:

* `start` is the first index in the range.
    * This argument can be omitted, in which case it is assumed that the loop starts at 0. That is, `parallelize_loop(end, loop, num_blocks)` is equivalent to `parallelize_loop(0, end, loop, num_blocks)`.
* `end` is the index after the last index in the range, such that the full range is `[start, end)`. In other words, the loop will be equivalent to the one above if `start` and `end` are the same.
    * `start` and `end` should both be integers, but they need not be of the same integer type. `parallelize_loop()` will automatically determine the best type to use for the loop indices.
* `loop()` is any function that takes two indices, `a`, and `b`, and executes only the portion of the loop in the range `[a, b)`. Typically, `loop()` will include a `for` loop of the form `for (T i = a; i < b; ++i)`.
* `num_blocks` is the number of blocks of the form `[a, b)` to split the loop into. For example, if the range is `[0, 9)` and there are 3 blocks, then the blocks will be the ranges `[0, 3)`, `[3, 6)`, and `[6, 9)`. If possible, the blocks will be equal in size; otherwise, the last block may be a bit longer.
    * This argument can be omitted, in which case the number of blocks will be the number of threads in the pool.

Each block will be submitted to the thread pool's queue as a separate task. Therefore, a loop that is split into 3 blocks will be split into 3 individual tasks, which may run in parallel. If there is only one block, then the entire loop will run as one task, and no parallelization will take place.

To parallelize the generic loop above, we use the following code:

```cpp
auto loop = [](const T a, const T b)
{
    for (T i = a; i < b; ++i)
        do_something(i);
};
BS::multi_future<void> loop_future = pool.parallelize_loop(start, end, loop, num_blocks);
loop_future.wait();
```

Here we defined `loop()` as a lambda function. Of course, `loop()` could also be defined as a lambda within the call to `parallelize_loop()` itself, as in the examples below; or it could be any ordinary function, but a lambda is preferred since one typically would like to capture some of the surrounding variables.

`parallelize_loop()` returns an object of the helper class template `BS::multi_future`. Each of the `num_blocks` blocks will have an `std::future` assigned to it, and all these futures will be stored inside the returned `BS::multi_future` object. When `loop_future.wait()` is called, the main thread will wait until all tasks generated by `parallelize_loop()` finish executing, and only those tasks - not any other tasks that also happen to be in the queue. This is essentially the role of the `BS::multi_future` class: to wait for a specific **group of tasks**, in this case the tasks running the loop blocks.

What value should you use for `num_blocks`? Omitting this argument, so that the number of blocks will be equal to the number of threads in the pool, is typically a good choice. For best performance, it is recommended to do your own benchmarks to find the optimal number of blocks for each loop (you can use the `BS::timer` helper class - see [below](#measuring-execution-time)). Using less tasks than there are threads may be preferred if you are also running other tasks in parallel. Using more tasks than there are threads may improve performance in some cases.

As a simple example, the following code calculates and prints the squares of all integers from 0 to 99:

```cpp
#include "BS_thread_pool.hpp"

int main()
{
    int squares[100];
    for (int i = 0; i < 100; ++i)
    {
        squares[i] = i * i;
        std::cout << i << "^2 = " << squares[i] << "   ";
    }
}
```

We can parallelize it as follows:

```cpp
#include "BS_thread_pool.hpp"

int main()
{
    BS::thread_pool pool(10);
    int squares[100];
    pool.parallelize_loop(100,
                          [&squares](const int a, const int b)
                          {
                              for (int i = a; i < b; ++i)
                                  squares[i] = i * i;
                          })
        .wait();
    for (int i = 0; i < 100; ++i)
        std::cout << i << "^2 = " << squares[i] << "   ";
}
```

Since there are 10 threads, and we omitted the `num_blocks` argument, the loop will be divided into 10 blocks, each calculating 10 squares. Also, since the loop starts from 0, we did not need to specify the first index.

In this example, instead of storing the `BS::multi_future` object and then using it to wait, we simply called the `wait()` member function directly on the temporary object returned by `parallelize_loop()` without storing it anywhere. This is a convenient shortcut when we have nothing else to do while waiting.

Notice that here we parallelized the calculation of the squares, but we did not parallelize printing the results. This is for two reasons:

1. We want to print out the squares in ascending order, and we have no guarantee that the blocks will be executed in the correct order. This is very important; you must never expect that the parallelized loop will execute at the same order as the non-parallelized loop.
2. If we did print out the squares from within the parallel tasks, we would get a huge mess, since all 10 blocks would print to the standard output at once. [Later](#synchronizing-printing-to-an-output-stream) we will see how to synchronize printing to a stream from multiple tasks at the same time.

### Loops with return values

Usually, `parallelize_loop()` should take functions with no return values. This is because the function will be executed once for each block, but the blocks are managed by the thread pool, so there's limited usability in returning one value per block. However, for the case where this is desired, such as for summation or some sorting algorithms, `parallelize_loop()` does accept functions with return values, in which case it returns a `BS::multi_future<T>` object where `T` is the type of the return values.

Here's an example of summing all the numbers from 1 to 100:

```cpp
#include "BS_thread_pool.hpp"

int main()
{
    BS::thread_pool pool;
    BS::multi_future<int> mf = pool.parallelize_loop(1, 101,
                                                     [](const int a, const int b)
                                                     {
                                                         int block_total = 0;
                                                         for (int i = a; i < b; ++i)
                                                             block_total += i;
                                                         return block_total;
                                                     });
    std::vector<int> totals = mf.get();
    int sum = 0;
    for (const int t : totals)
        sum += t;
    std::cout << sum;
}
```

Calling `get()` on a `BS::multi_future<T>` object returns an `std::vector<T>` with the values obtained from each future. In this case, the values will be the partial sums from each block, so when we add them up, we will get the total sum.

### Parallelizing loops without futures

Just as in the case of [`push_task()`](#submitting-tasks-to-the-queue-without-futures) vs. [`submit()`](#submitting-tasks-to-the-queue-with-futures), sometimes you may want to parallelize a loop, but you don't need it to return a `BS::multi_future`. In this case, you can save the overhead of generating the futures (which can be significant, depending on the number of blocks) by using `push_loop()` instead of `parallelize_loop()`, with the same arguments.

For example, you could also run the loop of squares example above as follows:

```cpp
#include "BS_thread_pool.hpp"

int main()
{
    BS::thread_pool pool(10);
    int squares[100];
    pool.push_loop(100,
                   [&squares](const int a, const int b)
                   {
                       for (int i = a; i < b; ++i)
                           squares[i] = i * i;
                   });
    pool.wait_for_tasks();
    for (int i = 0; i < 100; ++i)
        std::cout << i << "^2 = " << squares[i] << "   ";
}
```

As with `parallelize_loop()`, the first argument can be omitted if the start index is 0, and the last argument can be omitted if the number of blocks should be equal to the number of threads.

**Warning:** Since `push_loop()` does not return a `BS::multi_future`, there is no built-in way for the user to know when the loop finishes executing. You must use either [`wait_for_tasks()`](#manually-waiting-for-all-tasks-to-complete), or some other method such as condition variables, to ensure that the loop finishes executing before trying to use anything that depends on its output. Otherwise, bad things will happen!

## Helper classes

### Synchronizing printing to an output stream

When printing to an output stream from multiple threads in parallel, the output may become garbled. For example, consider this code:

```cpp
#include "BS_thread_pool.hpp"

int main()
{
    BS::thread_pool pool;
    for (size_t i = 1; i <= 5; ++i)
        pool.push_task([i] { std::cout << "Task no. " << i << " executing.\n"; });
}
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

The helper class `BS::synced_stream` is designed to eliminate such synchronization issues. The constructor takes one optional argument, specifying the output stream to print to. If no argument is supplied, `std::cout` will be used:

```cpp
// Construct a synced stream that will print to std::cout.
BS::synced_stream sync_out;
// Construct a synced stream that will print to the output stream my_stream.
BS::synced_stream sync_out(my_stream);
```

The member function `print()` takes an arbitrary number of arguments, which are inserted into the stream one by one, in the order they were given. `println()` does the same, but also prints a newline character `\n` at the end, for convenience. A mutex is used to synchronize this process, so that any other calls to `print()` or `println()` using the same `BS::synced_stream` object must wait until the previous call has finished.

As an example, this code:

```cpp
#include "BS_thread_pool.hpp"

int main()
{
    BS::synced_stream sync_out;
    BS::thread_pool pool;
    for (size_t i = 1; i <= 5; ++i)
        pool.push_task([i, &sync_out] { sync_out.println("Task no. ", i, " executing."); });
}
```

Will print out:

```none
Task no. 1 executing.
Task no. 2 executing.
Task no. 3 executing.
Task no. 4 executing.
Task no. 5 executing.
```

**Warning:** Always create the `BS::synced_stream` object **before** the `BS::thread_pool` object, as we did in this example. When the `BS::thread_pool` object goes out of scope, it waits for the remaining tasks to be executed. If the `BS::synced_stream` object goes out of scope before the `BS::thread_pool` object, then any tasks using the `BS::synced_stream` will crash. Since objects are destructed in the opposite order of construction, creating the `BS::synced_stream` object before the `BS::thread_pool` object ensures that the `BS::synced_stream` is always available to the tasks, even while the pool is destructing.

Most stream manipulators defined in the headers `<ios>` and `<iomanip>`, such as `std::setw` (set the character width of the next output), `std::setprecision` (set the precision of floating point numbers), and `std::fixed` (display floating point numbers with a fixed number of digits), can be passed to `print()` and `println()` just as you would pass them to a stream.

The only exceptions are the flushing manipulators `std::endl` and `std::flush`, which will not work because the compiler will not be able to figure out which template specializations to use. Instead, use `BS::synced_stream::endl` and `BS::synced_stream::flush`. Here is an example:

```cpp
#include "BS_thread_pool.hpp"
#include <cmath>
#include <iomanip>

int main()
{
    BS::synced_stream sync_out;
    BS::thread_pool pool;
    sync_out.print(std::setprecision(10), std::fixed);
    for (size_t i = 1; i <= 10; ++i)
        pool.push_task([i, &sync_out] { sync_out.print("The square root of ", std::setw(2), i, " is ", std::sqrt(i), ".", BS::synced_stream::endl); });
}
```

### Handling multiple futures at once

The helper class template `BS::multi_future<T>`, already introduced in the context of `parallelize_loop()`, provides a convenient way to collect and access groups of futures. This class works similarly to STL containers such as `std::vector`:

* When you create a new object, either use the default constructor to create an empty object and add futures to it later, or pass the desired number of futures to the constructor in advance.
* Use the `[]` operator to access the future at a specific index, or the `push_back()` member function to append a new future to the list.
* The `size()` member function tells you how many futures are currently stored in the object.
* Once all the futures are stored, you can use `wait()` to wait for all of them at once or `get()` to get an `std::vector<T>` with the results from all of them.

Aside from using `BS::multi_future` to track the execution of parallelized loops, it can also be used whenever you have several different groups of tasks and you want to track the execution of each group individually. Here's a simple example:

```cpp
#include "BS_thread_pool.hpp"
#include <cmath>

BS::synced_stream sync_out;
BS::thread_pool pool;

double power(const double i, const double j)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(10 * pool.get_thread_count()));
    return std::pow(i, j);
}

void print_vector(const std::vector<double>& vec)
{
    for (const double i : vec)
        sync_out.print(i, ' ');
    sync_out.println();
}

int main()
{
    constexpr size_t n = 100;

    // First group of tasks: calculate n squares.
    // Here we create an empty BS::multi_future object, and append futures to it via push_back().
    BS::multi_future<double> mf_squares;
    for (int i = 0; i < n; ++i)
        mf_squares.push_back(pool.submit(power, i, 2));

    // Second group of tasks: calculate n cubes.
    // In this case, we create a BS::multi_future object of the desired size in advance, and store the futures via the [] operator. This is faster since there will be no memory reallocations, but also more prone to errors.
    BS::multi_future<double> mf_cubes(n);
    for (int i = 0; i < n; ++i)
        mf_cubes[i] = pool.submit(power, i, 3);

    // Both groups are now queued, but it will take some time until they all execute.

    /// ...
    /// Do some stuff while the first group of tasks executes...
    /// ...

    // Get and print the results from the first group.
    sync_out.println("Squares:");
    print_vector(mf_squares.get());

    /// ...
    /// Do other stuff while the second group of tasks executes...
    /// ...

    // Get and print the results from the second group.
    sync_out.println("Cubes:");
    print_vector(mf_cubes.get());
}
```

In this example, we simulate complicated tasks by having each task wait for a bit before returning its result. We collect the futures of the tasks submitted within each group into two separate `BS::multi_future<double>` objects. `mf_squares` holds the results from the first group, and `mf_cubes` holds the results from the second group. Now we can wait for and/or get the results from `mf_squares` whenever is convenient, and separately wait for and/or get the results from `mf_cubes` at another time.

### Measuring execution time

If you are using a thread pool, then your code is most likely performance-critical. Achieving maximum performance requires performing a considerable amount of benchmarking to determine the optimal settings and algorithms. Therefore, it is important to be able to measure the execution time of various computations and operations under different conditions.

The helper class `BS::timer` provides a simple way to measure execution time. It is very straightforward to use:

1. Create a new `BS::timer` object.
2. Immediately before you execute the computation that you want to time, call the `start()` member function.
3. Immediately after the computation ends, call the `stop()` member function.
4. Use the member function `ms()` to obtain the elapsed time for the computation in milliseconds.

For example:

```cpp
BS::timer tmr;
tmr.start();
do_something();
tmr.stop();
std::cout << "The elapsed time was " << tmr.ms() << " ms.\n";
```

A practical application of the `BS::timer` class can be found in the benchmark portion of the test program `BS_thread_pool_test.cpp`.

## Other features

### Monitoring the tasks

Sometimes you may wish to monitor what is happening with the tasks you submitted to the pool. This may be done using three member functions:

* `get_tasks_queued()` gets the number of tasks currently waiting in the queue to be executed by the threads.
* `get_tasks_running()` gets the number of tasks currently being executed by the threads.
* `get_tasks_total()` gets the total number of unfinished tasks: either still in the queue, or running in a thread.
* Note that `get_tasks_total() == get_tasks_queued() + get_tasks_running()`.

These functions are demonstrated in the following program:

```cpp
#include "BS_thread_pool.hpp"

BS::synced_stream sync_out;
BS::thread_pool pool(4);

void sleep_half_second(const size_t i)
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
    for (size_t i = 0; i < 12; ++i)
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

### Pausing the workers

Sometimes you may wish to temporarily pause the execution of tasks, or perhaps you want to submit tasks to the queue in advance and only start executing them at a later time. You can do this using the member functions `pause()`, `unpause()`, and `is_paused()`.

When you call `pause()`, the workers will temporarily stop retrieving new tasks out of the queue. However, any tasks already executed will keep running until they are done, since the thread pool has no control over the internal code of your tasks. If you need to pause a task in the middle of its execution, you must do that manually by programming your own pause mechanism into the task itself. To resume retrieving tasks, call `unpause()`. To check whether the pool is currently paused, call `is_paused()`.

Here is an example:

```cpp
#include "BS_thread_pool.hpp"

BS::synced_stream sync_out;
BS::thread_pool pool(4);

void sleep_half_second(const size_t i)
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
    for (size_t i = 0; i < 8; ++i)
        pool.push_task(sleep_half_second, i);
    sync_out.println("Submitted 8 tasks.");
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    pool.pause();
    check_if_paused();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    sync_out.println("Still paused...");
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    for (size_t i = 8; i < 12; ++i)
        pool.push_task(sleep_half_second, i);
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

While the workers are paused, `wait_for_tasks()` will wait for the running tasks instead of all tasks (otherwise it would wait forever). This is demonstrated by the following program:

```cpp
#include "BS_thread_pool.hpp"

BS::synced_stream sync_out;
BS::thread_pool pool(4);

void sleep_half_second(const size_t i)
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
    for (size_t i = 0; i < 8; ++i)
        pool.push_task(sleep_half_second, i);
    sync_out.println("Submitted 8 tasks. Waiting for them to complete.");
    pool.wait_for_tasks();
    for (size_t i = 8; i < 20; ++i)
        pool.push_task(sleep_half_second, i);
    sync_out.println("Submitted 12 more tasks.");
    std::this_thread::sleep_for(std::chrono::milliseconds(250));
    pool.pause();
    check_if_paused();
    sync_out.println("Waiting for the ", pool.get_tasks_running(), " running tasks to complete.");
    pool.wait_for_tasks();
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

The first `wait_for_tasks()`, which was called while the pool was not paused, waited for all 8 tasks, both running and queued. The second `wait_for_tasks()`, which was called after pausing the pool, only waited for the 4 running tasks, while the other 8 tasks remained queued, and were not executed since the pool was paused. Finally, the third `wait_for_tasks()`, which was called after unpausing the pool, waited for the remaining 8 tasks, both running and queued.

**Warning:** If the thread pool is destroyed while paused, any tasks still in the queue will never be executed!

### Purging tasks

Consider a situation where the user cancels a multi-threaded operation while it is still ongoing. Perhaps the operation was split into multiple tasks, and half of the tasks are currently being executed by the pool's threads, but the other half are still waiting in the queue.

The thread pool cannot terminate the tasks that are already running, as the C++17 standard does not provide that functionality (and in any case, abruptly terminating a task while it's running could have extremely bad consequences, such as memory leaks and data corruption). However, the tasks that are still waiting in the queue can be purged using the `purge()` member function.

Once `purge()` is called, any tasks still waiting in the queue will be discarded, and will never be executed by the threads. Please note that there is no way to restore the purged tasks; they are gone forever.

Consider for example the following program:

```cpp
#include "BS_thread_pool.hpp"

BS::synced_stream sync_out;
BS::thread_pool pool(4);

int main()
{
    for (size_t i = 0; i < 8; ++i)
    {
        pool.push_task(
            [i]
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                sync_out.println("Task ", i, " done.");
            });
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    pool.purge();
    pool.wait_for_tasks();
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

`submit()` catches any exceptions thrown by the submitted task and forwards them to the corresponding future. They can then be caught when invoking the `get()` member function of the future. For example:

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
    std::future<double> my_future = pool.submit(inverse, num);
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
    std::future<void> my_future = pool.submit(print_inverse, num);
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

## Testing the package

### Automated tests

The file `BS_thread_pool_test.cpp` in the `tests` folder of the GitHub repository will perform automated tests of all aspects of the package. The output will be printed both to `std::cout` and to a file named `BS_thread_pool_test-yyyy-mm-dd_hh.mm.ss.log` based on the current date and time. In addition, the code is thoroughly documented, and is meant to serve as an extensive example of how to properly use the package.

Please make sure to:

1. [Compile](#compiling-and-compatibility) `BS_thread_pool_test.cpp` with optimization flags enabled (e.g. `-O3` on GCC / Clang or `/O2` on MSVC).
2. Run the test without any other applications, especially multithreaded applications, running in parallel.

A PowerShell script, `BS_thread_pool_test.ps1`, is provided for your convenience in the `tests` folder to make running the test on multiple compilers and operating systems easier. Since it is written in PowerShell, it is fully portable and works on Windows, Linux, and macOS. The script will automatically detect if Clang, GCC, and/or MSVC are available, compile the test program using each available compiler, and then run each compiled test program 5 times and report on any errors.

If any of the tests fail, please [submit a bug report](https://github.com/bshoshany/thread-pool/issues) including the exact specifications of your system (OS, CPU, compiler, etc.) and the generated log file.

### Performance tests

If all checks passed, `BS_thread_pool_test.cpp` performs simple benchmarks by filling a specific number of vectors of fixed size with values. The program decides how many vectors to use, and of what size, by testing how many are needed to reach a certain target duration in a single-threaded computation. This ensures that the test takes approximately the same amount of time on all systems, and is thus more consistent and portable.

Once the appropriate number and size of vectors has been determined, the program allocates the vectors and fills them with values, calculated according to a fixed prescription. This operation is performed both single-threaded and multithreaded, with the multithreaded computation spread across multiple tasks submitted to the pool.

Several different multithreaded tests are performed, with the number of tasks either equal to, smaller than, or larger than the pool's thread count. Each test is repeated multiple times, with the run times averaged over all runs of the same test. The run times of the tests are compared, and the maximum speedup obtained is calculated.

As an example, here are the results of the benchmarks from a [Digital Research Alliance of Canada](https://alliancecan.ca/en) node equipped with two 20-core / 40-thread Intel Xeon Gold 6148 CPUs (for a total of 40 cores and 80 threads), running CentOS Linux 7.9.2009. The tests were compiled using GCC v12.2.0 with the `-O3` and `-march=native` flags. The output was as follows:

```none
======================
Performing benchmarks:
======================
Using 80 threads.
Each test will be repeated 20 times to collect reliable statistics.
Determining the number and size of vectors to generate in order to achieve an approximate mean execution time of 50 ms with 80 tasks...
Generating 3840 vectors with 5120 elements each:
Single-threaded, mean execution time was 2122.5 ms with standard deviation 18.1 ms.
With   20 tasks, mean execution time was  119.8 ms with standard deviation 13.3 ms.
With   40 tasks, mean execution time was   68.9 ms with standard deviation  0.3 ms.
With   80 tasks, mean execution time was   45.5 ms with standard deviation  6.2 ms.
With  160 tasks, mean execution time was   39.3 ms with standard deviation  3.3 ms.
With  320 tasks, mean execution time was   40.4 ms with standard deviation  2.4 ms.
Maximum speedup obtained by multithreading vs. single-threading: 53.9x, using 160 tasks.

+++++++++++++++++++++++++++++++++++++++
Thread pool performance test completed!
+++++++++++++++++++++++++++++++++++++++
```

These two CPUs have 40 physical cores in total, with each core providing two separate logical cores via hyperthreading, for a total of 80 threads. Without hyperthreading, we would expect a maximum theoretical speedup of 40x. With hyperthreading, one might naively expect to achieve up to an 80x speedup, but this is in fact impossible, as each pair of hyperthreaded logical cores share the same physical core's resources. However, generally we would expect at most an estimated 30% additional speedup from hyperthreading, which amounts to around 52x in this case. The speedup of 53.9x in our performance test exceeds this estimate.

## The light version of the package

This package started out as a very lightweight C++ thread pool, but over time has expanded to include many additional features and helper classes. Therefore, I have decided to bundle a light version of the thread pool in a separate and stand-alone header file, `BS_thread_pool_light.hpp`.

This file does not contain any of the helper classes, only a new `BS::thread_pool_light` class, which is a minimal thread pool with only the 5 most basic member functions:

* `get_thread_count()`
* `push_loop()`
* `push_task()`
* `submit()`
* `wait_for_tasks()`

Note that each header file is 100% stand-alone. If you wish to use the full package, you only need `BS_thread_pool.hpp`, and if you wish to use the light version, you only need `BS_thread_pool_light.hpp`. Only a single header file needs to be included in your project. However, if you wish to use both the light and non-light thread pool classes in the same project, you can include both header files.

The test program `BS_thread_pool_test.cpp` tests both the full and the light versions of the package. If needed, the current version of the light thread pool can be obtained using the macro `BS_THREAD_POOL_LIGHT_VERSION`.

## About the project

### Issue and pull request policy

This package is under continuous and active development. If you encounter any bugs, or if you would like to request any additional features, please feel free to [open a new issue on GitHub](https://github.com/bshoshany/thread-pool/issues) and I will look into it as soon as I can.

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

Please note that the [companion paper on arXiv](https://arxiv.org/abs/2105.00613) is updated infrequently. The paper is intended to facilitate discovery of the package by scientists who may find it useful for scientific computing purposes and to allow citing the package in scientific research, but most users should read the `README.md` file on [the GitHub repository](https://github.com/bshoshany/thread-pool) instead, as it is guaranteed to always be up to date.

### Learning more about C++

Beginner C++ programmers may be interested in [my lecture notes](https://baraksh.com/CSE701/notes.php) for a course taught at McMaster University, which teach modern C and C++ from scratch, including some of the advanced techniques and programming practices used in developing this library.
