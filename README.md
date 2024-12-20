[![Author: Barak Shoshany](https://img.shields.io/badge/author-Barak_Shoshany-009933)](https://baraksh.com/)
[![DOI: 10.1016/j.softx.2024.101687](https://img.shields.io/badge/DOI-10.1016%2Fj.softx.2024.101687-b31b1b)](https://doi.org/10.1016/j.softx.2024.101687)
[![arXiv:2105.00613](https://img.shields.io/badge/arXiv-2105.00613-b31b1b)](https://arxiv.org/abs/2105.00613)
[![License: MIT](https://img.shields.io/github/license/bshoshany/thread-pool)](https://github.com/bshoshany/thread-pool/blob/master/LICENSE.txt)
[![Language: C++17 / C++20 / C++23](https://img.shields.io/badge/Language-C%2B%2B17%20%2F%20C%2B%2B20%20%2F%20C%2B%2B23-yellow)](https://cppreference.com/)
[![GitHub stars](https://img.shields.io/github/stars/bshoshany/thread-pool?style=flat&color=009999)](https://github.com/bshoshany/thread-pool/stargazers)
[![GitHub forks](https://img.shields.io/github/forks/bshoshany/thread-pool?style=flat&color=009999)](https://github.com/bshoshany/thread-pool/forks)
[![GitHub release](https://img.shields.io/github/v/release/bshoshany/thread-pool?color=660099)](https://github.com/bshoshany/thread-pool/releases)
[![Vcpkg version](https://img.shields.io/vcpkg/v/bshoshany-thread-pool?color=6600ff)](https://vcpkg.io/)
[![Meson WrapDB](https://img.shields.io/badge/dynamic/json?url=https%3A%2F%2Fraw.githubusercontent.com%2Fmesonbuild%2Fwrapdb%2Fmaster%2Freleases.json&query=%24%5B%22bshoshany-thread-pool%22%5D.versions%5B0%5D&label=wrapdb&color=6600ff)](https://mesonbuild.com/Wrapdb-projects.html)
[![Conan version](https://img.shields.io/conan/v/bshoshany-thread-pool?color=6600ff)](https://conan.io/center/recipes/bshoshany-thread-pool)
[![Open in Visual Studio Code](https://img.shields.io/badge/Open_in_Visual_Studio_Code-007acc)](https://vscode.dev/github/bshoshany/thread-pool)

# `BS::thread_pool`: a fast, lightweight, modern, and easy-to-use C&plus;&plus;17 / C&plus;&plus;20 / C&plus;&plus;23 thread pool library

By **Barak Shoshany**\
Email: <baraksh@gmail.com>\
Website: <https://baraksh.com/>\
GitHub: <https://github.com/bshoshany>

This is the complete documentation for **v5.0.0** of the library, released on **2024-12-19**.

* [Introduction](#introduction)
    * [Motivation](#motivation)
    * [Overview of features](#overview-of-features)
* [Getting started](#getting-started)
    * [Installing the library](#installing-the-library)
    * [Compiling and compatibility](#compiling-and-compatibility)
    * [Constructors](#constructors)
    * [Getting and resetting the number of threads in the pool](#getting-and-resetting-the-number-of-threads-in-the-pool)
* [Submitting tasks to the queue](#submitting-tasks-to-the-queue)
    * [Submitting tasks with no arguments and receiving a future](#submitting-tasks-with-no-arguments-and-receiving-a-future)
    * [Submitting tasks with arguments and receiving a future](#submitting-tasks-with-arguments-and-receiving-a-future)
    * [Detaching and waiting for tasks](#detaching-and-waiting-for-tasks)
    * [Waiting for submitted or detached tasks with a timeout](#waiting-for-submitted-or-detached-tasks-with-a-timeout)
    * [Class member functions as tasks](#class-member-functions-as-tasks)
* [Parallelizing loops](#parallelizing-loops)
    * [Automatic parallelization of loops](#automatic-parallelization-of-loops)
    * [Optimizing the number of blocks](#optimizing-the-number-of-blocks)
    * [Common index types](#common-index-types)
    * [Parallelizing loops without futures](#parallelizing-loops-without-futures)
    * [Parallelizing individual indices vs. blocks](#parallelizing-individual-indices-vs-blocks)
    * [Loops with return values](#loops-with-return-values)
    * [Parallelizing sequences](#parallelizing-sequences)
    * [More about `BS::multi_future`](#more-about-bsmulti_future)
* [Utility classes](#utility-classes)
    * [Synchronizing printing to a stream with `BS::synced_stream`](#synchronizing-printing-to-a-stream-with-bssynced_stream)
    * [Synchronizing tasks with `BS::counting_semaphore` and `BS::binary_semaphore`](#synchronizing-tasks-with-bscounting_semaphore-and-bsbinary_semaphore)
* [Managing tasks](#managing-tasks)
    * [Monitoring the tasks](#monitoring-the-tasks)
    * [Purging tasks](#purging-tasks)
    * [Exception handling](#exception-handling)
    * [Getting information about the current thread](#getting-information-about-the-current-thread)
    * [Thread initialization functions](#thread-initialization-functions)
    * [Thread cleanup functions](#thread-cleanup-functions)
    * [Passing task arguments by constant reference](#passing-task-arguments-by-constant-reference)
* [Optional features](#optional-features)
    * [Enabling features](#enabling-features)
    * [Setting task priority](#setting-task-priority)
    * [Pausing the pool](#pausing-the-pool)
    * [Avoiding wait deadlocks](#avoiding-wait-deadlocks)
* [Native extensions](#native-extensions)
    * [Enabling the native extensions](#enabling-the-native-extensions)
    * [Setting thread priority](#setting-thread-priority)
    * [Setting thread affinity](#setting-thread-affinity)
    * [Setting thread names](#setting-thread-names)
    * [Setting process priority](#setting-process-priority)
    * [Setting process affinity](#setting-process-affinity)
    * [Accessing native thread handles](#accessing-native-thread-handles)
* [Testing the library](#testing-the-library)
    * [Automated tests](#automated-tests)
    * [Performance tests](#performance-tests)
    * [Finding the version of the library](#finding-the-version-of-the-library)
* [Importing the library as a C++20 module](#importing-the-library-as-a-c20-module)
    * [Compiling the module](#compiling-the-module)
    * [Compiling with `compile_cpp.py` using `import BS.thread_pool`](#compiling-with-compile_cpppy-using-import-bsthread_pool)
    * [Compiling with Clang using `import BS.thread_pool`](#compiling-with-clang-using-import-bsthread_pool)
    * [Compiling with GCC using `import BS.thread_pool`](#compiling-with-gcc-using-import-bsthread_pool)
    * [Compiling with MSVC using `import BS.thread_pool`](#compiling-with-msvc-using-import-bsthread_pool)
    * [Compiling with CMake using `import BS.thread_pool`](#compiling-with-cmake-using-import-bsthread_pool)
* [Importing the C++23 Standard Library as a module](#importing-the-c23-standard-library-as-a-module)
    * [Enabling `import std`](#enabling-import-std)
    * [Compiling with `compile_cpp.py` using `import std`](#compiling-with-compile_cpppy-using-import-std)
    * [Compiling with Clang and LLVM libc++ using `import std`](#compiling-with-clang-and-llvm-libc-using-import-std)
    * [Compiling with MSVC and Microsoft STL using `import std`](#compiling-with-msvc-and-microsoft-stl-using-import-std)
    * [Compiling with CMake using `import std`](#compiling-with-cmake-using-import-std)
* [Installing the library using package managers](#installing-the-library-using-package-managers)
    * [Installing using vcpkg](#installing-using-vcpkg)
    * [Installing using Conan](#installing-using-conan)
    * [Installing using Meson](#installing-using-meson)
    * [Installing using CMake with CPM](#installing-using-cmake-with-cpm)
    * [Installing using CMake with `FetchContent`](#installing-using-cmake-with-fetchcontent)
* [Complete library reference](#complete-library-reference)
    * [The `BS::thread_pool` class template](#the-bsthread_pool-class-template)
    * [Optional features and the template parameter](#optional-features-and-the-template-parameter)
    * [The `BS::this_thread` class](#the-bsthis_thread-class)
    * [The native extensions](#the-native-extensions)
    * [The `BS::multi_future` class](#the-bsmulti_future-class)
    * [The `BS::synced_stream` class](#the-bssynced_stream-class)
    * [The `BS::version` class](#the-bsversion-class)
    * [Diagnostic variables](#diagnostic-variables)
    * [All names exported by the C++20 module](#all-names-exported-by-the-c20-module)
* [Development tools](#development-tools)
    * [The `compile_cpp.py` script](#the-compile_cpppy-script)
    * [Other included tools](#other-included-tools)
* [About the project](#about-the-project)
    * [Bug reports and feature requests](#bug-reports-and-feature-requests)
    * [Contribution and pull request policy](#contribution-and-pull-request-policy)
    * [Starring the repository](#starring-the-repository)
    * [Acknowledgements](#acknowledgements)
    * [Copyright and citing](#copyright-and-citing)
    * [About the author](#about-the-author)
    * [Learning more about C++](#learning-more-about-c)
    * [Other projects to check out](#other-projects-to-check-out)

## Introduction

### Motivation

Multithreading is essential for modern high-performance computing. Since C&plus;&plus;11, the C&plus;&plus; standard library has included built-in low-level multithreading support using constructs such as `std::thread`. However, `std::thread` creates a new thread each time it is called, which can have a significant performance overhead. Furthermore, it is possible to create more threads than the hardware can handle simultaneously, potentially resulting in a substantial slowdown.

The library presented here contains a C&plus;&plus; thread pool class, `BS::thread_pool`, which avoids these issues by creating a fixed pool of threads once and for all, and then continuously reusing the same threads to perform different tasks throughout the lifetime of the program. By default, the number of threads in the pool is equal to the maximum number of threads that the hardware can run in parallel.

The user submits tasks to be executed into a queue. Whenever a thread becomes available, it retrieves the next task from the queue and executes it. The pool optionally produces an `std::future` for each task, which allows the user to wait for the task to finish executing and/or obtain its eventual return value, if applicable. Threads and tasks are autonomously managed by the pool in the background, without requiring any input from the user aside from submitting the desired tasks.

The design of this library is guided by four important principles. First, *compactness*: the entire library consists of just one self-contained header file, with no other components or dependencies. Second, *portability*: the library only utilizes the C&plus;&plus; standard library, without relying on any compiler extensions or 3rd-party libraries, and is therefore compatible with any modern standards-conforming C&plus;&plus; compiler on any platform, as long as it supports C&plus;&plus;17 or later. Third, *ease of use*: the library is extensively documented, and programmers of any level should be able to use it right out of the box.

The fourth and final guiding principle is *performance*: each and every line of code in this library was carefully designed with maximum performance in mind, and performance was tested and verified on a variety of compilers and platforms. Indeed, the library was originally designed for use in the author's own computationally-intensive scientific computing projects, running both on high-end desktop/laptop computers and high-performance computing nodes.

Among the available C&plus;&plus; thread pool libraries, `BS::thread_pool` occupies the crucial middle ground between small bare-bones thread pool classes that offer rudimentary functionality and are only suitable for simple programs, and very large libraries that offer many advanced features but consist of multiple components and dependencies and involve complex APIs that require a substantial time investment to learn. `BS::thread_pool` was designed for users who want a simple and lightweight header-only library that is easy to learn and use, and can be readily incorporated into existing or new projects, but do not want to compromise on performance or functionality.

Obtaining the library is quick and easy; it can be downloaded manually from [the GitHub repository](https://github.com/bshoshany/thread-pool), or installed automatically using a variety of package managers and build systems. The library can be imported either as a traditional [header-only library](#installing-the-library), or as a modern [C&plus;&plus;20 module](#importing-the-library-as-a-c20-module). `BS::thread_pool` has undergone extensive testing on multiple platforms and is actively used by thousands of C&plus;&plus; developers worldwide for a wide range of applications, from scientific computing to game development.

### Overview of features

* **Fast:**
    * Built from scratch with [maximum performance](#performance-tests) in mind.
    * Suitable for use in high-performance computing nodes with a very large number of CPU cores.
    * Reusing threads avoids the overhead of creating and destroying them for individual tasks.
    * A task queue ensures that there are never more tasks running in parallel than is allowed by the hardware.
    * All optional features can be selectively turned on to ensure minimal overhead.
* **Lightweight:**
    * Single header file: simply [`#include "BS_thread_pool.hpp"`](#installing-the-library) and you're all set!
    * Header-only: no need to install or build the library.
    * Self-contained: no external requirements or dependencies.
    * Portable: uses only the C&plus;&plus; standard library, and works with any C&plus;&plus;17-compliant compiler on any platform.
    * Only 487 lines of code, including all optional features and utility classes (excluding comments, blank lines, lines containing only a single brace, C&plus;&plus;17 polyfills, and native extensions).
* **Modern:**
    * Fully supports C&plus;&plus;17, C&plus;&plus;20, and C&plus;&plus;23, taking advantage of the latest language features when available for maximum performance, reliability, and usability.
    * In C&plus;&plus;20, the library can an be imported as a C&plus;&plus;20 module using [`import BS.thread_pool`](#importing-the-library-as-a-c20-module), with many benefits, such as faster compilation times and avoiding namespace pollution.
    * In C&plus;&plus;23, the library can import the C&plus;&plus; Standard Library as a module using [`import std`](#importing-the-c23-standard-library-as-a-module) on supported compilers and platforms.
    * Makes use of modern C&plus;&plus; programming practices for readability, maintainability, performance, safety, portability, and reliability.
* **Easy to use:**
    * Very simple operation, using only a handful of member functions for basic use, with many additional member functions, classes, and functions for more advanced use.
    * Every task submitted to the queue using [`submit_task()`](#submitting-tasks-to-the-queue) automatically generates an `std::future`, which can be used to wait for the task to finish executing, obtain its eventual return value, and/or catch any thrown exceptions.
    * Loops can be automatically parallelized into any number of tasks using [`submit_loop()`](#parallelizing-loops), which returns a [`BS::multi_future`](#more-about-bsmulti_future) that can be used to track the execution of all parallel tasks at once.
    * If futures are not needed, tasks may be submitted using [`detach_task()`](#detaching-and-waiting-for-tasks), and loops can be parallelized using [`detach_loop()`](#parallelizing-loops-without-futures) - sacrificing convenience for even greater performance. In that case, `wait()`, `wait_for()`, and `wait_until()` can be used to wait for all the tasks in the queue to complete.
    * Extremely thorough and detailed documentation, with numerous examples, is available in the library's [`README.md` file](https://github.com/bshoshany/thread-pool/blob/master/README.md), with a total of 3,359 lines and 25,506 words!
    * The code is thoroughly documented using Doxygen comments - not only the interface, but also the implementation, in case the user would like to make modifications.
    * Optionally, the included Python script [`compile_cpp.py`](#the-compile_cpppy-script) can be used to easily compile any programs that are using the library, with full support for C&plus;&plus;20 modules and C&plus;&plus;23 Standard Library modules where applicable.
* **Additional features:**
    * Get the current thread count of the pool using [`get_thread_count()`](#getting-and-resetting-the-number-of-threads-in-the-pool).
    * Change the number of threads in the pool safely and on-the-fly as needed using [`reset()`](#getting-and-resetting-the-number-of-threads-in-the-pool).
    * Monitor the number of queued and/or running tasks using [`get_tasks_queued()`, `get_tasks_running()`, and `get_tasks_total()`](#monitoring-the-tasks).
    * Purge all tasks currently waiting in the queue with [`purge()`](#purging-tasks).
    * Run an [initialization function](#thread-initialization-functions) in each thread before it starts to execute any submitted tasks, by passing it to the `BS::thread_pool` constructor.
    * Run a cleanup function in each thread right before it is destroyed, using [`set_cleanup_func()`](#thread-cleanup-functions).
    * Assume lower-level control of parallelized loops using [`detach_blocks()` and `submit_blocks()`](#parallelizing-individual-indices-vs-blocks).
    * Parallelize a sequence of tasks enumerated by indices to the queue using [`detach_sequence()` and `submit_sequence()`](#parallelizing-sequences).
    * Get [information about the current thread](#getting-information-about-the-current-thread): the pool index using `BS::this_thread::get_index()` and a pointer to the owning pool using `BS::this_thread::get_pool()`.
    * Get the unique thread IDs for all threads in the pool using [`get_thread_ids()`](#getting-and-resetting-the-number-of-threads-in-the-pool).
    * Synchronize output to one or more streams from multiple threads in parallel using the [`BS::synced_stream`](#synchronizing-printing-to-a-stream-with-bssynced_stream) utility class.
    * Access C&plus;&plus;20 semaphores in C&plus;&plus;17 using the [`BS::binary_semaphore` and `BS::counting_semaphore`](#synchronizing-tasks-with-bscounting_semaphore-and-bsbinary_semaphore) polyfill classes.
* **Optional features:**
    * [Optional features](#enabling-features) can be enabled by passing a bitmask template parameter to the `BS::thread_pool` class template.
    * Assign a priority to each task using the optional [task priority](#setting-task-priority) feature. The priority, in the range -128 to +127, is passed as the last argument to all `submit` and `detach` member functions. Tasks with higher priorities will be executed first.
    * Freely pause and resume the pool using `pause()`, `unpause()`, and `is_paused()` with the optional [pausing](#pausing-the-pool) feature. When paused, threads do not retrieve new tasks out of the queue.
    * Avoid deadlocks using the optional [wait deadlock checks](#avoiding-wait-deadlocks) feature. If a deadlock is detected while waiting for tasks, the pool will throw the exception `BS::wait_deadlock`.
* **Native extensions:**
    * The library includes optional [native extensions](#native-extensions), which contain non-portable features using the operating system's native API, enabled by defining the macro `BS_THREAD_POOL_NATIVE_EXTENSIONS` at compilation time. This feature should work on most Windows, Linux, and macOS systems.
    * Use [`BS::this_thread::get_os_thread_priority()` and `BS::this_thread::set_os_thread_priority()`](#setting-thread-priority) to get and set the priority of the current thread.
    * Use [`BS::this_thread::get_os_thread_affinity()` and `BS::this_thread::set_os_thread_affinity()`](#setting-thread-affinity) to get and set the processor affinity of the current thread.
    * Use [`BS::this_thread::get_os_thread_name()` and `BS::this_thread::set_os_thread_name()`](#setting-thread-names) to get and set the name of the current thread.
    * Use [`BS::get_os_process_priority()` and `BS::set_os_process_priority()`](#setting-process-priority) to get and set the priority of the current process.
    * Use [`BS::get_os_process_affinity()` and `BS::set_os_process_affinity()`](#setting-process-affinity) to get and set the processor affinity of the current process.
    * Get the implementation-defined thread handles for all threads in the pool using [`get_native_handles()`](#accessing-native-thread-handles).
* **Well-tested:**
    * The included test program [`BS_thread_pool_test.cpp`](#automated-tests) performs hundreds of automated tests, and also serves as a comprehensive example of how to properly use the library.
    * The test program also performs [benchmarks](#performance-tests) using a highly-optimized multithreaded algorithm which generates a plot of the Mandelbrot set.
    * The included Python script `test_all.py` provides a portable way to easily run the tests with multiple compilers.
    * [Compatibility](#compiling-and-compatibility) is comprehensively tested on the latest versions of Windows, Ubuntu, and macOS, using Clang, GCC, and MSVC.
    * Under continuous and active development. Bug reports and feature requests are welcome, and should be made via [GitHub issues](https://github.com/bshoshany/thread-pool/issues).

## Getting started

### Installing the library

To install `BS::thread_pool`, simply download the [latest release](https://github.com/bshoshany/thread-pool/releases) from [the GitHub repository](https://github.com/bshoshany/thread-pool), place the header file `BS_thread_pool.hpp` from the `include` folder in the desired folder, and include it in your program:

```cpp
#include "BS_thread_pool.hpp"
```

The thread pool will now be accessible via the `BS::thread_pool` class. For an even quicker installation, you can download the header file itself directly [at this URL](https://raw.githubusercontent.com/bshoshany/thread-pool/master/include/BS_thread_pool.hpp); no additional files are required, as the library is a single-header library.

This library is also available on various package managers and build system, including [vcpkg](https://vcpkg.io/), [Conan](https://conan.io/), [Meson](https://mesonbuild.com/), and [CMake](https://cmake.org/). Please [see below](#installing-the-library-using-package-managers) for more details.

If C&plus;&plus;20 features are available, the library can also be imported as a C&plus;&plus;20 module, in which case `#include "BS_thread_pool.hpp"` should be replaced with `import BS.thread_pool;`. This requires one additional file, and the module must be compiled before it can be used; please see detailed instructions [below](#importing-the-library-as-a-c20-module).

### Compiling and compatibility

This library officially supports C&plus;&plus;17, C&plus;&plus;20, and C&plus;&plus;23. If compiled with C&plus;&plus;20 and/or C&plus;&plus;23 support, the library will make use of newly available features for maximum performance and usability. However, the library is fully compatible with C&plus;&plus;17, and should successfully compile on any C&plus;&plus;17 standard-compliant compiler, on all operating systems and architectures for which such a compiler is available.

Compatibility was verified using the bundled test program `BS_thread_pool_test.cpp`, compiled using the bundled Python scripts `test_all.py` and `compile_cpp.py` with native extensions enabled, importing the library [as a C&plus;&plus;20 module](#importing-the-library-as-a-c20-module) where applicable, and importing the [C&plus;&plus;23 Standard Library as a module](#importing-the-c23-standard-library-as-a-module) where applicable, on a 24-core (8P+16E) / 32-thread Intel i9-13900K CPU, using the following compilers, C&plus;&plus; standard libraries, and platforms:

* Windows 11 23H2 build 22631.4602:
    * [Clang](https://clang.llvm.org/) v19.1.4 with LLVM libc&plus;&plus; v19.1.4 ([MSYS2 build](https://www.msys2.org/))
    * [GCC](https://gcc.gnu.org/) v14.2.0 with GNU libstdc&plus;&plus; v14 (20240801) ([MSYS2 build](https://www.msys2.org/))
    * [MSVC](https://docs.microsoft.com/en-us/cpp/) v19.42.34435 with Microsoft STL v143 (202408).
* Ubuntu 24.10:
    * [Clang](https://clang.llvm.org/) v19.1.6 with LLVM libc&plus;&plus; v19.1.6
    * [GCC](https://gcc.gnu.org/) v14.2.0 with GNU libstdc&plus;&plus; v14 (20240908)
* macOS 15.1 build 24B83:
    * [Clang](https://clang.llvm.org/) v19.1.6 with LLVM libc&plus;&plus; v19.1.6 ([Homebrew build](https://formulae.brew.sh/formula/llvm))
    * Note: Apple Clang is currently not officially supported, as it does not support C&plus;&plus;20 modules.

As this library requires C&plus;&plus;17 features, the code must be compiled with C&plus;&plus;17 support:

* For Clang or GCC, use the `-std=c++17` flag. On Linux, you will also need to use the `-pthread` flag to enable the POSIX threads library.
* For MSVC, use `/std:c++17`, and also `/permissive-` to ensure standards conformance.

For maximum performance, it is recommended to compile with all available compiler optimizations:

* For Clang or GCC, use the `-O3` flag.
* For MSVC, use `/O2`.

As an example, to compile the test program `BS_thread_pool_test.cpp` with compiler optimizations, it is recommended to use the following commands:

* Windows:
    * GCC: `g++ BS_thread_pool_test.cpp -std=c++17 -O3 -o BS_thread_pool_test.exe`
    * Clang: `clang++ BS_thread_pool_test.cpp -std=c++17 -O3 -o BS_thread_pool_test.exe`
    * MSVC: `cl BS_thread_pool_test.cpp /std:c++17 /permissive- /O2 /EHsc /Fo:BS_thread_pool_test.obj /Fe:BS_thread_pool_test.exe`
* Linux/macOS:
    * GCC: `g++ BS_thread_pool_test.cpp -std=c++17 -O3 -pthread -o BS_thread_pool_test`
    * Clang: `clang++ BS_thread_pool_test.cpp -std=c++17 -O3 -pthread -o BS_thread_pool_test`

If your compiler and codebase support C&plus;&plus;20 and/or C&plus;&plus;23, it is recommended to enable them in order to allow the library access to the latest features:

* For Clang or GCC, use the `-std=c++20` or `-std=c++23` flag.
* For MSVC, use `/std:c++20` for C&plus;&plus;20 or `/std:c++latest` for C&plus;&plus;23.

In addition, if C&plus;&plus;20 features are available, the library can be imported as a module; instructions for doing so are provided [below](#importing-the-library-as-a-c20-module).

### Constructors

The default constructor creates a thread pool with as many threads as the hardware can handle concurrently, as reported by the implementation via `std::thread::hardware_concurrency()`. This is usually determined by the number of cores in the CPU. If a core is hyperthreaded, it will count as two threads. For example:

```cpp
// Constructs a thread pool with as many threads as are available in the hardware.
BS::thread_pool pool;
```

Optionally, a number of threads different from the hardware concurrency can be specified as an argument to the constructor. However, note that adding more threads than the hardware can handle will **not** improve performance, and in fact will most likely hinder it. This option exists in order to allow using **fewer** threads than the hardware concurrency, in cases where you wish to leave some threads available for other processes. For example:

```cpp
// Constructs a thread pool with only 12 threads.
BS::thread_pool pool(12);
```

Usually, when the thread pool is used, a program's main thread should only submit tasks to the thread pool and wait for them to finish, and should not perform any computationally intensive tasks on its own. If this is the case, it is recommended to use the default value for the number of threads. This ensures that all the threads available in the hardware will be put to work while the main thread waits.

However, if the main thread also performs computationally intensive tasks, it may be beneficial to use one fewer thread than the hardware concurrency, leaving one hardware thread available for the main thread. Furthermore, if more than one thread pool is used in the program simultaneously, the total number of thread across all pools should not exceed the hardware concurrency.

### Getting and resetting the number of threads in the pool

The member function `get_thread_count()` returns the number of threads in the pool. This will be equal to `std::thread::hardware_concurrency()` if the default constructor was used.

It is generally unnecessary to change the number of threads in the pool after it has been created, since the whole point of a thread pool is that you only create the threads once. However, if needed, this can be done, safely and on-the-fly, using the `reset()` member function.

`reset()` will wait for all currently running tasks to be completed, but will leave the rest of the tasks in the queue. Then it will destroy the thread pool and create a new one with the desired new number of threads, as specified in the function's argument (or the hardware concurrency if no argument is given). The new thread pool will then resume executing the tasks that remained in the queue and any newly submitted tasks.

The member function `get_thread_ids()` returns a vector containing the unique identifiers for each of the pool's threads, as obtained by `std::thread::get_id()`. These values are not so useful on their own, but can be used to identify and distinguish between threads, or for allocating resources.

## Submitting tasks to the queue

### Submitting tasks with no arguments and receiving a future

In this section we will learn how to submit a task with no arguments, but potentially with a return value, to the queue. Once a task has been submitted, it will be executed as soon as a thread becomes available. Tasks are executed in the order that they were submitted (first-in, first-out), unless task priority is enabled ([see below](#setting-task-priority)).

For example, if the pool has 8 threads and an empty queue, and we submitted 16 tasks, then we should expect the first 8 tasks to be executed in parallel, with the remaining tasks being picked up by the threads one by one as each thread finishes executing its first task, until no tasks are left in the queue.

The member function `submit_task()` is used to submit tasks to the queue. It takes exactly one input, the task to submit. This task must be a function with no arguments, but it can have a return value.

`submit_task()` returns an `std::future` associated to the task. If the submitted task has a return value of type `T`, then the future will be of type `std::future<T>`, and will be set to the task's return value when the task finishes its execution. If the submitted task does not have a return value, then the future will be an `std::future<void>`, which will not contain any value, but may still be used to wait for the task to finish.

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

In addition to submitting a pre-defined function, we can also use a [lambda expression](https://en.cppreference.com/w/cpp/language/lambda) to quickly define the task on-the-fly. Rewriting the previous example in terms of a lambda expression, we get:

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

As stated in the previous section, tasks submitted using `submit_task()` cannot have any arguments. However, it is easy to submit tasks with argument either by wrapping the function in a lambda or using lambda captures directly. The following is an example of submitting a pre-defined function with arguments by wrapping it in a lambda:

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

As you can see, to pass the arguments to `multiply()` we simply called `multiply(6, 7)` explicitly inside a lambda. If the arguments are not literals, we can use the lambda capture clause to capture the arguments from the local scope:

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

We could even get rid of the `multiply()` function entirely and just put everything inside a lambda, if desired:

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

In such cases, you may wish to avoid the overhead involved in assigning a future to the task, in order to increase performance. This is called "detaching" the task, as the task detaches from the main thread and runs independently.

Detaching tasks is done using the `detach_task()` member function, which allows you to detach a task to the queue without generating a future for it. As with `submit_task()`, the task must have no arguments, but you can pass arguments by wrapping it in a lambda, as shown in the previous section. However, tasks executed via `detach_task()` cannot have a return value, as there would be no way for the main thread to retrieve that value.

Since `detach_task()` does not return a future, there is no built-in way for the user to know when the task finishes executing. You must manually ensure that the task finishes executing before trying to use anything that depends on its output. Otherwise, bad things will happen!

`BS::thread_pool` provides the member function `wait()` to facilitate waiting for all the tasks in the queue to complete, whether they were detached or submitted with a future. The `wait()` member function works similarly to the `wait()` member function of `std::future`. Consider, for example, the following code:

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

This program first defines a local variable named `result` and initializes it to `0`. It then detaches a task in the form of a lambda expression. Note that the lambda captures `result` **by reference**, as indicated by the `&` in front of it. This means that the task can modify `result`, and any such modification will be reflected in the main thread.

The task changes `result` to `42`, but it first sleeps for 100 milliseconds. When the main thread prints out the value of `result`, the task has not yet had time to modify its value, since it is still sleeping. Therefore, the program will actually print out the initial value `0`, which is not what we want.

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

Now the program will print out the value `42`, as expected. Note, however, that `wait()` will wait for **all** the tasks in the queue, including any other tasks that were potentially submitted before or after the one we care about. If we want to wait just for **one** task, `submit_task()` would be a better choice.

### Waiting for submitted or detached tasks with a timeout

Sometimes you may wish to wait for the tasks to complete, but only for a certain amount of time, or until a specific point in time. For example, if the tasks have not yet completed after some time, you may wish to let the user know that there is a delay.

For tasks submitted with futures using `submit_task()`, this can be achieved using two member functions of `std::future`:

* `wait_for()` waits for the task to be completed, but stops waiting after the specified duration, given as an argument of type `std::chrono::duration`, has passed.
* `wait_until()` waits for the task to be completed, but stops waiting after the specified time point, given as an argument of type `std::chrono::time_point`, has been reached.

In both cases, the functions will return `std::future_status::ready` if the future is ready, meaning the task is finished and its return value, if any, has been obtained. However, they will return `std::future_status::timeout` if the future is not yet ready when the timeout has expired.

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

For detached tasks, since we do not have futures for them, we cannot use this method. However, `BS::thread_pool` has two member functions, also named `wait_for()` and `wait_until()`, which similarly wait for a specified duration or until a specified time point, but do so for **all** tasks (whether submitted or detached). Instead of an `std::future_status`, the thread pool's wait functions returns `true` if all tasks finished running, or `false` if the duration expired or the time point was reached but some tasks are still running.

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
#include <iostream> // std::boolalpha, std::cout

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

What if we want to submit the member function `set_flag()` as a task to the thread pool? We can simply wrap the entire statement `flag_object.set_flag(true);` in a lambda, and pass `flag_object` to the lambda by reference, as in the following example:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <iostream>           // std::boolalpha, std::cout

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

Note that in this example, instead of getting a future from `submit_task()` and then waiting for that future, we simply called `wait()` on that future straight away. This is a common way of waiting for a task to complete if we have nothing else to do in the meantime. Note also that we passed `flag_object` by reference to the lambda, since we want to set the flag on that same object, not a copy of it.

Another thing you might want to do is call a member function from within the object itself, that is, from another member function. This follows a similar syntax, except that you must also capture `this` (i.e. a pointer to the current object) in the lambda. Here is an example:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <iostream>           // std::boolalpha, std::cout

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

Note that in this example we defined the thread pool as a global object, so that it is accessible outside the `main()` function. Although we could have, in theory, passed a reference to the thread pool in our call to `set_flag_to_true()`, that would be very cumbersome to do if multiple different functions need to use the same thread pool. Defining the thread pool as a global object is common practice, as it allows all functions to access the same thread pool without having to pass it around as an argument.

## Parallelizing loops

### Automatic parallelization of loops

One of the most common and effective methods of parallelization is splitting a loop into smaller sub-loops and running them in parallel. It is most effective in "embarrassingly parallel" computations, such as vector or matrix operations, where each iteration of the loop is completely independent of every other iteration.

For example, if we are summing up two vectors of 1000 elements each, and we have 10 threads, we could split the summation into 10 blocks of 100 elements each, and run all the blocks in parallel, potentially increasing performance by up to a factor of 10.

`BS::thread_pool` can automatically parallelize loops, making it very easy to implement many parallel algorithms without having to worry about the details. To see how this works, consider the following generic loop:

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
* `end` is the index after the last index in the range, such that the full range is `[start, end)`. In other words, the loop will be equivalent to the generic loop above, but parallelized. Note that if `end <= start`, nothing will happen; the loop cannot go backwards.
* `loop()` is the function that should run in every iteration of the loop. It must take exactly one argument, the loop index. It cannot have a return value, as it will be executed multiple times by each task, so a return value would not make sense.
* `num_blocks` is the number of blocks of the form `[a, b)` to split the loop into. For example, if the range is `[0, 9)` and there are 3 blocks, then the blocks will be the ranges `[0, 3)`, `[3, 6)`, and `[6, 9)`. This argument can be omitted, in which case the number of blocks will be the number of threads in the pool.

The thread pool's internal algorithm ensures that each of the blocks has one of two sizes, differing by 1, with the larger blocks always first, so that the tasks are as evenly distributed as possible, to optimize performance. For example, if the range `[0, 100)` is split into 15 blocks, the result will be 10 blocks of size 7, which will be submitted first, and 5 blocks of size 6.

Each block will be submitted to the thread pool's queue as a separate task. Therefore, a loop that is split into 3 blocks will be split into 3 individual tasks, which may run in parallel. If there is only one block, then the entire loop will run as one task, and no parallelization will take place.

To parallelize the generic loop above, we use the following commands:

```cpp
BS::multi_future<void> loop_future = pool.submit_loop(start, end, loop, num_blocks);
loop_future.wait();
```

`submit_loop()` returns an object of the helper class [`BS::multi_future<T>`](#more-about-bsmulti_future). This is essentially a specialization of `std::vector<std::future<T>>` with additional member functions. Each of the `num_blocks` blocks will have an `std::future<T>` assigned to it, and all these futures will be stored inside the returned `BS::multi_future<T>`. When `loop_future.wait()` is called, the main thread will wait until **all** tasks generated by `submit_loop()` finish executing, and **only** those tasks - not any other tasks that also happen to be in the queue. This is essentially the role of the `BS::multi_future<T>` class: to wait for a specific **group of tasks**, in this case the tasks running the loop blocks.

As a simple example, the following code calculates and prints a table of squares of all integers from 0 to 99:

```cpp
#include <cstddef>  // std::size_t
#include <iomanip>  // std::setw
#include <iostream> // std::cout

int main()
{
    constexpr std::size_t max = 100;
    std::size_t squares[max];
    for (std::size_t i = 0; i < max; ++i)
        squares[i] = i * i;
    for (std::size_t i = 0; i < max; ++i)
        std::cout << std::setw(2) << i << "^2 = " << std::setw(4) << squares[i] << ((i % 5 != 4) ? " | " : "\n");
}
```

We can parallelize it as follows:

```cpp
#include "BS_thread_pool.hpp" // BS::multi_future, BS::thread_pool
#include <cstddef>            // std::size_t
#include <iomanip>            // std::setw
#include <iostream>           // std::cout

int main()
{
    BS::thread_pool pool(10);
    constexpr std::size_t max = 100;
    std::size_t squares[max];
    const BS::multi_future<void> loop_future = pool.submit_loop(0, max,
        [&squares](const std::size_t i)
        {
            squares[i] = i * i;
        });
    loop_future.wait();
    for (std::size_t i = 0; i < max; ++i)
        std::cout << std::setw(2) << i << "^2 = " << std::setw(4) << squares[i] << ((i % 5 != 4) ? " | " : "\n");
}
```

Since there are 10 threads, and we omitted the `num_blocks` argument, the loop will be divided into 10 blocks, each calculating 10 squares.

As a side note, notice that here we parallelized the calculation of the squares, but we did not parallelize printing the results. This is for two reasons:

1. We want to print out the squares in ascending order, and we have no guarantee that the blocks will be executed in the correct order. This is very important; you must never expect that the parallelized loop will execute at the same order as the non-parallelized loop.
2. If we did print out the squares from within the parallel tasks, we would get a huge mess, since all 10 blocks would print to the standard output at once. [Later](#synchronizing-printing-to-a-stream-with-bssynced_stream) we will see how to synchronize printing to a stream from multiple tasks at the same time.

### Optimizing the number of blocks

The most important factor to consider when parallelizing loops is the number of blocks `num_blocks` to split the loop into. Naively, it may seem that the number of blocks should simply be equal to the number of threads in the pool, but that is usually **not** the optimal choice. Inevitably, some blocks will finish before other blocks; if there is only one block per thread, then any threads that have already finished executing their blocks will remain idle until the rest of the blocks are done, wasting many CPU cycles.

It is therefore generally better to use a larger number of blocks than the number of threads, to ensure that all threads work at maximum capacity. On the other hand, parallelization with too many blocks will eventually suffer from diminishing returns due to increased overhead. A good rule of thumb is to use a number of blocks equal to the square of the number of threads, but this is not necessarily the optimal number in all cases.

In the end, the optimal number of blocks will always depend on the specific algorithm being parallelized and the total number of indices in the loop, and may differ between different compilers, operating systems, and hardware configurations. For best performance, it is strongly recommended to do your own benchmarks to find the optimal number of blocks for your particular use case; see the [benchmarks code in the bundled test program](#performance-tests) for an example of how to do this.

Finally, note that the discussion here only pertains to situations where the parallelized loop is the only thing running in the pool. If there are many other tasks running in parallel from other sources, then you probably do not need to worry about idle time, since the threads will be kept busy by the other tasks anyway.

### Common index types

Let us now consider a subtlety regarding the types of the start and end indices. In the example [above](#automatic-parallelization-of-loops), the start index is `0`, which is of type `int`, while the end index is `max`, which is of type `std::size_t`. These two types are not compatible, as they are both of different signedness and (on a 64-bit system) of different bit width. In such cases, `submit_loop()` uses a custom type trait `BS::common_index_type` to determine the common type of the indices.

The common index type of two signed integers or two unsigned integers is the larger of the integers, while the common index type of a signed and an unsigned integer is a signed integer that can hold the full ranges of both integers. (This is in contrast to [`std::common_type`](https://en.cppreference.com/w/cpp/types/common_type), which would choose the unsigned integer in the latter case, causing a loop with a negative start index and an unsigned end index to fail due to integer overflow.)

The exception to this rule is when one of the integers is a 64-bit unsigned integer, and the other is a signed integer (of any bit width), since there is no fundamental signed type that can hold the full ranges of both integers. In this case, we choose a 64-bit unsigned integer as the common index type, since the most common scenario where this might happen is when the indices go from `0` to an index of type `std::size_t` - as in our example in the previous section.

However, it is important to note that this will fail if the first index is in fact negative. Therefore, **only** in the edge case where one index is a negative integer and the other is of an unsigned 64-bit integer type such as `std::size_t`, the user must cast both indices explicitly to the desired common type. In all other cases, this is handled automatically behind the scenes using `BS::common_index_type`.

### Parallelizing loops without futures

Just as in the case of [`detach_task()`](#detaching-and-waiting-for-tasks) vs. [`submit_task()`](#submitting-tasks-with-no-arguments-and-receiving-a-future), sometimes you may want to parallelize a loop, but you don't need it to return a `BS::multi_future`. In this case, you can save the overhead of generating the futures (which can be significant, depending on the number of blocks) by using `detach_loop()` instead of `submit_loop()`, with the same arguments.

For example, we could detach the loop of squares example above as follows:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <cstddef>            // std::size_t
#include <iomanip>            // std::setw
#include <iostream>           // std::cout

int main()
{
    BS::thread_pool pool(10);
    constexpr std::size_t max = 100;
    std::size_t squares[max];
    pool.detach_loop(0, max,
        [&squares](const std::size_t i)
        {
            squares[i] = i * i;
        });
    pool.wait();
    for (std::size_t i = 0; i < max; ++i)
        std::cout << std::setw(2) << i << "^2 = " << std::setw(4) << squares[i] << ((i % 5 != 4) ? " | " : "\n");
}
```

**Warning:** Since `detach_loop()` does not return a `BS::multi_future`, there is no built-in way for the user to know when the loop finishes executing. You must use either [`wait()`](#detaching-and-waiting-for-tasks) as we did here, or some other method such as condition variables, to ensure that the loop finishes executing before trying to use anything that depends on its output. Otherwise, bad things will happen! If the loop is the only thing running in the pool, then generally `detach_loop()` followed by `wait()` is the optimal choice in terms of performance.

### Parallelizing individual indices vs. blocks

We have seen that `detach_loop()` and `submit_loop()` execute the function `loop(i)` for each index `i` in the loop. However, behind the scenes, the loop is split into blocks, and each block executes the `loop()` function multiple times. Each block has an internal loop of the form (where `T` is the type of the indices):

```cpp
for (T i = start; i < end; ++i)
    loop(i);
```

The `start` and `end` indices of each block are determined automatically by the pool. For example, in the previous section, the loop from 0 to 100 was split into 10 blocks of 10 indices each: `start = 0` to `end = 10`, `start = 10` to `end = 20`, and so on; the blocks are not inclusive of the last index, since the `for` loop has the condition `i < end` and not `i <= end`.

However, this also means that the `loop()` function is executed multiple times per block. This generates additional overhead due to the multiple function calls. For short loops, this should not affect performance. However, for very long loops, with millions of indices, the performance cost may be significant.

For this reason, the thread pool library provides two additional member functions for parallelizing loops: `detach_blocks()` and `submit_blocks()`. While `detach_loop()` and `submit_loop()` execute a function `loop(i)` once per index but multiple times per block, `detach_blocks()` and `submit_blocks()` execute a function `block(start, end)` only once per block.

The main advantage of this method is increased performance, but the main disadvantage is slightly more complicated code. In particular, the user must define the loop from `start` to `end` manually within each block, ensuring that all the indices in the block are handled. Here is the previous example again, this time using `detach_blocks()`:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <cstddef>            // std::size_t
#include <iomanip>            // std::setw
#include <iostream>           // std::cout

int main()
{
    BS::thread_pool pool(10);
    constexpr std::size_t max = 100;
    std::size_t squares[max];
    pool.detach_blocks(0, max,
        [&squares](const std::size_t start, const std::size_t end)
        {
            for (std::size_t i = start; i < end; ++i)
                squares[i] = i * i;
        });
    pool.wait();
    for (std::size_t i = 0; i < max; ++i)
        std::cout << std::setw(2) << i << "^2 = " << std::setw(4) << squares[i] << ((i % 5 != 4) ? " | " : "\n");
}
```

Note how the block function takes two arguments, and includes the internal loop. Also, since we are using `detach_blocks()`, we must wait for the loop to finish executing using `wait()`. Alternatively, we could have used `submit_blocks()` and waited on the returned `BS::multi_future<void>` object.

Generally, compiler optimizations should be able to make `detach_loop()` and `submit_loop()` perform roughly the same as `detach_blocks()` and `submit_blocks()`. However, `detach_blocks()` and `submit_blocks()` are always going to be inherently faster, at the cost of being slightly more complicated to use. In addition, having low-level control of each block can allow for further optimizations, such as allocating resources per block instead of per index. As usual, you should perform your own benchmarks to see which option works best for your particular use case.

### Loops with return values

As mentioned above, unlike `submit_task()`, the member function `submit_loop()` only takes loop functions with no return value. The reason is that each block is running the loop function multiple times, so a return value would not make sense. In contrast, `submit_blocks()` allows the block function to have a return value, as each block can return a unique value.

The block function will be executed once for each block, but the blocks are managed by the thread pool, with the user only able to select the number of blocks, but not the range of each block. Therefore, there is limited usability in returning one value per block. However, for cases where this is desired, such as for summation or some sorting algorithms, `submit_blocks()` does accept functions with return values, in which case it returns a `BS::multi_future<T>` object where `T` is the type of the return value.

Here's an example of a function template summing all elements of type `T` in a given range:

```cpp
#include "BS_thread_pool.hpp" // BS::multi_future, BS::thread_pool
#include <cstdint>            // std::uint64_t
#include <future>             // std::future
#include <iostream>           // std::cout

BS::thread_pool pool;

template <typename T>
T sum(T min, T max)
{
    BS::multi_future<T> loop_future = pool.submit_blocks(
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

Note that we needed to specify the type `T` explicitly as `std::uint64_t`, that is, an unsigned 64-bit integer, as the result, 500,000,500,000, would not fit in a 32-bit integer.

Here we used the fact that `BS::multi_future<T>` is a specialization of `std::vector<std::future<T>>`, so we can use a range-based `for` loop to iterate over the futures, and use the `get()` member function of each future to get its value. The values of the futures will be the partial sums from each block, so when we add them up, we will get the total sum. Note that we divided the loop into 100 blocks, so there will be 100 futures in total, each with the partial sum of 10,000 numbers.

The range-based `for` loop will likely start before the loop finished executing, and each time it calls a future, it will get the value of that future if it is ready, or it will wait until the future is ready and then get the value. This increases performance, since we can start summing the results without waiting for the entire loop to finish executing first - we only need to wait for individual blocks.

If we did want to wait until the entire loop finishes before summing the results, we could have used the `get()` member function of the `BS::multi_future<T>` object itself, which returns an `std::vector<T>` with the values obtained from each future. In that case, the sum could be obtained after calling `submit_blocks()`, for example using `std::reduce`, as follows:

```cpp
#include "BS_thread_pool.hpp" // BS::multi_future, BS::thread_pool
#include <cstdint>            // std::uint64_t
#include <iostream>           // std::cout
#include <numeric>            // std::reduce
#include <vector>             // std::vector

BS::thread_pool pool;

template <typename T>
T sum(T min, T max)
{
    BS::multi_future<T> loop_future = pool.submit_blocks(
        min, max + 1,
        [](const T start, const T end)
        {
            T block_total = 0;
            for (T i = start; i < end; ++i)
                block_total += i;
            return block_total;
        },
        100);
    std::vector<T> partial_sums = loop_future.get();
    T result = std::reduce(partial_sums.begin(), partial_sums.end());
    return result;
}

int main()
{
    std::cout << sum<std::uint64_t>(1, 1'000'000);
}
```

### Parallelizing sequences

The member functions `detach_loop()`, `submit_loop()`, `detach_blocks()`, and `submit_blocks()` parallelize a loop by splitting it into blocks, and submitting each block as an individual task to the queue, with each such task iterating over all the indices in the corresponding block's range, which can be numerous. However, sometimes we have a loop with a small number of indices, or more generally, a sequence of tasks enumerated by some index. In such cases, we can avoid the overhead of splitting into blocks and simply submit each individual index as its own independent task to the pool's queue.

This can be done with `detach_sequence()` and `submit_sequence()`. The syntax of these functions is similar to `detach_loop()` and `submit_loop()`, except that they don't have the `num_blocks` argument at the end. The sequence function must take only one argument, the index.

As usual, `detach_sequence()` detaches the tasks and does not return a future, so you must use `wait()` if you need to wait for the entire sequence to finish executing, while `submit_sequence()` returns a `BS::multi_future`. If the tasks in the sequence return values, then the futures will contain those values, otherwise they will be `void` futures.

Here is a simple example, where each task in the sequence calculates the factorial of its index:

```cpp
#include "BS_thread_pool.hpp" // BS::multi_future, BS::thread_pool
#include <cstdint>            // std::uint64_t
#include <iostream>           // std::cout
#include <vector>             // std::vector

std::uint64_t factorial(const std::uint64_t n)
{
    std::uint64_t result = 1;
    for (std::uint64_t i = 2; i <= n; ++i)
        result *= i;
    return result;
}

int main()
{
    BS::thread_pool pool;
    constexpr std::uint64_t max = 20;
    BS::multi_future<std::uint64_t> sequence_future = pool.submit_sequence(0, max + 1, factorial);
    std::vector<std::uint64_t> factorials = sequence_future.get();
    for (std::uint64_t i = 0; i < max + 1; ++i)
        std::cout << i << "! = " << factorials[i] << '\n';
}
```

Note how the factorials of each index are stored in the `BS::multi_future`, and can be obtained as a vector using `get()`; each element of the vector is equal to the factorial of the element's index, calculated by its own individual task in the sequence.

**Warning:** Since each index in the sequence will be submitted as a separate task, `detach_sequence()` and `submit_sequence()` should only be used if the number of indices is small (say, within 1-2 orders of magnitude of the number of threads), and each index performs a substantial computation on its own. If you submit a sequence of 1 million indices, each performing a 1 ms calculation, the overhead of submitting each index as a separate task would far outweigh the benefits of parallelization.

### More about `BS::multi_future`

The helper class `BS::multi_future<T>`, which we have been using throughout this section, provides a convenient way to collect and access groups of futures. While a `BS::multi_future<T>` object is created automatically by the pool when parallelizing loops, you can also use it to store futures manually, such as those obtained from `submit_task()` or by other means. `BS::multi_future<T>` is a specialization of `std::vector<std::future<T>>`, so it should be used in a similar way:

* When you create a new `BS::multi_future<T>` object, either use the default constructor to create an empty object and add futures to it later, or pass the desired number of futures to the constructor in advance.
* Use the `[]` operator to access the future at a specific index, or the `push_back()` member function to append a new future to the list. (If the number of futures is known in advance, you should use `reserve()` to allocate memory for all of them first, and only then `push_back()` the individual futures, otherwise memory will have to be reallocated multiple times, which is very inefficient.)
* The `size()` member function tells you how many futures are currently stored in the object.

However, `BS::multi_future<T>` also has additional member functions that are aimed specifically at handling futures:

* Once all the futures are stored, you can use `wait()` to wait for all of them at once or `get()` to get an `std::vector<T>` with the results from all of them.
* You can check how many futures are ready using `ready_count()`.
* You can check if all the stored futures are valid using `valid()`.
* You can wait for all the stored futures for a specific duration with `wait_for()` or wait until a specific time with `wait_until()`. These functions return `true` if all futures have been waited for before the duration expired or the time point was reached, and `false` otherwise.

Aside from using `BS::multi_future<T>` to track the execution of parallelized loops, it can also be used, for example, whenever you have several different groups of tasks and you want to track the execution of each group individually.

## Utility classes

### Synchronizing printing to a stream with `BS::synced_stream`

When printing to an output stream from multiple threads in parallel, the output may become garbled. For example, try running this code:

```cpp
#include "BS_thread_pool.hpp" // BS::thread_pool
#include <iostream>           // std::cout

BS::thread_pool pool;

int main()
{
    pool.submit_sequence(0, 5,
            [](const unsigned int i)
            {
                std::cout << "Task no. " << i << " executing.\n";
            })
        .wait();
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

The thread pool utility class `BS::synced_stream` is designed to eliminate such synchronization issues. The stream to print to should be passed as a constructor argument. If no argument is supplied, `std::cout` will be used:

```cpp
// Construct a synced stream that will print to std::cout.
BS::synced_stream sync_out;
// Construct a synced stream that will print to the output stream my_stream.
BS::synced_stream sync_out(my_stream);
```

The member function `print()` takes an arbitrary number of arguments, which are inserted into the stream one by one, in the order they were given. `println()` does the same, but also prints a newline character `\n` at the end, for convenience. A mutex is used to synchronize this process, so that any other calls to `print()` or `println()` using the same `BS::synced_stream` object must wait until the previous call has finished.

As an example, this code:

```cpp
#include "BS_thread_pool.hpp" // BS::synced_stream, BS::thread_pool

BS::synced_stream sync_out;
BS::thread_pool pool;

int main()
{
    pool.submit_sequence(0, 5,
            [](const unsigned int i)
            {
                sync_out.println("Task no. ", i, " executing.");
            })
        .wait();
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

Most stream manipulators defined in the headers `<ios>` and `<iomanip>`, such as `std::setw` (set the character width of the next output), `std::setprecision` (set the precision of floating point numbers), and `std::fixed` (display floating point numbers with a fixed number of digits), can be passed as arguments to `print()` and `println()`, and will have the same effect as inserting them to the associated stream.

The only exceptions are the flushing manipulators `std::endl` and `std::flush`, which will not work because the compiler will not be able to figure out which template specializations to use. Instead, use `BS::synced_stream::endl` and `BS::synced_stream::flush`. Here is an example:

```cpp
#include "BS_thread_pool.hpp" // BS::synced_stream, BS::thread_pool
#include <cmath>              // std::sqrt
#include <iomanip>            // std::setprecision, std::setw
#include <ios>                // std::fixed

BS::synced_stream sync_out;
BS::thread_pool pool;

int main()
{
    sync_out.print(std::setprecision(10), std::fixed);
    pool.submit_sequence(0, 16,
            [](const unsigned int i)
            {
                sync_out.print("The square root of ", std::setw(2), i, " is ", std::sqrt(i), ".", BS::synced_stream::endl);
            })
        .wait();
}
```

Note, however, that `BS::synced_stream::endl` should only be used if flushing is desired; otherwise, a newline character should be used instead. As with `std::endl`, using `BS::synced_stream::endl` too often will cause a performance hit, as it will force the stream to flush the buffer every time it is called.

If desired, `BS::synced_stream` can also synchronize printing into more than one stream at a time. To facilitate this, we can pass a list of output streams to the constructor. For example, the following program will print the same output to both `std::cout` and a log file:

```cpp
#include "BS_thread_pool.hpp" // BS::synced_stream, BS::thread_pool
#include <fstream>            // std::ofstream
#include <iostream>           // std::cout

BS::thread_pool pool;

int main()
{
    std::ofstream log_file("task.log");
    BS::synced_stream sync_out(std::cout, log_file);
    pool.submit_sequence(0, 5,
            [&sync_out](const unsigned int i)
            {
                sync_out.println("Task no. ", i, " executing.");
            })
        .wait();
}
```

Note that we must wait on the future before the `main()` function ends, as otherwise the log file may be destructed before the tasks finish executing. If we used `detach_sequence()`, which does not return a future, we would have to call `pool.wait()` in the last line.

In this example we did not create the `BS::synced_stream` as a global object, since we wanted to pass the log file as a stream to the constructor. However, it is also possible to add streams to or remove streams from an existing `BS::synced_stream` object using the member functions `add_stream()` and `remove_stream()`. For example, in the following program, we create a `BS::synced_stream` global object with the default constructor, so that it prints to `std::cout`, but then we change out minds, remove `std::cout` from the list of streams, and add a log file instead:

```cpp
#include "BS_thread_pool.hpp" // BS::synced_stream, BS::thread_pool
#include <fstream>            // std::ofstream
#include <iostream>           // std::cout

BS::synced_stream sync_out;
BS::thread_pool pool;

int main()
{
    std::ofstream log_file("task.log");
    sync_out.remove_stream(std::cout);
    sync_out.add_stream(log_file);
    pool.submit_sequence(0, 5,
            [](const unsigned int i)
            {
                sync_out.println("Task no. ", i, " executing.");
            })
        .wait();
}
```

It is common practice to create a global `BS::synced_stream` object, so that it can be accessed from anywhere in the program, without having to pass it to every function that might want to print something to the stream. However, if you also have a global `BS::thread_pool` object, you must always make sure to define the global `BS::synced_stream` object **before** the global `BS::thread_pool` object, for the reasons explained in the warning above.

Internally, `BS::synced_stream` keeps the streams in an `std::vector<std::ostream*>`. The order in which the streams are added is also the order in which they will be printed to. For more precise control, you can use the member function `get_streams()` to get a reference to this vector, and manipulate it directly as you see fit.

### Synchronizing tasks with `BS::counting_semaphore` and `BS::binary_semaphore`

The thread pool library provides two utility classes, `BS::counting_semaphore` and `BS::binary_semaphore`, which offer versatile synchronization primitives that can be used to synchronize tasks in a variety of ways. These classes are equivalent to the C&plus;&plus;20 `std::counting_semaphore` and `std::binary_semaphore`, respectively, but are offered in the library as convenience polyfills for projects based on C&plus;&plus;17. If C&plus;&plus;20 features are available, the polyfills are not used, and instead are just aliases for the standard library classes.

Since `BS::counting_semaphore` and `BS::binary_semaphore` are identical in functionality to their standard library counterparts, we will not explain how to use them here. Instead, the user is referred to [cppreference.com](https://en.cppreference.com/w/cpp/thread/counting_semaphore).

## Managing tasks

### Monitoring the tasks

Sometimes you may wish to monitor what is happening with the tasks you submitted to the pool. This may be done using these three member functions:

* `get_tasks_queued()` gets the number of tasks currently waiting in the queue to be executed.
* `get_tasks_running()` gets the number of tasks currently being executed by the threads.
* `get_tasks_total()` gets the total number of unfinished tasks: either still in the queue, or being executed by a thread.

Note that `get_tasks_total() == get_tasks_queued() + get_tasks_running()`. These functions are demonstrated in the following program:

```cpp
#include "BS_thread_pool.hpp" // BS::synced_stream, BS::thread_pool
#include <chrono>             // std::chrono
#include <thread>             // std::this_thread

BS::synced_stream sync_out;
BS::thread_pool pool(4);

void sleep_half_second(const unsigned int i)
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
    pool.wait();
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

The reason we called `pool.wait()` in the beginning is that when the thread pool is created, an initialization task runs in each thread, so if we don't wait, the first line would say there are 16 tasks in total, including the 4 initialization tasks. See [below](#thread-initialization-functions) for more details. Of course, we also called `pool.wait()` at the end to ensure that all tasks have finished executing before the program ends.

### Purging tasks

Consider a situation where the user cancels a multithreaded operation while it is still ongoing. Perhaps the operation was split into multiple tasks, and half of the tasks are currently being executed by the pool's threads, but the other half are still waiting in the queue.

The thread pool cannot terminate the tasks that are already running, as C&plus;&plus; does not provide that functionality (and in any case, abruptly terminating a task while it's running could have extremely bad consequences, such as memory leaks and data corruption). However, the tasks that are still waiting in the queue can be purged using the `purge()` member function.

Once `purge()` is called, any tasks still waiting in the queue will be discarded, and will never be executed by the threads. Please note that there is no way to restore the purged tasks; they are gone forever!

Consider for example the following program:

```cpp
#include "BS_thread_pool.hpp" // BS::synced_stream, BS::thread_pool
#include <chrono>             // std::chrono
#include <thread>             // std::this_thread

BS::synced_stream sync_out;
BS::thread_pool pool(4);

int main()
{
    pool.detach_sequence(0, 8,
        [](const unsigned int i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            sync_out.println("Task ", i, " done.");
        });
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

Please note that, as explained above, the thread pool cannot terminate running tasks on its own. If you need to do that, you must incorporate a mechanism into the task itself that will terminate the task safely. For example, you could create an atomic flag that the task checks periodically, terminating itself if the flag is set. Here is a simple example:

```cpp
#include "BS_thread_pool.hpp" // BS::synced_stream, BS::thread_pool
#include <chrono>             // std::chrono
#include <thread>             // std::this_thread

BS::synced_stream sync_out;
BS::thread_pool pool(4);

int main()
{
    std::atomic<bool> stop_flag = false;
    pool.detach_sequence(0, 8,
        [&stop_flag](const unsigned int i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (stop_flag)
                return;
            sync_out.println("Task ", i, " done.");
        });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    stop_flag = true;
    pool.purge();
    pool.wait();
}
```

This program will not print out any output, as the tasks will terminate themselves prematurely when `stop_flag` is set to `true`. In this case, we did not have to call `purge()`, but by doing so we prevented the other 4 tasks from being executed for no reason.

### Exception handling

`submit_task()` catches any exceptions thrown by the submitted task and forwards them to the corresponding future. They can then be caught when invoking the `get()` member function of the future. For example:

```cpp
#include "BS_thread_pool.hpp" // BS::synced_stream, BS::thread_pool
#include <exception>          // std::exception
#include <future>             // std::future
#include <stdexcept>          // std::runtime_error

BS::synced_stream sync_out;
BS::thread_pool pool;

double inverse(const double x)
{
    if (x == 0)
        throw std::runtime_error("Division by zero!");
    return 1 / x;
}

int main()
{
    constexpr double num = 0;
    std::future<double> my_future = pool.submit_task(
        [num]
        {
            return inverse(num);
        });
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
#include "BS_thread_pool.hpp" // BS::synced_stream, BS::thread_pool
#include <exception>          // std::exception
#include <future>             // std::future
#include <stdexcept>          // std::runtime_error

BS::synced_stream sync_out;
BS::thread_pool pool;

void print_inverse(const double x)
{
    if (x == 0)
        throw std::runtime_error("Division by zero!");
    sync_out.println("The inverse of ", x, " is ", 1 / x, ".");
}

int main()
{
    constexpr double num = 0;
    std::future<void> my_future = pool.submit_task(
        [num]
        {
            print_inverse(num);
        });
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

When using `BS::multi_future<T>` to handle multiple futures at once, exception handling works the same way: if any of the futures may throw exceptions, you may catch these exceptions when calling `get()`, even in the case of `BS::multi_future<void>`.

Note that if you use `detach_task()`, or any other `detach` member function, there is no way to catch exceptions thrown by the task, as a future will not be returned. In such cases, all exceptions thrown by the task will be silently ignored, to prevent program termination. If you need to catch exceptions in a detached task, you must do so within the task itself, as in this example:

```cpp
#include "BS_thread_pool.hpp" // BS::synced_stream, BS::thread_pool
#include <exception>          // std::exception
#include <stdexcept>          // std::runtime_error

BS::synced_stream sync_out;
BS::thread_pool pool;

double inverse(const double x)
{
    if (x == 0)
        throw std::runtime_error("Division by zero!");
    return 1 / x;
}

int main()
{
    constexpr double num = 0;
    pool.detach_task(
        [num]
        {
            try
            {
                const double result = inverse(num);
                sync_out.println("The inverse of ", num, " is ", result, ".");
            }
            catch (const std::exception& e)
            {
                sync_out.println("Caught exception: ", e.what());
            }
        });
    pool.wait();
}
```

If exceptions are explicitly disabled in your codebase, or if the feature-test macro `__cpp_exceptions` is undefined for any other reason, exception handling will be automatically disabled in the thread pool.

### Getting information about the current thread

The class `BS::this_thread` provides functionality analogous to `std::this_thread`, that is, it allows a thread to reference itself. It contains the following static member functions:

* `BS::this_thread::get_index()` can be used to get the index of the current thread as an `std::optional<std::size_t>` object.
    * If this thread belongs to a `BS::thread_pool` object, the return value will be an index in the range `[0, N)` where `N == BS::thread_pool::get_thread_count()`.
    * Otherwise, for example if this thread is the main thread or an independent thread not in any pools, `std::nullopt` will be returned.
* `BS::this_thread::get_pool()` can be used to get a pointer to the thread pool that owns the current thread as an `std::optional<void*>` object.
    * If this thread belongs to a `BS::thread_pool` object, the return value will be a `void` pointer to that object.
    * Otherwise, `std::nullopt` will be returned.

An [`std::optional`](https://en.cppreference.com/w/cpp/utility/optional) is an object that may or may not have a value. [`std::nullopt`](https://en.cppreference.com/w/cpp/utility/optional/nullopt) is a placeholder which indicates that the object does not have a value. To access an `std::optional`, you should first use [`std::optional::has_value()`](https://en.cppreference.com/w/cpp/utility/optional/operator_bool) to check if it contains a value, and if so, use [`std::optional::value()`](https://en.cppreference.com/w/cpp/utility/optional/value) to obtain that value. A shortcut for `if (x.has_value())` is `if (x)`, and a shortcut for `x.value()` is `*x`.

The reason that `BS::this_thread::get_pool()` returns a `void*` is that `BS::thread_pool` is a template. Once you obtain the pool pointer, you must cast it to the desired instantiation of the template if you want to use any member functions. Note that you have to cast it to the correct type; if you cast a pointer to a `BS::light_thread_pool` into a pointer to a `BS::priority_thread_pool`, for example, your program will have undefined behavior. (Please see the [optional features](#optional-features) section for more information about the template parameters and aliases.)

Here is an example illustrating all of the above:

```cpp
#include "BS_thread_pool.hpp" // BS::light_thread_pool, BS::synced_stream, BS::this_thread
#include <atomic>             // std::atomic
#include <cstddef>            // std::size_t
#include <optional>           // std::optional
#include <thread>             // std::thread

BS::synced_stream sync_out;
BS::light_thread_pool p1;
BS::light_thread_pool p2;
std::atomic<char> ltr = 'A';

void check_this_thread(const char letter)
{
    const std::optional<void*> my_pool = BS::this_thread::get_pool();
    const std::optional<std::size_t> my_index = BS::this_thread::get_index();

    if (my_pool && my_index)
    {
        const std::size_t pool_number = *my_pool == &p1 ? 1 : 2;
        sync_out.println("Task ", letter, " is being executed by thread #", *my_index, " of pool #", pool_number, '.');
        static_cast<BS::light_thread_pool*>(*my_pool)->detach_task(
            [letter]
            {
                sync_out.println("-> Task ", ltr++, " was submitted by task ", letter, " using detach_task().");
            });
    }
    else
    {
        sync_out.println("Task ", letter, " is being executed by an independent thread, not in any thread pools.");
        std::thread(
            [letter]
            {
                sync_out.println("-> Task ", ltr++, " was submitted by task ", letter, " using a detached std::thread.");
            })
            .detach();
    }
}

int main()
{
    p1.submit_task(
          []
          {
              check_this_thread(ltr++);
          })
        .wait();
    p2.submit_task(
          []
          {
              check_this_thread(ltr++);
          })
        .wait();
    std::thread(
        []
        {
            check_this_thread(ltr++);
        })
        .join();
}
```

The output of this program will be similar to:

```none
Task A is being executed by thread #3 of pool #1.
-> Task B was submitted by task A using detach_task().
Task C is being executed by thread #7 of pool #2.
-> Task D was submitted by task C using detach_task().
Task E is being executed by an independent thread, not in any thread pools.
-> Task F was submitted by task E using a detached std::thread.
```

In this example, we execute the task `check_this_thread()` in three different ways:

1. By submitting it from the thread pool `p1`.
2. By submitting it from the thread pool `p2`.
3. By submitting it from an independent `std::thread`.

The task calls `BS::this_thread::get_pool()` and `BS::this_thread::get_index()` and receives two `std::optional` objects, `my_pool` and `my_index`. If both have a value (that is, evaluate to `true`), then the task knows it is running in a thread pool. The actual values are then obtained by "dereferencing" them: the pool pointer is `*my_pool`, and the thread index is `*my_index`.

The task deduces which pool it is running in by comparing the pointer `*my_pool` to the addresses of the pools `p1` and `p2`. It also gets the index of the thread from `*my_index`. Finally, it detaches an additional task (without waiting for it, as that might cause a deadlock!) from its own pool by first casting the `void*` pointer to the correct type, which in this case is `BS::light_thread_pool*`, and then calling the `detach_task()` member function of that specific pool.

If `my_pool` and `my_index` do not have values (that is, evaluate to `false`), then the task knows it is running in an independent thread. In this case, it detaches the additional task using another independent thread.

### Thread initialization functions

Sometimes, it is necessary to initialize the threads before they run any tasks. This can be done by submitting a proper initialization function to the `BS::thread_pool` constructor or to `reset()`, either as the only argument or as the second argument after the desired number of threads.

The thread initialization function must have no return value. It can either take one argument, the thread index of type `std::size_t`, or zero arguments. In the latter case, the function can use `BS::this_thread::get_index()` to find the thread index. In addition, the function can use `BS::this_thread::get_pool()` to find which pool its thread belongs to.

The initialization functions are effectively submitted as a set of special tasks, one per thread, which bypass the queue, but still count towards the number of running tasks. This means `get_tasks_total()` and `get_tasks_running()` will report that these tasks are running if they are checked immediately after the pool is initialized.

This is done so that the user has the option to either wait for the initialization functions to finish, by calling `wait()` on the pool, or just keep going. In either case, the initialization functions will always finish running before any tasks are executed by the corresponding thread, so there is no reason to wait for them to finish unless they have some side-effects that affect the main thread, or if they must finish running on **all** the threads before the pool starts executing any tasks.

Here is a simple example:

```cpp
#include "BS_thread_pool.hpp" // BS::synced_stream, BS::thread_pool
#include <random>             // std::mt19937_64, std::random_device

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

In this example, we create a `thread_local` Mersenne twister engine, meaning that each thread has its own independent engine. However, if we do not seed the engine, each thread would generate the exact same sequence of pseudo-random numbers. To remedy this, we pass an initialization function to the `BS::thread_pool` constructor which seeds the twister in each thread with the (hopefully) non-deterministic random number generator `std::random_device`.

Note that the lambda function we passed to `submit_sequence()` has the signature `[](int)`, with an unnamed `int` argument, as it does not make use of the sequence index, which will be a number in the range `[0, 4)`. This is an easy way to simply submit the same task multiple times.

**Warning:** Exceptions thrown by thread initialization functions must not throw any exceptions, as that will result in program termination. Any exceptions must be handled explicitly within the function.

### Thread cleanup functions

Similarly to the thread initialization function, it is also possible to provide the pool with a cleanup function to run in each thread right before it is destroyed, which will happen when the pool is destructed or reset. Like the initialization function, the cleanup function must have no return value, and can either take one argument, the thread index of type `std::size_t`, or zero arguments. Each pool can have its own cleanup function, which is specified using the member function `set_cleanup_func()`. Here is an example:

```cpp
#include "BS_thread_pool.hpp" // BS::synced_stream, BS::this_thread, BS::thread_pool
#include <chrono>             // std::chrono
#include <cstddef>            // std::size_t
#include <fstream>            // std::ofstream
#include <string>             // std::to_string
#include <thread>             // std::this_thread

thread_local std::ofstream log_file;
thread_local BS::synced_stream sync_out(log_file);
constexpr std::size_t threads = 4;

int main()
{
    BS::thread_pool pool(threads,
        [](const std::size_t idx)
        {
            log_file.open("thread_" + std::to_string(idx) + ".log");
        });
    pool.set_cleanup_func(
        []
        {
            log_file.close();
        });
    pool.submit_sequence(0, threads * 10,
            [](const std::size_t idx)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                sync_out.println("Task ", idx, " is running on thread ", *BS::this_thread::get_index(), '.');
            })
        .wait();
}
```

In this example, we create 4 threads, each of which has a separate thread-local `BS::synced_stream` object writing to its own log file of the form `thread_N.log` where `N` is the thread index. The initialization function, passed as an argument to the constructor, opens the log file. The cleanup function, set using `set_cleanup_func()`, closes the log file.

We submit 40 tasks to the queue using `submit_sequence()`, each of which prints a message to the log file indicating which thread it is running on. When the `main()` function exits and `pool` is destroyed, the cleanup function is called for each thread, ensuring that the log files are closed properly.

**Warning:** As with initialization functions, exceptions thrown by thread cleanup functions must not throw any exceptions, as that will result in program termination. Any exceptions must be handled explicitly within the function.

### Passing task arguments by constant reference

In C&plus;&plus;, it is often crucial to pass function arguments by reference or constant reference, instead of by value. This allows the function to access the object being passed directly, rather than creating a new copy of the object. We have already seen [above](#detaching-and-waiting-for-tasks) that submitting an argument by reference is a simple matter of capturing it with a `&` in the lambda capture list. To submit by **constant** reference, we can use `std::as_const()` as in the following example:

```cpp
#include "BS_thread_pool.hpp" // BS::synced_stream, BS::thread_pool
#include <utility>            // std::as_const

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

Similarly, the `print()` function takes a **constant reference** to an integer, and prints that integer. Passing the argument by constant reference guarantees that the variable will not be accidentally modified by the function, even though we are accessing `n` itself, rather than a copy. If we replace `print()` with `increment()`, the program won't compile, as `increment()` cannot take constant references.

Generally, it is not really necessary to pass arguments by constant reference, but it is more "correct" to do so, if we would like to guarantee that the variable being referenced is indeed never modified.

## Optional features

### Enabling features

The thread pool has some optional features, which are disabled by default to minimize overhead. They can be enabled by passing the appropriate template parameter to the `BS::thread_pool` class when creating the pool. The template parameter is a bitmask, so you can enable several features at once by combining them with the bitwise OR operator `|`. The bitmask flags are members of the `BS::tp` enumeration:

* `BS::tp::priority` enables [task priority](#setting-task-priority).
* `BS::tp::pause` enables [pausing the pool](#pausing-the-pool).
* `BS::tp::wait_deadlock_checks` enables [wait deadlock checks](#avoiding-wait-deadlocks).
* The default is `BS::tp::none`, which disables all optional features.

For example, to enable both task priority and pausing the pool, the thread pool object should be created like this:

```cpp
BS::thread_pool<BS::tp::priority | BS::tp::pause> pool;
```

Convenience aliases are defined as follows:

* `BS::light_thread_pool` disables all optional features (equivalent to `BS::thread_pool` with the default template parameter, that is, `BS::thread_pool<BS::tp::none>`).
* `BS::priority_thread_pool` enables task priority (equivalent to `BS::thread_pool<BS::tp::priority>`).
* `BS::pause_thread_pool` enables pausing the pool (equivalent to `BS::thread_pool<BS::tp::pause>`).
* `BS::wdc_thread_pool` enables wait deadlock checks (equivalent to `BS::thread_pool<BS::tp::wait_deadlock_checks>`).

There are no aliases with multiple features enabled; if this is desired, you must either pass the template parameter explicitly or define your own alias, and use the bitwise OR operator as shown above.

Note that, since optional features are enabled separately for each `BS::thread_pool` object, you can have multiple pools with different features enabled in the same program. For example, you can have one `BS::light_thread_pool` for tasks that do not need to be prioritized, and a separate `BS::priority_thread_pool` for tasks that do.

### Setting task priority

Turning on the `BS::tp::priority` flag in the template parameter to `BS::thread_pool` enables task priority. In addition, the library defines the convenience alias `BS::priority_thread_pool`, which is equivalent to `BS::thread_pool<BS::tp::priority>`. When this feature is enabled, the static member `priority_enabled` will be set to `true`.

The priority of a task or group of tasks may then be specified as an additional argument (at the end of the argument list) to `detach_task()`, `submit_task()`, `detach_blocks()`, `submit_blocks()`, `detach_loop()`, `submit_loop()`, `detach_sequence()`, and `submit_sequence()`. If the priority is not specified, the default value will be 0.

The priority is a number of type `BS::priority_t`, which is a signed 8-bit integer, so it can have any value between -128 and +127. The tasks will be executed in priority order from highest to lowest. If priority is assigned to the block/loop/sequence parallelization functions, which submit multiple tasks, then all of these tasks will have the same priority.

The enumeration `BS::pr` contains some pre-defined priorities for users who wish to avoid magic numbers and enjoy better future-proofing. In order of decreasing priority, the pre-defined priorities are: `BS::pr::highest`, `BS::pr::high`, `BS::pr::normal`, `BS::pr::low`, and `BS::pr::lowest`.

Here is a simple example:

```cpp
#include "BS_thread_pool.hpp" // BS::priority_thread_pool, BS::synced_stream

BS::synced_stream sync_out;
BS::priority_thread_pool pool(1);

int main()
{
    pool.detach_task(
        []
        {
            sync_out.println("This task will execute third.");
        },
        BS::pr::normal);
    pool.detach_task(
        []
        {
            sync_out.println("This task will execute fifth.");
        },
        BS::pr::lowest);
    pool.detach_task(
        []
        {
            sync_out.println("This task will execute second.");
        },
        BS::pr::high);
    pool.detach_task(
        []
        {
            sync_out.println("This task will execute first.");
        },
        BS::pr::highest);
    pool.detach_task(
        []
        {
            sync_out.println("This task will execute fourth.");
        },
        BS::pr::low);
}
```

This program will print out the tasks in the correct priority order. Note that for simplicity, we used a pool with just one thread, so the tasks will run one at a time. In a pool with 5 or more threads, all 5 tasks will actually run more or less at the same time, because, for example, the task with the second-highest priority will be picked up by another thread while the task with the highest priority is still running.

Of course, this is just a pedagogical example. In a realistic use case we may want, for example, to submit tasks that must be completed immediately with high priority so they skip over other tasks already in the queue, or background non-urgent tasks with low priority so they evaluate only after higher-priority tasks are done.

Task priority is facilitated using [`std::priority_queue`](https://en.cppreference.com/w/cpp/container/priority_queue), which has O(log n) complexity for storing new tasks, but only O(1) complexity for retrieving the next (i.e. highest-priority) task. This is in contrast with [`std::queue`](https://en.cppreference.com/w/cpp/container/queue), used if priority is disabled, which both stores and retrieves with O(1) complexity.

Due to this, enabling the priority queue can incur a very slight decrease in performance, depending on the specific use case, which is why this feature is disabled by default. In other words, you gain functionality, but pay for it in performance. However, the difference in performance is never substantial, and compiler optimizations can often reduce it to a negligible amount.

Lastly, please note that when using the priority queue, tasks will not necessarily be executed in the same order they were submitted, **even if they all have the same priority**. This is due to the implementation of `std::priority_queue` as a [binary heap](https://en.wikipedia.org/wiki/Binary_heap), which means tasks are stored as a binary tree instead of sequentially.

### Pausing the pool

Turning on the `BS::tp::pause` flag in the template parameter to `BS::thread_pool` enables pausing the pool. In addition, the library defines the convenience alias `BS::pause_thread_pool`, which is equivalent to `BS::thread_pool<BS::tp::pause>`. When this feature is enabled, the static member `pause_enabled` will be set to `true`.

This feature enables the member functions `pause()`, `unpause()`, and `is_paused()`. When you call `pause()`, the workers will temporarily stop retrieving new tasks out of the queue. However, any tasks already executed will keep running until they are done, since the thread pool has no control over the internal code of your tasks. If you need to pause a task in the middle of its execution, you must do that manually by programming your own pause mechanism into the task itself. To resume retrieving tasks, call `unpause()`. To check whether the pool is currently paused, call `is_paused()`.

Here is an example:

```cpp
#include "BS_thread_pool.hpp" // BS::pause_thread_pool, BS::synced_stream
#include <chrono>             // std::chrono
#include <thread>             // std::this_thread

BS::synced_stream sync_out;
BS::pause_thread_pool pool(4);

void sleep_half_second(const unsigned int i)
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

In this example, we initially submit a total of 8 tasks to the queue. The first 4 tasks start running immediately (only 4, since the pool has 4 threads). We wait for 250ms, and then pause. The tasks that are already running (for 500ms) will keep running until they finished; pausing has no effect on currently running tasks. However, the other 4 tasks will not be executed yet. While the pool is paused, we submit 4 more tasks to the queue, but they just wait at the end of the queue. When we unpause, the remaining 4 initial tasks are executed, followed by the 4 new tasks.

While the workers are paused, `wait()` will wait only for the running tasks instead of all tasks (otherwise it would wait forever). This is demonstrated by the following program:

```cpp
#include "BS_thread_pool.hpp" // BS::pause_thread_pool, BS::synced_stream
#include <chrono>             // std::chrono
#include <thread>             // std::this_thread

BS::synced_stream sync_out;
BS::pause_thread_pool pool(4);

void sleep_half_second(const unsigned int i)
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

Note that pausing the pool adds additional checks to the waiting and worker functions, which have a very small but non-zero overhead. This is why this feature is disabled by default.

**Warning:** If the thread pool is destroyed while paused, any tasks still in the queue will never be executed!

### Avoiding wait deadlocks

Turning on the `BS::tp::wait_deadlock_checks` flag in the template parameter to `BS::thread_pool` enables wait deadlock checks. In addition, the library defines the convenience alias `BS::wdc_thread_pool`, which is equivalent to `BS::thread_pool<BS::tp::wait_deadlock_checks>`. When this feature is enabled, the static member `wait_deadlock_checks_enabled` will be set to `true`.

To understand why this feature is useful, consider the following program:

```cpp
#include "BS_thread_pool.hpp" // BS::synced_stream, BS::thread_pool

BS::synced_stream sync_out;
BS::thread_pool pool;

int main()
{
    pool.detach_task(
        []
        {
            pool.wait();
            sync_out.println("Done waiting.");
        });
}
```

This program creates a thread pool, and then detaches a task that waits for tasks in the same thread pool to complete. If you run this program, it will never print the message "Done waiting", because the task will wait for **itself** to complete. This causes a **deadlock**, and the program will wait forever.

Usually, in simple programs, this will never happen. However, in more complicated programs, perhaps ones running multiple thread pools in parallel, wait deadlocks could potentially occur. In such cases, wait deadlock checks may be useful. If enabled, `wait()`, `wait_for()`, and `wait_until()` will check whether the user tried to call them from within a thread of the same pool, and if so, they will throw the exception `BS::wait_deadlock` instead of waiting.

Here is an example:

```cpp
#include "BS_thread_pool.hpp" // BS::synced_stream, BS::wdc_thread_pool

BS::synced_stream sync_out;
BS::wdc_thread_pool pool;

int main()
{
    pool.detach_task(
        []
        {
            try
            {
                pool.wait();
                sync_out.println("Done waiting.");
            }
            catch (const BS::wait_deadlock&)
            {
                sync_out.println("Error: Deadlock!");
            }
        });
}
```

This time, `wait()` will detect the deadlock, and will throw an exception, causing the output to be "Error: Deadlock!".

Wait deadlock checks are disabled by default because wait deadlocks are not something that happens often, and the check adds a small but non-zero overhead every time `wait()`, `wait_for()`, or `wait_until()` is called. Note that if the feature-test macro `__cpp_exceptions` is undefined, wait deadlock checks will be automatically disabled, and trying to compile a program which creates a pool with the `BS::tp::wait_deadlock_checks` flag enabled will result in a compilation error.

## Native extensions

### Enabling the native extensions

While portability is one of the guiding principle for developing this library, non-portable features such as setting the thread priority using the operating system's native API can be very useful. Therefore, the library includes native extensions - which are disabled by default, as they are not portable.

The native extensions may be enabled by defining the macro `BS_THREAD_POOL_NATIVE_EXTENSIONS` at compilation time. If including the library as a header file, the macro must be defined before `#include "BS_thread_pool.hpp"`. Note that even if the macro is defined, the native extensions are disabled automatically if a supported operating system (Windows, Linux, or macOS) is not detected.

If importing the library [as a C&plus;&plus;20 module](#importing-the-library-as-a-c20-module), defining the macro before importing the module will not work, as modules cannot access macros defined in the program that imported them. Instead, you must define the macro as a compiler flag: `-D BS_THREAD_POOL_NATIVE_EXTENSIONS` for Clang and GCC or `/D BS_THREAD_POOL_NATIVE_EXTENSIONS` for MSVC.

[The test program](#testing-the-library) only tests the native extensions if the macro `BS_THREAD_POOL_NATIVE_EXTENSIONS` is defined at compilation time. If importing the library [as a C&plus;&plus;20 module](#importing-the-library-as-a-c20-module), please ensure that the macro is also enabled when compiling the module.

The `constexpr` flag `BS::thread_pool_native_extensions` indicates whether the thread pool library was compiled with native extensions enabled. Note that the flag will be `false` if `BS_THREAD_POOL_NATIVE_EXTENSIONS` is defined but the operating system is unsupported.

**Warning:** Please note that, as of v5.0.0 of the thread pool library, the native extensions have only been tested on **Windows 11 23H2, Ubuntu 24.10, and macOS 15.1**. They have not been tested on older versions of these operating systems, other Linux distributions, or any other operating systems, and are therefore not guaranteed to work on every system. If you encounter any issues, please report them on [the GitHub repository](https://github.com/bshoshany/thread-pool).

### Setting thread priority

The thread pool's native extensions provide the ability to set a thread's priority using the operating system's native API. Please note that this is **not** the same as [setting a task's priority](#setting-task-priority), which is a feature of the thread pool's queue, unrelated to the pool's threads themselves. Task priority controls which tasks are executed first, while thread priority (roughly) controls how much CPU time a thread gets compared to other threads. In addition, you can use the native extensions to set the priority of any thread (such as a thread created using `std::thread`), not just a pool thread.

For performance-critical applications, you may wish to increase the thread priority, while for applications that should run in the background, you may wish to decrease it. As priority is handled very differently on different operating systems, the thread pool library provides an abstraction layer over the native APIs, in the form of the enumeration class `BS::os_thread_priority`, which has the following 7 members:

* `BS::os_thread_priority::idle`
* `BS::os_thread_priority::lowest`
* `BS::os_thread_priority::below_normal`
* `BS::os_thread_priority::normal`
* `BS::os_thread_priority::above_normal`
* `BS::os_thread_priority::highest`
* `BS::os_thread_priority::realtime`

On Windows, these pre-defined priorities map 1-to-1 with [the thread priority values defined by the Windows API](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setthreadpriority) (with `realtime` mapping to time critical priority). On Linux and macOS, thread priorities are a lot more complicated, so these pre-defined priorities are mapped to the parameters available in the native API.

On Linux (with POSIX threads), thread priority is determined by three factors: [scheduling policy](https://www.man7.org/linux/man-pages/man3/pthread_setschedparam.3.html), priority value, and ["nice" value](https://www.man7.org/linux/man-pages/man2/setpriority.2.html). The thread pool library's abstraction layer distills these factors into the above pre-defined levels, for simplicity and portability. The total number of possible combinations of parameters is much larger, but allowing more fine-grained control would not be portable, and in any case it would have limited use. For the precise mapping, please refer to the source code itself (in the header file `BS_thread_pool.hpp`).

On macOS, the thread pool library will also use POSIX threads, but unlike Linux, the "nice" value is per-process, not per-thread (in compliance with the POSIX standard). However, macOS does allow more freedom with respect to the available range of priorities. Again, for the precise details of the mapping, please refer to the source code itself.

Most users do not need to worry about the specifics of how thread priority is handled on different operating systems. The abstraction layer provided by the thread pool library is meant to make everything as simple and portable as possible. However, it is important to note that only Windows allows a non-privileged user to set a thread's priority to a higher value. On Linux and macOS, a non-privileged user can only set a thread's priority to a lower value, and only root can set a higher value; also, confusingly, if a user decreased the priority of their thread from normal to a lower priority, they cannot increase it back to normal without root privileges, even though normal was the thread's initial priority.

Thread priority is managed using two static member functions of the `BS::this_thread` class:

* `BS::this_thread::get_os_thread_priority()` gets the current thread's priority. It returns an object of type `std::optional<BS::os_thread_priority>`. If the returned object does not contain a value, then either the priority could not be determined, or it is not one of the pre-defined values listed above.
* `BS::this_thread::set_os_thread_priority()` sets the current thread's priority. It returns `true` if the priority was set successfully, or `false` otherwise. Usually, `false` means that the user does not have the necessary permissions to set the desired priority.

Increasing or decreasing the priority of all the threads in a pool can be done most easily using an [initialization function](#thread-initialization-functions). Here is an example:

```cpp
#define BS_THREAD_POOL_NATIVE_EXTENSIONS
#include "BS_thread_pool.hpp" // BS::os_thread_priority, BS::synced_stream, BS::this_thread, BS::thread_pool
#include <cstddef>            // std::size_t
#include <map>                // std::map
#include <optional>           // std::optional
#include <string>             // std::string

BS::synced_stream sync_out;
BS::os_thread_priority target = BS::os_thread_priority::highest;

const std::map<BS::os_thread_priority, std::string> os_thread_priority_map = {{BS::os_thread_priority::idle, "idle"}, {BS::os_thread_priority::lowest, "lowest"}, {BS::os_thread_priority::below_normal, "below_normal"}, {BS::os_thread_priority::normal, "normal"}, {BS::os_thread_priority::above_normal, "above_normal"}, {BS::os_thread_priority::highest, "highest"}, {BS::os_thread_priority::realtime, "realtime"}};

std::string os_thread_priority_name(const BS::os_thread_priority priority)
{
    const std::map<BS::os_thread_priority, std::string>::const_iterator it = os_thread_priority_map.find(priority);
    return (it != os_thread_priority_map.end()) ? it->second : "unknown";
}

void set_priority(const std::size_t idx)
{
    const std::optional<BS::os_thread_priority> get_result = BS::this_thread::get_os_thread_priority();
    if (get_result)
        sync_out.println("The OS thread priority of thread ", idx, " is currently set to '", os_thread_priority_name(*get_result), "'.");
    else
        sync_out.println("Error: Failed to get the OS thread priority of thread ", idx, '!');
    const bool set_result = BS::this_thread::set_os_thread_priority(target);
    sync_out.println(set_result ? "Successfully" : "Error: Failed to", " set the OS priority of thread ", idx, " to '", os_thread_priority_name(target), "'.");
}

int main()
{
    BS::thread_pool pool(4, set_priority);
}
```

On Linux or macOS, please ensure that you run this example as root using `sudo`, otherwise it will fail. In this example we used an initialization function `set_priority()` to first print the initial priority of each thread (which should be "normal") and then set the priority of each thread to "highest". `os_thread_priority_name()` is a helper function to convert a `BS::os_thread_priority` value to a human-readable string.

### Setting thread affinity

The thread pool's native extensions allow the user to set a thread's processor affinity using the operating system's native API. Processor affinity, sometimes called "pinning", controls which logical processors a thread is allowed to run on. Generally, a non-hyperthreaded core corresponds to one logical processor, and a hyperthreaded core corresponds to two logical processors.

This can be useful for performance optimization, as it can reduce cache misses. However, it can also degrade performance, sometimes severely, since the thread will not run at all until its assigned cores are available. Therefore, it is usually better to let the operating system's scheduler manage thread affinities on its own, except in very specific cases.

Please note that setting thread affinity works on Windows and Linux, but not on macOS, as the native API does not allow it. As affinity is handled differently on different operating systems, the thread pool library provides an abstraction layer over the native APIs. In this abstraction layer, affinity is controlled using an `std::vector<bool>` where each element corresponds to a logical processor.

Thread affinity is managed using two static member functions of the `BS::this_thread` class:

* `BS::this_thread::get_os_thread_affinity()` gets the current thread's affinity. It returns an object of type `std::optional<std::vector<bool>>`. If the returned object does not contain a value, then the affinity could not be determined. On macOS, this function always returns `std::nullopt`.
* `BS::this_thread::set_os_thread_affinity()` sets the current thread's affinity. It returns `true` if the affinity was set successfully, or `false` otherwise. On macOS, this function always returns `false`.

Note that the thread affinity must be a subset of the process affinity (as obtained using [`BS::get_os_process_affinity()`](#setting-process-affinity)) for the containing process of a thread.

Setting thread affinity can significantly increase performance if multiple threads are accessing the same data, as the data can be kept in the local cache of the specific core that the threads are running on. This is illustrated in the following program:

```cpp
#define BS_THREAD_POOL_NATIVE_EXTENSIONS
#include "BS_thread_pool.hpp" // BS::synced_stream, BS::this_thread
#include <atomic>             // std::atomic
#include <chrono>             // std::chrono
#include <cstdint>            // std::uint64_t
#include <thread>             // std::thread
#include <vector>             // std::vector

void do_test(const bool pin_threads)
{
    BS::synced_stream sync_out;
    constexpr std::uint64_t num_increments = 10'000'000;
    sync_out.println(pin_threads ? "With   " : "Without", " thread pinning:");
    std::atomic<std::uint64_t> counter = 0;
    auto worker = [&counter, pin_threads]
    {
        if (pin_threads)
        {
            std::vector<bool> affinity(std::thread::hardware_concurrency(), false);
            affinity[0] = true;
            BS::this_thread::set_os_thread_affinity(affinity);
        }
        for (std::uint64_t i = 0; i < num_increments; ++i)
            ++counter;
    };
    const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    std::thread thread1(worker);
    std::thread thread2(worker);
    thread1.join();
    thread2.join();
    const std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    sync_out.println("Final count: ", counter, ", execution time: ", (std::chrono::duration_cast<std::chrono::milliseconds>(end - start)).count(), " ms.");
}

int main()
{
    do_test(false);
    do_test(true);
}
```

The output should be similar to:

```none
Without thread pinning:
Final count: 20000000, execution time: 160 ms.
With thread pinning:
Final count: 20000000, execution time: 68 ms.
```

In this program, we create two threads, each of which increments an atomic counter 10 million times. First, we do this without thread pinning; in this case, since the OS will most likely run the threads on two different cores, the state of the atomic variable will need to be synchronized between the two cores, which will incur a performance penalty. Then, we do this with thread pinning, using `BS::this_thread::set_os_thread_affinity()` to set the affinity of each thread to core 0 by passing a vector with `true` at index 0 and `false` at all other indices. In this case, the atomic variable will be kept in the local cache of core 0, which will increase performance.

**Warning:** Setting the affinity of threads in a pool is almost never a good idea! When you submit a task to a thread pool, you have no control over which thread it will actually run in. The main benefit of thread affinity is to reduce cache misses, but there is no way to guarantee that tasks accessing the same data will run on the same core if they are submitted to a pool. In fact, setting the affinity of the pool threads will almost certainly decrease performance, sometimes substantially, as the operating system's scheduler will be prevented from assigning threads to cores in the most optimal way. The most common use case for `BS::this_thread::set_os_thread_affinity()` is to set the affinity of individual threads created independently of any pool, for example using `std::thread`.

### Setting thread names

The thread pool's native extensions permit setting a thread's name using the operating system's native API. This can be useful for debugging, as the names of the threads will be visible in the debugger (for example, in the Call Stack on Visual Studio Code).

As with other features of the native extensions, the thread pool library provides an abstraction layer over the native APIs, consisting of the following two static member functions of the `BS::this_thread` class:

* `BS::this_thread::get_os_thread_name()` gets the current thread's name. It returns an object of type `std::optional<std::string>`. If the returned object does not contain a value, then the name could not be determined.
* `BS::this_thread::set_os_thread_name()` sets the current thread's name. It returns `true` if the name was set successfully, or `false` otherwise. Note that on Linux thread names are limited to 16 characters, including the null terminator.

This feature is illustrated by the following program:

```cpp
#define BS_THREAD_POOL_NATIVE_EXTENSIONS
#include "BS_thread_pool.hpp" // BS::synced_stream, BS::this_thread, BS::thread_pool
#include <cstddef>            // std::size_t
#include <optional>           // std::optional
#include <string>             // std::string, std::to_string

BS::synced_stream sync_out;

void set_name(const std::size_t idx)
{
    const std::string name = "Thread " + std::to_string(idx);
    const bool result = BS::this_thread::set_os_thread_name(name);
    sync_out.println(result ? "Successfully" : "Error: Failed to", " set the name of thread ", idx, " to '", name, "'.");
}

void get_name()
{
    const std::optional<std::string> result = BS::this_thread::get_os_thread_name();
    if (result)
        sync_out.println("This thread's name is set to '", *result, "'.");
    else
        sync_out.println("Error: Failed to get this thread's name!");
}

int main()
{
    const bool result = BS::this_thread::set_os_thread_name("Main Thread");
    sync_out.println(result ? "Successfully" : "Error: Failed to", " set the name of the main thread.");
    BS::thread_pool pool(4, set_name);
    pool.wait();
    // Place a breakpoint here to see the thread names in the debugger.
    pool.submit_task(get_name).wait();
}
```

If you place a breakpoint on the indicated line, you will be able to see the names of the threads in the debugger. The main thread will be named "Main Thread", while the 4 pool threads will be named "Thread 0" to "Thread 3". In the last line, a random thread's name will be read and printed out.

### Setting process priority

Although not directly related to multithreading, `BS::thread_pool`'s native extensions also provide the ability to set the entire process's priority using the operating system's native API. As with thread priority, the thread pool library provides an abstraction layer over the native APIs, in the form of the enumeration class `BS::os_process_priority`, which has the following 6 members:

* `BS::os_process_priority::idle`
* `BS::os_process_priority::below_normal`
* `BS::os_process_priority::normal`
* `BS::os_process_priority::above_normal`
* `BS::os_process_priority::high`
* `BS::os_process_priority::realtime`

On Windows, these pre-defined priorities map 1-to-1 with [the process priority classes defined by the Windows API](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setpriorityclass). On Linux and macOS, process priorities are mapped to ["nice" values](https://www.man7.org/linux/man-pages/man2/setpriority.2.html), as given by the actual values of the enumeration members (note that lower numbers correspond to higher priorities).

Process priority is managed using two functions:

* `BS::get_os_process_priority()` gets the process's priority. It returns an object of type `std::optional<BS::os_process_priority>`. If the returned object does not contain a value, then either the priority could not be determined, or it is not one of the pre-defined values listed above.
* `BS::set_os_process_priority()` sets the process's priority. It returns `true` if the priority was set successfully, or `false` otherwise. Usually, `false` means that the user does not have the necessary permissions to set the desired priority.

This is demonstrated by the following program:

```cpp
#define BS_THREAD_POOL_NATIVE_EXTENSIONS
#include "BS_thread_pool.hpp" // BS::get_os_process_priority, BS::os_process_priority, BS::set_os_process_priority, BS::synced_stream
#include <map>                // std::map
#include <optional>           // std::optional
#include <string>             // std::string

BS::synced_stream sync_out;
BS::os_process_priority target = BS::os_process_priority::high;

const std::map<BS::os_process_priority, std::string> os_process_priority_map = {{BS::os_process_priority::idle, "idle"}, {BS::os_process_priority::below_normal, "below_normal"}, {BS::os_process_priority::normal, "normal"}, {BS::os_process_priority::above_normal, "above_normal"}, {BS::os_process_priority::high, "high"}, {BS::os_process_priority::realtime, "realtime"}};

std::string os_process_priority_name(const BS::os_process_priority priority)
{
    const std::map<BS::os_process_priority, std::string>::const_iterator it = os_process_priority_map.find(priority);
    return (it != os_process_priority_map.end()) ? it->second : "unknown";
}

int main()
{
    const std::optional<BS::os_process_priority> get_result = BS::get_os_process_priority();
    if (get_result)
        sync_out.println("The OS process priority is currently set to '", os_process_priority_name(*get_result), "'.");
    else
        sync_out.println("Error: Failed to get the OS process priority!");
    const bool set_result = BS::set_os_process_priority(target);
    sync_out.println(set_result ? "Successfully" : "Error: Failed to", " set the OS process priority to '", os_process_priority_name(target), "'.");
}
```

On Linux or macOS, please ensure that you run this example as root using `sudo`, otherwise it will fail. (Note that here we didn't actually need to use `BS::synced_stream`, since we are not using the thread pool, and only the main thread prints to the stream; we used it only for consistency with other examples.)

### Setting process affinity

The thread pool's native extensions also allow the user to set the entire process's processor affinity using the operating system's native API. This works on Windows and Linux, but not on macOS, as the native API does not allow it. As with thread affinity, the thread pool library provides an abstraction layer over the native APIs, in the form of an `std::vector<bool>` where each element corresponds to a logical processor.

Process affinity is managed using two functions:

* `BS::this_thread::get_os_process_affinity()` gets the process's affinity. It returns an object of type `std::optional<std::vector<bool>>`. If the returned object does not contain a value, then the affinity could not be determined. On macOS, this function always returns `std::nullopt`.
* `BS::this_thread::set_os_process_affinity()` sets the process's affinity. It returns `true` if the affinity was set successfully, or `false` otherwise. On macOS, this function always returns `false`.

### Accessing native thread handles

If the native extensions are enabled, the `BS::thread_pool` class gains the member function `get_native_handles()`, which returns a vector containing the underlying implementation-defined thread handles for each of the pool's threads. These can then be used in an implementation-specific way to manage the threads at the OS level.

Here is a quick example:

```cpp
#define BS_THREAD_POOL_NATIVE_EXTENSIONS
#include "BS_thread_pool.hpp" // BS::synced_stream, BS::thread_pool
#include <thread>             // std::thread
#include <vector>             // std::vector

BS::synced_stream sync_out;
BS::thread_pool pool(4);

int main()
{
    std::vector<std::thread::native_handle_type> handles = pool.get_native_handles();
    for (std::size_t i = 0; i < handles.size(); ++i)
        sync_out.println("Thread ", i, " native handle: ", handles[i]);
}
```

The output will depend on your compiler and operating system. Here is an example:

```none
Thread 0 native handle: 00000000000000AC
Thread 1 native handle: 00000000000000B0
Thread 2 native handle: 00000000000000B4
Thread 3 native handle: 00000000000000B8
```

**Warning:** Please note that any code written using the native handles directly will **not** be portable. As detailed above, the thread pool's native extensions define abstraction layers for several commonly used thread operations, which are portable on supported platforms, and are therefore strongly preferred over non-portable operations. The native handles are made available for users who need to perform operations that are not covered by these abstraction layers.

## Testing the library

### Automated tests

The file `BS_thread_pool_test.cpp` in the `tests` folder of [the GitHub repository](https://github.com/bshoshany/thread-pool) will perform automated tests of all aspects of the library. In addition, the code is meant to serve as an extensive example of how to properly use the library.

The test program also takes the following command line arguments:

* `help`: Show a help message and exit. Any other arguments will be ignored.
* `stdout`: Print to the standard output.
* `log`: Print to a log file. It will have the same name as the executable, with a suffix `-yyyy-mm-dd_hh.mm.ss.log` based on the current date and time.
* `tests`: Perform standard tests.
* `deadlock`: Perform long deadlock tests.
* `benchmarks`: Perform full Mandelbrot plot benchmarks.
* `plot`: Perform quick Mandelbrot plot benchmarks.
* `save`: Save the Mandelbrot plot to a file.

If no options are entered, the default is `benchmarks log stdout tests`. If the file `default_args.txt` exists in the same folder, the test program reads the default arguments from it (space separated in a single line). Command line arguments can still override these defaults. This is useful when debugging.

The following macros can be defined during compilation (using the `-D` flag in Clang and GCC or `/D` in MSVC) to enable additional features:

* `BS_THREAD_POOL_TEST_IMPORT_MODULE`: Import the thread pool library [as a C&plus;&plus;20 module](#importing-the-library-as-a-c20-module). Note that the module must be compiled beforehand, as explained in the relevant section.
* `BS_THREAD_POOL_NATIVE_EXTENSIONS`: Test the [native extensions](#native-extensions). If importing the library as a C&plus;&plus;20 module, ensure that the library was compiled with the same macro.

A Python script, `test_all.py`, is provided for convenience in the `scripts` folder. This script makes use of the bundled [`compile_cpp.py` script](#the-compile_cpppy-script), and requires Python 3.12 or later. The script will automatically detect if Clang, GCC, and/or MSVC are available, and compile and run the test program using each available compiler 3 times:

1. With C&plus;&plus;17 support.
2. With C&plus;&plus;20 support, using `import BS.thread_pool`.
3. With C&plus;&plus;23 support, using `import BS.thread_pool`, and using `import std` on supported compilers.

If any of the tests fail, please [submit a bug report](https://github.com/bshoshany/thread-pool/issues) including the exact specifications of your system (OS, CPU, compiler, etc.) and the generated log file. However, please note that only the latest versions of each compiler are supported.

### Performance tests

`BS_thread_pool_test.cpp` also performs benchmarks, using a highly-optimized multithreaded algorithm which generates a plot of the [Mandelbrot set](https://en.wikipedia.org/wiki/Mandelbrot_set), utilizing a normalized iteration count algorithm and linear interpolation to create smooth coloring. If tests are enabled, the benchmarks will only be performed if all of the tests pass.

These benchmarks are heavily CPU-intensive, which results in a high speedup factor due to multithreading, ideally utilizing every core and thread to their fullest extent. This makes them useful for optimizing the library, since they are more sensitive to the thread pool's own performance than to other factors such as memory or cache.

The full benchmarks are enabled using the command line argument `benchmarks`, which is enabled by default. The command line argument `plot` can be used to just plot the Mandelbrot set once, either instead of or in addition to doing the full benchmarks. This will plot the largest possible image that can be plotted in 5 seconds, and only measure the performance in pixels/ms for the entire plot.

If you want to see the actual plot, pass the `save` command line argument. The plot is saved to a BMP file, to avoid having to depend on 3rd-party libraries. This is off by default, since that file can get quite large.

The program determines the optimal resolution of the Mandelbrot plot by testing how many pixels are needed to reach a certain target duration when parallelizing the loop using a number of tasks equal to the number of threads. This ensures that the benchmarks take approximately the same amount of time (per thread) on all systems, and are thus more consistent and portable.

Once the appropriate resolution has been determined, the program plots the Mandelbrot set. For more details about the algorithm used, please see the source code for `BS_thread_pool_test.cpp`. This operation is performed both single-threaded and multithreaded, with the multithreaded computation spread across multiple tasks submitted to the pool.

Multithreaded tests are performed with increasingly higher task counts, while keeping the number of threads in the pool equal to the hardware concurrency for optimal performance. Each test is repeated multiple times, with the run times averaged over all runs of the same test. The program keeps increasing the number of tasks by a factor of 2 until diminishing returns are encountered. The run times of the tests are compared, and the maximum speedup obtained compared to the single-threaded test is calculated.

If the [native extensions](#native-extensions) are enabled, the program will try to increase the priority of both the process itself and all the threads in the pool to the highest possible value, to prevent other processes from interfering with the benchmarks. Therefore, to obtain the most reliable benchmarks, it is recommended to run the tests as a privileged user, especially on Linux or macOS where only root can increase the priority.

As an example, here are the results of the benchmarks running on a 24-core (8P+16E) / 32-thread Intel i9-13900K CPU. The tests were compiled using MSVC in C&plus;&plus;23 mode, to obtain maximum performance using the latest C&plus;&plus;23 features. Compiler optimizations were enabled using the `/O2` flag. The benchmarks were run 5 times, and the result with the median speedup was as follows:

```none
Generating a 3965x3965 plot of the Mandelbrot set...
Each test will be repeated 30 times to collect reliable statistics.
   1 task:  [..............................]  (single-threaded)
-> Mean:  510.5 ms, standard deviation:  0.5 ms, speed:  1026.5 pixels/ms.
   8 tasks: [..............................]
-> Mean:  149.1 ms, standard deviation:  0.6 ms, speed:  3514.7 pixels/ms.
  16 tasks: [..............................]
-> Mean:   85.4 ms, standard deviation:  2.5 ms, speed:  6133.9 pixels/ms.
  32 tasks: [..............................]
-> Mean:   48.3 ms, standard deviation:  1.8 ms, speed: 10849.7 pixels/ms.
  64 tasks: [..............................]
-> Mean:   29.1 ms, standard deviation:  1.0 ms, speed: 17987.7 pixels/ms.
 128 tasks: [..............................]
-> Mean:   23.6 ms, standard deviation:  0.7 ms, speed: 22173.8 pixels/ms.
 256 tasks: [..............................]
-> Mean:   22.5 ms, standard deviation:  0.6 ms, speed: 23325.3 pixels/ms.
 512 tasks: [..............................]
-> Mean:   21.8 ms, standard deviation:  0.5 ms, speed: 24075.4 pixels/ms.
1024 tasks: [..............................]
-> Mean:   21.9 ms, standard deviation:  0.7 ms, speed: 23892.4 pixels/ms.
Maximum speedup obtained by multithreading vs. single-threading: 23.5x, using 512 tasks.
```

This CPU has 24 cores, of which 8 are fast (5.40 GHz max) performance cores with hyperthreading (thus providing 16 threads in total), and 16 are slower (4.30 GHz max) efficiency cores without hyperthreading, for a total of 32 threads.

Due to the hybrid architecture, it is not trivial to calculate the theoretical maximum speedup. However, we can get a rough estimate by noticing that the E-cores are about 20% slower than the P-cores, and that hyperthreading is generally known to provide around a 30% speedup. Thus, the estimated theoretical speedup (compared to a single P-core) is 8 &times; 1.3 + 16 &times; 0.8 = 23.2x.

The actual median speedup obtained, 23.5x, is slightly above this estimate, which indicates that the thread pool provides optimal performance and allows the Mandelbrot plot algorithm to take full advantage of the CPU's capabilities.

It should also be noted that even though the available number of hardware threads is 32, the maximum possible speedup is achieved not with 32 tasks, but with 512 tasks - half the square of the number of hardware threads. The reason for this is that splitting the job into more tasks than threads eliminates thread idle time, as explained [above](#optimizing-the-number-of-blocks). However, at 1024 tasks we encounter diminishing returns, as the overhead of submitting the tasks to the pool starts to outweigh the benefits of parallelization.

### Finding the version of the library

Starting with v5.0.0, the thread pool library defines the `constexpr` object `BS::thread_pool_version`, which can be used to check the version of the library at compilation time. This object is of type `BS::version`, with members `major`, `minor`, and `patch`, and all comparison operators defined as `constexpr`. It also has a `to_string()` member function and an `operator<<` overload for easy printing at runtime.

Since `BS::thread_pool_version` is a `constexpr` object, it can be used in any context where a `constexpr` object is allowed, such as `static_assert()` and `if constexpr`. For example, the following program will fail to compile if the version is not 5.1.0 or higher:

```cpp
#include "BS_thread_pool.hpp"

static_assert(BS::thread_pool_version >= BS::version(5, 1, 0), "This program requires version 5.1.0 or later of the BS::thread_pool library.");

int main()
{
    // ...
}
```

As another example, the following program will print the version of the library (this will implicitly use the `<<` operator of `BS::version`) and then conditionally compile one of two branches of code depending on the version of the library:

```cpp
#include "BS_thread_pool.hpp" // BS::synced_stream, BS::thread_pool

BS::synced_stream sync_out;
BS::thread_pool pool;

int main()
{
    sync_out.println("Detected BS::thread_pool v", BS::thread_pool_version, '.');
    if constexpr (BS::thread_pool_version <= BS::version(5, 1, 0))
    {
        // Do something supported by BS::thread_pool v5.1.0 or earlier.
    }
    else
    {
        // Do something supported by newer versions of BS::thread_pool after v5.1.0.
    }
}
```

Currently, both the examples above are of pedagogical value only, because `BS::thread_pool_version` was only introduced in v5.0.0, and that is also the latest version at the time of writing, so there are no other versions to compare to. However, once future versions of the library are released, this object will be the preferred way to do version checking.

For backwards compatibility, if you are not sure if you are going to get v4 or v5 of the library, you can check the version using the following preprocessor macros, which were introduced in v4.0.1:

* `BS_THREAD_POOL_VERSION_MAJOR` - indicates the major version.
* `BS_THREAD_POOL_VERSION_MINOR` - indicates the minor version.
* `BS_THREAD_POOL_VERSION_PATCH` - indicates the patch version.

These macros allow for conditional inclusion of code using `#if` directives. As an example, the member function [`set_cleanup_func()`](#thread-cleanup-functions) was introduced in v5.0.0. Therefore, if the major version number is 5 or higher, we can use this function; otherwise, we must find some other way to do the cleanup:

 ```cpp
#include "BS_thread_pool.hpp" // BS::synced_stream, BS::thread_pool

BS::synced_stream sync_out;
BS::thread_pool pool;

int main()
{
#if BS_THREAD_POOL_VERSION_MAJOR >= 5
    pool.set_cleanup_func(
        []
        {
            sync_out.println("Doing cleanup...");
        });
#else
    // Do the cleanup in some other way.
#endif
}
```

However, please note that if the library is imported [as a C&plus;&plus;20 module](#importing-the-library-as-a-c20-module), these macros will not be available, since macros cannot be exported from a module. In this case, you must use `BS::thread_pool_version` instead. (Indeed, this is exactly why it was introduced in the first place.)

## Importing the library as a C&plus;&plus;20 module

### Compiling the module

If C&plus;&plus;20 features are available, the library can be imported as a C&plus;&plus;20 module using `import BS.thread_pool`. This is the officially recommended way to use the library, as it has many benefits, such as faster compilation times, better encapsulation, no namespace pollution, no include order issues, easier maintainability, simpler dependency management, and more. The `constexpr` flag `BS::thread_pool_module` indicates whether the thread pool library was compiled as a module. For more information on C&plus;&plus;20 modules, please see [cppreference.com](https://en.cppreference.com/w/cpp/language/modules).

The module file itself is `BS.thread_pool.cppm`, located in the `modules` folder, and it is just a thin wrapper around the header file `BS_thread_pool.hpp`. The C&plus;&plus;20 standard does not provide a way for one file to be used as both a module and a header file, so both files are needed in order to compile the library as a module. (However, to use the library as a header file, only `BS_thread_pool.hpp` is needed.)

Note that the header file `BS_thread_pool.hpp` has an underscore `_` following `BS`, for backwards compatibility with older versions of the library. However, the module file `BS.thread_pool.cppm` has a dot `.` following `BS`, to conform with the C&plus;&plus;20 module naming convention, where dots represent hierarchy; all modules written by the author of this library will use the `BS.` prefix.

This feature has been tested with the latest versions of Clang, GCC, and MSVC. Unfortunately, at the time of writing, C&plus;&plus;20 modules are still not fully implemented in all compilers, and each compiler implements them differently.

The easiest way to compile the module itself, as well as any programs that import it, is using the `compile_cpp.py` Python script provided in [the GitHub repository](https://github.com/bshoshany/thread-pool), which will automatically figure out the appropriate flags for each compiler. Please see the [next section](#compiling-with-compile_cpppy-using-import-bsthread_pool) for more information.

However, if you prefer to compile manually, the module must first be compiled into a binary file, in a format specific to each compiler, as described in the following sections. Once it is compiled once and for all, this binary file (plus an object file, in MSVC) is the only file needed to import the library; the `.cppm` and `.hpp` files are no longer needed. However, any program using the module must be compiled with a flag indicating to the compiler where to find that binary file.

Once the module is compiled, it can be imported using `import BS.thread_pool`. In all the examples above, you can simply replace `#include "BS_thread_pool.hpp"` with `import BS.thread_pool;` in order to import the library as a module. The only exception is the [native extensions](#native-extensions), which are enabled in the examples using a macro; as explained in that section, the macro must be defined as a compiler flag, as modules cannot access macros defined in the program that imported them.

Here is a quick example:

```cpp
import BS.thread_pool;

BS::synced_stream sync_out;
BS::thread_pool pool;

int main()
{
    pool.submit_task(
            []
            {
                sync_out.println("Thread pool library successfully imported using C++20 modules!");
            })
        .wait();
}
```

Below we will provide the commands for compiling the library as a module and then compiling [the test program](#testing-the-library) `BS_thread_pool_test.cpp` using this module, with Clang, GCC, and MSVC, as well as with CMake. In [the GitHub repository](https://github.com/bshoshany/thread-pool), the relevant files are organized as follows:

```
├── README.md                     <- this documentation file
├── include
│   └── BS_thread_pool.hpp        <- the header file
├── modules
│   └── BS.thread_pool.cppm       <- the module file
├── tasks
│   └── compile_cpp.py            <- the compile script (optional)
└── tests
    └── BS_thread_pool_test.cpp   <- the test program
```

In the following examples, it is assumed that the commands are executed in the root directory of the repository (the one that contains `README.md`). The compiled files will be placed in a `build` subdirectory, which should be created beforehand.

### Compiling with `compile_cpp.py` using `import BS.thread_pool`

The bundled Python script [`compile_cpp.py`](#the-compile_cpppy-script) can be used to easily compile any programs that import the library as a module. The script will automatically figure out the appropriate flags for each compiler, so you do not have to worry about the details. For example, to compile the test program `BS_thread_pool_test.cpp` and have it import the `BS.thread_pool` module, simply run the following command in the root folder of the repository:

```bash
python scripts/compile_cpp.py tests/BS_thread_pool_test.cpp -s=c++20 -i=include -t=release -m="BS.thread_pool=modules/BS.thread_pool.cppm,include/BS_thread_pool.hpp" -o=build/BS_thread_pool_test -d=BS_THREAD_POOL_TEST_IMPORT_MODULE -v
```

Please see [below](#the-compile_cpppy-script) for an explanation of the command line arguments. The `-d` argument defines the macro `BS_THREAD_POOL_TEST_IMPORT_MODULE`, which is used to indicate to the test program that it needs to import the library as a module instead of including the header file. **Note that this macro is only used by the test program; it is not needed when you compile your own programs.** To enable the [native extensions](#native-extensions), you should also add `-d=BS_THREAD_POOL_NATIVE_EXTENSIONS` to define the required macro. To use C&plus;&plus;23, replace `-s=c++20` with `-s=c++23`.

Since we used `-t=release`, optimization flags will be added automatically. If you now type `build/BS_thread_pool_test`, the test program will run; you can also add the argument `-r` to run it automatically after compilation. If the module was successfully imported, the test program will print the message:

```none
Thread pool library imported using: import BS.thread_pool (C++20 modules).
```

For further customization, it is recommend to create a `compile_cpp.yaml` file as explained [below](#the-compile_cpppy-script).

### Compiling with Clang using `import BS.thread_pool`

Note: The following instructions have only been tested using Clang v19.1.6, the latest version at the time of writing, and may not work with older versions of the compiler.

To compile the module file `BS.thread_pool.cppm` with Clang, first create the `build` folder using `mkdir build`, and then run the following command in the root folder of the repository:

```bash
clang++ modules/BS.thread_pool.cppm --precompile -std=c++20 -I include -o build/BS.thread_pool.pcm
```

Here is a breakdown of the compiler arguments:

* `modules/BS.thread_pool.cppm`: The module file to compile. Note that it will include the file `include/BS_thread_pool.hpp` automatically.
* `--precompile`: Do not run the linker, only compile the module.
* `-std=c++20`: Use the C&plus;&plus;20 standard. For C&plus;&plus;23, use `-std=c++23`.
* `-I include`: Add the `include` folder to the include path, so that the module can find the header file `BS_thread_pool.hpp`.
* `-o build/BS.thread_pool.pcm`: Output the compiled module to `build/BS.thread_pool.pcm`. The extension `.pcm` is used by Clang for precompiled modules.

Note that to enable the [native extensions](#native-extensions), you should add `-D BS_THREAD_POOL_NATIVE_EXTENSIONS` to define the required macro.

Once the module is compiled, you can compile the test program as follows:

```bash
clang++ tests/BS_thread_pool_test.cpp -fmodule-file="BS.thread_pool=build/BS.thread_pool.pcm" -std=c++20 -o build/BS_thread_pool_test -D BS_THREAD_POOL_TEST_IMPORT_MODULE
```

Here is a breakdown of the compiler arguments:

* `tests/BS_thread_pool_test.cpp`: The program to compile.
* `-fmodule-file="BS.thread_pool=build/BS.thread_pool.pcm"`: Specify that the module `BS.thread_pool` is located in the file `build/BS.thread_pool.pcm`.
* `-std=c++20`: Same as above.
* `-o build/BS_thread_pool_test`: Output the compiled program to `build/BS_thread_pool_test` (or `build/BS_thread_pool_test.exe` on Windows).
* `-D BS_THREAD_POOL_TEST_IMPORT_MODULE`: Define the macro `BS_THREAD_POOL_TEST_IMPORT_MODULE`, which is used to indicate to the test program that it needs to import the library as a module instead of including the header file. **Note that this macro is only used by the test program; it is not needed when you compile your own programs.**

Again, you should add `-D BS_THREAD_POOL_NATIVE_EXTENSIONS` if you wish to test the native extensions. You do not need to use the `-I` flag, since the header file is not needed, only the `.pcm` file. If you now type `build/BS_thread_pool_test`, the test program will run. If the module was successfully imported, the test program will print the message:

```none
Thread pool library imported using: import BS.thread_pool (C++20 modules).
```

Of course, you should add warning, debugging, optimization, and other compiler flags to the commands above as needed. For more information about using C&plus;&plus;20 modules with Clang, please see [the official documentation](https://clang.llvm.org/docs/StandardCPlusPlusModules.html).

**Note:** On macOS, Apple Clang v16.0.0 (the latest version at the time of writing) does not support C&plus;&plus;20 modules. Please either install the latest version of LLVM Clang using [Homebrew](https://formulae.brew.sh/formula/llvm), or include the library as a header file.

### Compiling with GCC using `import BS.thread_pool`

Note: The following instructions have only been tested using GCC v14.2.0, the latest version at the time of writing, and may not work with older versions of the compiler.

To compile the module file `BS.thread_pool.cppm` with GCC, first create the `build` folder using `mkdir build`, and then run the following command in the root folder of the repository:

```bash
g++ -x c++ modules/BS.thread_pool.cppm -c "-fmodule-mapper=|@g++-mapper-server -r build" -fmodule-only -fmodules-ts -std=c++20 -I include
```

Here is a breakdown of the compiler arguments:

* `-x c++`: Treat the input file as a C&plus;&plus; file. This is necessary because the file has the `.cppm` extension, which is not recognized by GCC.
* `modules/BS.thread_pool.cppm`: The module file to compile. Note that it will include the file `include/BS_thread_pool.hpp` automatically.
* `-c`: Do not run the linker, only compile the module.
* `"-fmodule-mapper=|@g++-mapper-server -r build"`: Specify to the module mapper that the compiled module should be placed in the `build` folder. This will create a file `build/BS.thread_pool.gcm`. The extension `.gcm` is used by GCC for compiled modules.
* `-fmodule-only`: Do not create an object file for the module.
* `-fmodules-ts`: Enable C&plus;&plus;20 modules.
* `-std=c++20`: Use the C&plus;&plus;20 standard. For C&plus;&plus;23, use `-std=c++23`.
* `-I include`: Add the `include` folder to the include path, so that the module can find the header file `BS_thread_pool.hpp`.

Note that to enable the [native extensions](#native-extensions), you should add `-D BS_THREAD_POOL_NATIVE_EXTENSIONS` to define the required macro.

Once the module is compiled, you can compile the test program as follows:

```bash
g++ tests/BS_thread_pool_test.cpp "-fmodule-mapper=|@g++-mapper-server -r build" -fmodules-ts -std=c++20 -o build/BS_thread_pool_test -D BS_THREAD_POOL_TEST_IMPORT_MODULE
```

Here is a breakdown of the compiler arguments:

* `tests/BS_thread_pool_test.cpp`: The program to compile.
* `"-fmodule-mapper=|@g++-mapper-server -r build"`: Specify to the module mapper that the compiled module can be found in the `build` folder. It will look for the file `build/BS.thread_pool.gcm`.
* `-fmodules-ts`, `-std=c++20`: Same as above.
* `-o build/BS_thread_pool_test`: Output the compiled program to `build/BS_thread_pool_test` (or `build/BS_thread_pool_test.exe` on Windows).
* `-D BS_THREAD_POOL_TEST_IMPORT_MODULE`: Define the macro `BS_THREAD_POOL_TEST_IMPORT_MODULE`, which is used to indicate to the test program that it needs to import the library as a module instead of including the header file. **Note that this macro is only used by the test program; it is not needed when you compile your own programs.**

Again, you should add `-D BS_THREAD_POOL_NATIVE_EXTENSIONS` if you wish to test the native extensions. You do not need to use the `-I` flag, since the header file is not needed, only the `.gcm` file. If you now type `build/BS_thread_pool_test`, the test program will run. If the module was successfully imported, the test program will print the message:

```none
Thread pool library imported using: import BS.thread_pool (C++20 modules).
```

Of course, you should add warning, debugging, optimization, and other compiler flags to the commands above as needed. For more information about using C&plus;&plus;20 modules with GCC, please see [the official documentation](https://gcc.gnu.org/onlinedocs/gcc/C_002b_002b-Modules.html).

**Note:** GCC v14.2.0 (latest version at the time of writing) appears to have an internal compiler error when compiling programs containing modules (or at least, this particular module) with any optimization flags other than `-Og` enabled. Until this is fixed, if you wish to use compiler optimizations, please either include the library as a header file or use a different compiler.

### Compiling with MSVC using `import BS.thread_pool`

Note: The following instructions have only been tested using MSVC v19.42.34435, the latest version at the time of writing, and may not work with older versions of the compiler.

To compile the module file `BS.thread_pool.cppm` with MSVC, first open the Visual Studio Developer PowerShell for the appropriate CPU architecture. For example, for x64, execute the following command in PowerShell:

```pwsh
& 'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\Launch-VsDevShell.ps1' -Arch amd64 -HostArch amd64
```

For ARM64, replace `amd64` with `arm64`. (Do not use the "Developer PowerShell for VS 2022" Start Menu shortcut, as it may not use the correct CPU architecture by default.)

Navigate to the repository folder, create the `build` folder using `mkdir build`, and then run the following command in the root folder of the repository:

```pwsh
cl modules/BS.thread_pool.cppm /c /EHsc /interface /nologo /permissive- /std:c++20 /TP /Zc:__cplusplus /I include /ifcOutput build/BS.thread_pool.ifc /Fo:build/BS.thread_pool.obj
```

Here is a breakdown of the compiler arguments:

* `modules/BS.thread_pool.cppm`: The module file to compile. Note that it will include the file `include/BS_thread_pool.hpp` automatically.
* `/c`: Do not run the linker, only compile the module.
* `/EHsc`: Enable C&plus;&plus; exceptions.
* `/interface`: Treat the input file as a module interface unit. This is needed because MSVC expects the `.ixx` extension for the module file, but this library uses the `.cppm` extension.
* `/nologo`: Do not display the compiler's banner.
* `/permissive-`: Disable permissive behaviors, that is, enforce strict C&plus;&plus; standard conformance.
* `/std:c++20`: Use the C&plus;&plus;20 standard. For C&plus;&plus;23, use `/std:c++latest`.
* `/TP`: Treat the input file as a C&plus;&plus; file. This is necessary because the file has the `.cppm` extension, which is not recognized by MSVC.
* `/Zc:__cplusplus`: Make the `__cplusplus` preprocessor macro correctly reflect the C&plus;&plus; standard being used.
* `/I include`: Add the `include` folder to the include path, so that the module can find the header file `BS_thread_pool.hpp`.
* `/ifcOutput build/BS.thread_pool.ifc`: Output the compiled module to `build/BS.thread_pool.ifc`. The extension `.ifc` is used by MSVC for module interface files.
* `/Fo:build/BS.thread_pool.obj`: Output the compiled object file to `build/BS.thread_pool.obj`.

Note that to enable the [native extensions](#native-extensions), you should add `/D BS_THREAD_POOL_NATIVE_EXTENSIONS` to define the required macro.

Once the module is compiled, you can compile the test program as follows:

```pwsh
cl tests/BS_thread_pool_test.cpp build/BS.thread_pool.obj /reference BS.thread_pool=build/BS.thread_pool.ifc /EHsc /nologo /permissive- /std:c++20 /Zc:__cplusplus /Fo:build/BS_thread_pool_test.obj /Fe:build/BS_thread_pool_test.exe /D BS_THREAD_POOL_TEST_IMPORT_MODULE
```

Here is a breakdown of the compiler arguments:

* `tests/BS_thread_pool_test.cpp`: The program to compile.
* `build/BS.thread_pool.obj`: The module object file to link to the program.
* `/reference BS.thread_pool=build/BS.thread_pool.ifc`: Specify that the module `BS.thread_pool` is located in the file `build/BS.thread_pool.ifc`.
* `/EHsc`, `/nologo`, `/permissive-`, `/std:c++20`, `/Zc:__cplusplus`: Same as above.
* `/Fo:build/BS_thread_pool_test.obj`: Output the compiled object file to `build/BS_thread_pool_test.obj`.
* `/Fe:build/BS_thread_pool_test.exe`: Output the compiled program to `build/BS_thread_pool_test.exe`.
* `/D BS_THREAD_POOL_TEST_IMPORT_MODULE`: Define the macro `BS_THREAD_POOL_TEST_IMPORT_MODULE`, which is used to indicate to the test program that it needs to import the library as a module instead of including the header file. **Note that this macro is only used by the test program; it is not needed when you compile your own programs.**

Again, you should add `/D BS_THREAD_POOL_NATIVE_EXTENSIONS` if you wish to test the native extensions. You do not need to use the `/I` flag, since the header file is not needed, only the `.obj` and `.ifc` files. If you now type `build/BS_thread_pool_test`, the test program will run. If the module was successfully imported, the test program will print the message:

```none
Thread pool library imported using: import BS.thread_pool (C++20 modules).
```

Of course, you should add warning, debugging, optimization, and other compiler flags to the commands above as needed. For more information about using C&plus;&plus;20 modules with MSVC, please see [this blog post](https://devblogs.microsoft.com/cppblog/using-cpp-modules-in-msvc-from-the-command-line-part-1/).

### Compiling with CMake using `import BS.thread_pool`

Note: The following instructions have only been tested using CMake v3.31.2, the latest version at the time of writing, and may not work with older versions. Also, modules are currently only supported by CMake with the [`Ninja`](https://ninja-build.org/) and `Visual Studio 17 2022` generators.

If you are using [CMake](https://cmake.org/), you can use `target_sources()` with `CXX_MODULES` to include the module file `BS.thread_pool.cppm`. CMake will then automatically compile the module and link it to your program. Here is an example of a `CMakeLists.txt` file that can be used to build the test program and import the thread pool library as a module:

```cmake
cmake_minimum_required(VERSION 3.31)
project(BS_thread_pool_test LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/permissive- /Zc:__cplusplus)
endif()

add_library(BS_thread_pool)
target_sources(BS_thread_pool PRIVATE FILE_SET CXX_MODULES FILES modules/BS.thread_pool.cppm)
target_include_directories(BS_thread_pool PRIVATE include)

add_executable(${PROJECT_NAME} tests/BS_thread_pool_test.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE BS_thread_pool)
target_compile_definitions(${PROJECT_NAME} PRIVATE BS_THREAD_POOL_TEST_IMPORT_MODULE)
```

Note that for MSVC we have to add the `/permissive-` flag to enforce strict C&plus;&plus; standard conformance, otherwise the test program will not compile, and `/Zc:__cplusplus`, otherwise the test program cannot detect the correct C&plus;&plus; version. This is handled automatically by the `if(MSVC)` block.

To enable the [native extensions](#native-extensions), add the line `add_compile_definitions(BS_THREAD_POOL_NATIVE_EXTENSIONS)`. Replace `CMAKE_CXX_STANDARD 20` with `23` if you wish to use C&plus;&plus;23 features.

Place this file in the root folder of the repository, and then run the following commands:

```bash
cmake -B build
cmake --build build
build/BS_thread_pool_test
```

For MSVC, replace the last command with `build/Debug/BS_thread_pool_test`. If the module was successfully imported, the test program will print the message:

```none
Thread pool library imported using: import BS.thread_pool (C++20 modules).
```

Of course, you should add warning, debugging, optimization, and other compiler flags to the configuration above as needed. For more information about using C&plus;&plus;20 modules with CMake, please see [the official documentation](https://cmake.org/cmake/help/latest/manual/cmake-cxxmodules.7.html).

You can also instruct CMake to download the library automatically from the GitHub repository, as explained below, either using [CPM](#installing-using-cmake-with-cpm) or [`FetchContent`](#installing-using-cmake-with-fetchcontent).

## Importing the C&plus;&plus;23 Standard Library as a module

### Enabling `import std`

If C&plus;&plus;23 features are available, the thread pool library can import the C&plus;&plus; Standard Library as a module using `import std`. This has the same benefits described [above](#importing-the-library-as-a-c20-module) for importing the library as a module, such as faster compilation times. To enable this feature, define the macro `BS_THREAD_POOL_IMPORT_STD` at compilation time.

At the time of writing, importing the C&plus;&plus; Standard Library as a module is only officially supported by the following combinations of compilers and standard libraries:

* Recent versions of MSVC with Microsoft STL.
* Recent versions of LLVM Clang (**not** Apple Clang) with LLVM libc&plus;&plus;.

It is not supported by GCC with any standard library, Clang with any standard library other than libc&plus;&plus;, any compiler with GNU libstdc&plus;&plus;, or any other compiler or standard library.

If `BS_THREAD_POOL_IMPORT_STD` is defined, then you must also import the thread pool library itself as a module. If the library is included as a header file, this will force the program that included the header file to also import `std`, which is not desirable and can lead to compilation errors if the program `#include`s any Standard Library header files.

Defining the macro before importing the module will not work, as modules cannot access macros defined in the program that imported them. Instead, you must define the macro as a compiler flag: `-D BS_THREAD_POOL_IMPORT_STD` for Clang and GCC or `/D BS_THREAD_POOL_IMPORT_STD` for MSVC.

[The test program](#testing-the-library) will also import the `std` module if the macro `BS_THREAD_POOL_IMPORT_STD` is defined at compilation time. In that case, you should also enable the macro `BS_THREAD_POOL_TEST_IMPORT_MODULE` to import the thread pool library as a module.

The `constexpr` flag `BS::thread_pool_import_std` indicates whether the thread pool library was compiled with `import std`. Note that the flag will be `false` if `BS_THREAD_POOL_IMPORT_STD` is defined but the compiler or standard library does not support importing the C&plus;&plus; Standard Library as a module.

At the time of writing, importing the `std` module requires compiling it first. As explained in the [previous section](#importing-the-library-as-a-c20-module), using the bundled `compile_cpp.py` script is the easiest way to do this, as we show in the [next section](#compiling-with-compile_cpppy-using-import-std). However, for those who wish to compile manually, in the following sections we will explain how to do it with both Clang and MSVC, as well as with CMake. It is assumed that the reader has already read the section about importing the `BS.thread_pool` library as a module, so we omit some details here.

### Compiling with `compile_cpp.py` using `import std`

The bundled Python script [`compile_cpp.py`](#the-compile_cpppy-script) can be used to easily compile any programs that import the C&plus;&plus; Standard Library as a module. The script will automatically figure out the appropriate flags for each compiler, so you do not have to worry about the details. For example, to compile the test program `BS_thread_pool_test.cpp` and have it import both the `BS.thread_pool` module and the `std` module, simply run the following command in the root folder of the repository:

```bash
python scripts/compile_cpp.py tests/BS_thread_pool_test.cpp -s=c++23 -i=include -t=release -m="BS.thread_pool=modules/BS.thread_pool.cppm,include/BS_thread_pool.hpp" -o=build/BS_thread_pool_test -d=BS_THREAD_POOL_TEST_IMPORT_MODULE -d=BS_THREAD_POOL_IMPORT_STD -u=auto -v
```

Please see [below](#the-compile_cpppy-script) for an explanation of the command line arguments. The differences between this command and the one we used for [importing the thread pool library as a module](#compiling-with-compile_cpppy-using-import-bsthread_pool) are:

* Changed `-s=c++20` to `-s=c++23` so we can use the C&plus;&plus;23 standard.
* Added `-d=BS_THREAD_POOL_IMPORT_STD` to define the required macro.
* Added `-u=auto` to automatically detect the location of the `std` module. If this doesn't work, you will need to specify the path manually.

To enable the [native extensions](#native-extensions), you should also add `-d=BS_THREAD_POOL_NATIVE_EXTENSIONS` to define the required macro. If you now type `build/BS_thread_pool_test`, the test program will run. If the `std` module was successfully imported, the test program will print the message:

```none
C++ Standard Library imported using:
* Thread pool library: import std (C++23 std module).
* Test program: import std (C++23 std module).
```

For further customization, it is recommend to create a `compile_cpp.yaml` file as explained [below](#the-compile_cpppy-script).

### Compiling with Clang and LLVM libc&plus;&plus; using `import std`

Note: The following instructions have only been tested using Clang v19.1.6 and LLVM libc&plus;&plus; v19.1.6, the latest versions at the time of writing, and may not work with older versions.

Before compiling the `std` module, you must find the file `std.cppm`:

* On Windows, libc&plus;&plus; is most likely installed via [MSYS2](https://www.msys2.org/), so the `std` module should be at `C:\msys64\clang64\share\libc&plus;&plus;\v1\std.cppm`. If you did not install MSYS2 in `C:\msys64`, replace that with the correct path. If you installed libc&plus;&plus; without MSYS2, locate `std.cppm` manually in the installation folder.
* On Linux, the `std` module should be at `/usr/lib/llvm-<LLVM major version>/share/libc&plus;&plus;/v1/std.cppm`. Replace `<LLVM major version>` with the major version number of libc&plus;&plus;, e.g. `19`. If you installed libc&plus;&plus; in a different folder, locate `std.cppm` manually in that folder.
* On macOS, the `std` module should be at `/usr/local/Cellar/llvm/<LLVM full version>/share/libc&plus;&plus;/v1/std.cppm`. Replace `<LLVM full version>` with the full version number of libc&plus;&plus;, e.g. `19.1.6`. If you installed libc&plus;&plus; in a different folder, locate `std.cppm` manually in that folder.

To compile the module file `std.cppm` with Clang, first create the `build` folder using `mkdir build`, and then run the following command in the root folder of the repository:

```bash
clang++ "path to std.cppm" --precompile -std=c++23 -o build/std.pcm -Wno-reserved-module-identifier
```

Of course, you should replace `"path to std.cppm"` with the actual path. The compiler arguments are explained [above](#compiling-with-clang-using-import-bsthread_pool). The additional argument `-Wno-reserved-module-identifier` is needed to silence a false-positive warning.

Next, compile the `BS.thread_pool` module as [above](#compiling-with-clang-using-import-bsthread_pool), but with the following additional flags:

* `-fmodule-file="std=build/std.pcm"`: Specify that the module `std` is located in the file `build/std.pcm`.
* `-D BS_THREAD_POOL_IMPORT_STD`: Instruct the library to import the `std` module.

```bash
clang++ modules/BS.thread_pool.cppm --precompile -fmodule-file="std=build/std.pcm" -std=c++23 -I include -o build/BS.thread_pool.pcm -D BS_THREAD_POOL_IMPORT_STD
```

Add `-D BS_THREAD_POOL_NATIVE_EXTENSIONS` if you wish to enable the [native extensions](#native-extensions). Once the module is compiled, you can compile the test program as follows:

```bash
clang++ tests/BS_thread_pool_test.cpp -fmodule-file="std=build/std.pcm" -fmodule-file="BS.thread_pool=build/BS.thread_pool.pcm" -std=c++23 -o build/BS_thread_pool_test -D BS_THREAD_POOL_TEST_IMPORT_MODULE -D BS_THREAD_POOL_IMPORT_STD
```

Again, you should add `-D BS_THREAD_POOL_NATIVE_EXTENSIONS` if you wish to test the native extensions. If you now type `build/BS_thread_pool_test`, the test program will run. If the `std` module was successfully imported, the test program will print the message:

```none
C++ Standard Library imported using:
* Thread pool library: import std (C++23 std module).
* Test program: import std (C++23 std module).
```

### Compiling with MSVC and Microsoft STL using `import std`

Note: The following instructions have only been tested using MSVC v19.42.34435 and Microsoft STL v143 (202408), the latest versions at the time of writing, and may not work with older versions.

Before compiling the `std` module, you must find the file `std.ixx`. It should be located in the folder `C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\<MSVC runtime version>\modules`. Replace `<MSVC runtime version>` with the full version number of the MSVC runtime library, e.g. `14.42.34433`. If you installed Visual Studio in a different folder, locate `std.ixx` manually in that folder.

To compile the module file `std.ixx` with MSVC, first open the Visual Studio Developer PowerShell for the appropriate CPU architecture as explained [above](#compiling-with-msvc-using-import-bsthread_pool). Navigate to the repository folder, create the `build` folder using `mkdir build`, and then run the following command in the root folder of the repository:

```pwsh
cl "path to std.ixx" /c /EHsc /nologo /permissive- /std:c++latest /Zc:__cplusplus /ifcOutput build/std.ifc /Fo:build/std.obj
```

Of course, you should replace `"path to std.ixx"` with the actual path. The compiler arguments are explained [above](#compiling-with-msvc-using-import-bsthread_pool).

Next, compile the `BS.thread_pool` module as [above](#compiling-with-msvc-using-import-bsthread_pool), but with the following additional flags:

* `/reference std=build/std.ifc`: Specify that the module `std` is located in the file `build/std.ifc`.
* `/D BS_THREAD_POOL_IMPORT_STD`: Instruct the library to import the `std` module.

```pwsh
cl modules/BS.thread_pool.cppm /reference std=build/std.ifc /c /EHsc /interface /nologo /permissive- /std:c++latest /TP /Zc:__cplusplus /I include /ifcOutput build/BS.thread_pool.ifc /Fo:build/BS.thread_pool.obj /D BS_THREAD_POOL_IMPORT_STD
```

Add `/D BS_THREAD_POOL_NATIVE_EXTENSIONS` if you wish to enable the [native extensions](#native-extensions). Once the module is compiled, you can compile the test program as follows (note that we added `build/std.obj` to link with the `std` module):

```pwsh
cl tests/BS_thread_pool_test.cpp build/std.obj build/BS.thread_pool.obj /reference std=build/std.ifc /reference BS.thread_pool=build/BS.thread_pool.ifc /EHsc /nologo /permissive- /std:c++latest /Zc:__cplusplus /Fo:build/BS_thread_pool_test.obj /Fe:build/BS_thread_pool_test.exe /D BS_THREAD_POOL_TEST_IMPORT_MODULE /D BS_THREAD_POOL_IMPORT_STD
```

Again, you should add `/D BS_THREAD_POOL_NATIVE_EXTENSIONS` if you wish to test the native extensions. If you now type `build/BS_thread_pool_test`, the test program will run. If the `std` module was successfully imported, the test program will print the message:

```none
C++ Standard Library imported using:
* Thread pool library: import std (C++23 std module).
* Test program: import std (C++23 std module).
```

### Compiling with CMake using `import std`

Note: The following instructions have only been tested using CMake v3.31.2, the latest version at the time of writing, and may not work with older versions. Also, modules are currently only supported by CMake with the [`Ninja`](https://ninja-build.org/) and `Visual Studio 17 2022` generators.

If you are using [CMake](https://cmake.org/), you can enable `CMAKE_EXPERIMENTAL_CXX_IMPORT_STD` to automatically compile the `std` module, provided the compiler and standard library support it. Here is an example of a `CMakeLists.txt` file that can be used to build the test program, import the thread pool library as a module, and import the C&plus;&plus; Standard Library as a module:

```cmake
cmake_minimum_required(VERSION 3.31)
project(BS_thread_pool_test LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD ON)

add_compile_definitions(BS_THREAD_POOL_IMPORT_STD)

if(MSVC)
    add_compile_options(/permissive- /Zc:__cplusplus)
endif()

add_library(BS_thread_pool)
target_sources(BS_thread_pool PRIVATE FILE_SET CXX_MODULES FILES modules/BS.thread_pool.cppm)
target_include_directories(BS_thread_pool PRIVATE include)

add_executable(${PROJECT_NAME} tests/BS_thread_pool_test.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE BS_thread_pool)
target_compile_definitions(${PROJECT_NAME} PRIVATE BS_THREAD_POOL_TEST_IMPORT_MODULE)
```

The `if(MSVC)` block is explained [above](#compiling-with-msvc-using-import-bsthread_pool). To enable the [native extensions](#native-extensions), add the macro `BS_THREAD_POOL_NATIVE_EXTENSIONS` to `add_compile_definitions()`.

Place this file in the root folder of the repository, and then run the following commands:

```bash
cmake -B build
cmake --build build
build/BS_thread_pool_test
```

For MSVC, replace the last command with `build/Debug/BS_thread_pool_test`. If the `std` module was successfully imported, the test program will print the message:

```none
C++ Standard Library imported using:
* Thread pool library: import std (C++23 std module).
* Test program: import std (C++23 std module).
```

You can also instruct CMake to download the library automatically from the GitHub repository, as explained below, either using [CPM](#installing-using-cmake-with-cpm) or [`FetchContent`](#installing-using-cmake-with-fetchcontent).

## Installing the library using package managers

### Installing using vcpkg

If you are using the [vcpkg](https://vcpkg.io/) C/C&plus;&plus; package manager, you can easily install `BS::thread_pool` with the following command:

```bash
vcpkg install bshoshany-thread-pool
```

To update the package to the latest version, run:

```bash
vcpkg upgrade
```

Please refer to [this package's page on vcpkg.io](https://vcpkg.io/en/package/bshoshany-thread-pool) for more information.

### Installing using Conan

If you are using the [Conan](https://conan.io/) C/C&plus;&plus; package manager, you can easily integrate `BS::thread_pool` into your project by adding the following lines to your `conanfile.txt`:

```ini
[requires]
bshoshany-thread-pool/5.0.0
```

To update the package to the latest version, simply change the version number. Please refer to [this package's page on ConanCenter](https://conan.io/center/recipes/bshoshany-thread-pool) for more information.

### Installing using Meson

If you are using the [Meson](https://mesonbuild.com/) build system, you can install `BS::thread_pool` from [WrapDB](https://mesonbuild.com/Wrapdb-projects.html). To do so, create a `subprojects` folder in your project (if it does not already exist) and run the following command:

```bash
meson wrap install bshoshany-thread-pool
```

Then, use `dependency('bshoshany-thread-pool')` in your `meson.build` file to include the package. To update the package to the latest version, run:

```bash
meson wrap update bshoshany-thread-pool
```

### Installing using CMake with CPM

Note: The following instructions have only been tested using CMake v3.31.2 and CPM v0.40.2, the latest versions at the time of writing, and may not work with older versions.

If you are using [CMake](https://cmake.org/), you can install `BS::thread_pool` most easily with [CPM](https://github.com/cpm-cmake/CPM.cmake). If CPM is already installed, simply add the following to your project's `CMakeLists.txt`:

```cmake
CPMAddPackage(
    NAME BS_thread_pool
    GITHUB_REPOSITORY bshoshany/thread-pool
    VERSION 5.0.0
    EXCLUDE_FROM_ALL
    SYSTEM
)
add_library(BS_thread_pool INTERFACE)
target_include_directories(BS_thread_pool INTERFACE ${BS_thread_pool_SOURCE_DIR}/include)
```

This will automatically download the indicated version of the package from [the GitHub repository](https://github.com/bshoshany/thread-pool) and include it in your project.

A convenient shorthand for GitHub packages also exists, in which case `CPMAddPackage()` can be called with a single argument of the form `"gh:user/name@version"`. After that, `CPM_LAST_PACKAGE_NAME` will be set to the name of the package, so we need to use this variable to define the include folder. This results in a more compact configuration:

```cmake
CPMAddPackage("gh:bshoshany/thread-pool@5.0.0")
add_library(BS_thread_pool INTERFACE)
target_include_directories(BS_thread_pool INTERFACE ${${CPM_LAST_PACKAGE_NAME}_SOURCE_DIR}/include)
```

It is also possible to use CPM without installing it first, by adding the following lines to `CMakeLists.txt` before `CPMAddPackage()`:

```cmake
set(CPM_DOWNLOAD_LOCATION ${CMAKE_BINARY_DIR}/CPM.cmake)
if(NOT(EXISTS ${CPM_DOWNLOAD_LOCATION}))
    file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()
include(${CPM_DOWNLOAD_LOCATION})
```

Here is an example of a complete `CMakeLists.txt` which automatically downloads and compiles the test program [`BS_thread_pool_test.cpp`](#automated-tests):

```cmake
cmake_minimum_required(VERSION 3.31)
project(BS_thread_pool_test LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/permissive- /Zc:__cplusplus)
endif()

set(CPM_DOWNLOAD_LOCATION ${CMAKE_BINARY_DIR}/CPM.cmake)
if(NOT(EXISTS ${CPM_DOWNLOAD_LOCATION}))
    file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()
include(${CPM_DOWNLOAD_LOCATION})

CPMAddPackage("gh:bshoshany/thread-pool@5.0.0")
add_library(BS_thread_pool INTERFACE)
target_include_directories(BS_thread_pool INTERFACE ${${CPM_LAST_PACKAGE_NAME}_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} ${${CPM_LAST_PACKAGE_NAME}_SOURCE_DIR}/tests/BS_thread_pool_test.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE BS_thread_pool)
```

Note that for MSVC we have to add the `/permissive-` flag to enforce strict C&plus;&plus; standard conformance, otherwise the test program will not compile, and `/Zc:__cplusplus`, otherwise the test program cannot detect the correct C&plus;&plus; version. This is handled automatically by the `if(MSVC)` block.

To enable the [native extensions](#native-extensions), add the line `add_compile_definitions(BS_THREAD_POOL_NATIVE_EXTENSIONS)`. Replace `CMAKE_CXX_STANDARD 17` with `20` or `23` if you wish to use C&plus;&plus;20 or C&plus;&plus;23 features, respectively. Of course, you should add warning, debugging, optimization, and other compiler flags to the configuration above as needed.

With this `CMakeLists.txt` in an empty folder, type the following commands to build and run the project:

```bash
cmake -B build
cmake --build build
build/BS_thread_pool_test
```

For MSVC, replace the last command with `build/Debug/BS_thread_pool_test`. Please see [here](#compiling-with-cmake-using-import-bsthread_pool) for instructions on how to import the library as a C&plus;&plus;20 module with CMake, and [here](#compiling-with-cmake-using-import-std) for instructions on how to import the C&plus;&plus; Standard Library as a module with CMake.

### Installing using CMake with `FetchContent`

Note: The following instructions have only been tested using CMake v3.31.2, the latest version at the time of writing, and may not work with older versions.

If you are using [CMake](https://cmake.org/) but do not wish to use 3rd-party tools, you can also install `BS::thread_pool` using the built-in [`FetchContent`](https://cmake.org/cmake/help/latest/module/FetchContent.html) module. Here is an example of a complete `CMakeLists.txt` which automatically downloads and compiles the test program, as in the previous section, but this time using `FetchContent` directly:

```cmake
cmake_minimum_required(VERSION 3.31)
project(BS_thread_pool_test LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/permissive- /Zc:__cplusplus)
endif()

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
FetchContent_Declare(
    bshoshany_thread_pool
    GIT_REPOSITORY https://github.com/bshoshany/thread-pool.git
    GIT_TAG v5.0.0
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(bshoshany_thread_pool)
add_library(BS_thread_pool INTERFACE)
target_include_directories(BS_thread_pool INTERFACE ${bshoshany_thread_pool_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME} ${bshoshany_thread_pool_SOURCE_DIR}/tests/BS_thread_pool_test.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE BS_thread_pool)
```

## Complete library reference

This section provides a complete reference for all classes and functions available in this library, along with other important information. Functions are given with simplified prototypes (e.g. removing `const`) for ease of reading. Explanations are kept brief, as the purpose of this section is only to provide a quick reference; for more detailed information and usage examples, please refer to the full documentation above.

Descriptions of each item can also be found in the [Doxygen](https://www.doxygen.nl/) comments embedded in the source code. Any modern IDE, such as [Visual Studio Code](https://code.visualstudio.com/), can use these Doxygen comments to provide automatic documentation for any class and member function in this library when hovering over code with the mouse or using auto-complete.

### The `BS::thread_pool` class template

`BS::thread_pool` is the main thread pool class. It is used to create a pool of threads that continuously execute tasks submitted to a queue. It can take template parameters, which enable optional features as described [below](#optional-features-and-the-template-parameter). The member functions that are available by default, when no template parameters are used, are:

* Constructors:
    * `thread_pool()`: Construct a new thread pool with a number of threads equal to `std::thread::hardware_concurrency()`.
    * `thread_pool(std::size_t num_threads)`: Construct a new thread pool with the specified number of threads.
    * `thread_pool(F&& init)`: Construct a new thread pool with a number of threads equal to `std::thread::hardware_concurrency()` and the specified initialization function. `F` is a template parameter.
    * `thread_pool(std::size_t num_threads, F&& init)`: Construct a new thread pool with the specified number of threads and the specified initialization function.
* Resetters:
    * `void reset()`: Reset the pool with a number of threads equal to `std::thread::hardware_concurrency()`, waiting for running tasks first, and preserving submitted tasks after the reset.
    * `void reset(std::size_t num_threads)`: Reset the pool with a new number of threads.
    * `void reset(F&& init)` Reset the pool with a number of threads equal to `std::thread::hardware_concurrency()` and a new initialization function. `F` is a template parameter.
    * `void reset(std::size_t num_threads, F&& init)`: Reset the pool with a new number of threads and a new initialization function.
* Setters:
    * `void set_cleanup_func(F&& cleanup)`: Set the thread pool's cleanup function. `F` is a template parameter.
* Getters:
    * `std::size_t get_tasks_queued()`: Get the number of tasks currently waiting in the queue to be executed by the threads.
    * `std::size_t get_tasks_running()`: Get the number of tasks currently being executed by the threads.
    * `std::size_t get_tasks_total()`: Get the total number of unfinished tasks: either still waiting in the queue, or running in a thread. Note that `get_tasks_total() == get_tasks_queued() + get_tasks_running()`.
    * `std::size_t get_thread_count()`: Get the number of threads in the pool.
    * `std::vector<std::thread::id> get_thread_ids()`: Get a vector containing the unique identifiers for each of the pool's threads, as obtained by `std::thread::get_id()` (or `std::jthread::get_id()` in C&plus;&plus;20 and later).
* Task submission without futures (`T1`, `T2`, and `F` are template parameters):
    * `void detach_task(F&& task)`: Submit a function with no arguments and no return value into the task queue. To submit a function with arguments, enclose it in a lambda expression.
    * `void detach_blocks(T1 first_index, T2 index_after_last, F&& block, std::size_t num_blocks = 0)`: Parallelize a loop by automatically splitting it into blocks. The block function takes two arguments, the start and end of the block, so that it is only called once per block, but it is up to the user make sure the block function correctly deals with all the indices in each block.
    * `void detach_loop(T1 first_index, T2 index_after_last, F&& loop, std::size_t num_blocks = 0)`: Parallelize a loop by automatically splitting it into blocks. The loop function takes one argument, the loop index, so that it is called many times per block.
    * `void detach_sequence(1T first_index, T2 index_after_last, F&& sequence)`: Submit a sequence of tasks enumerated by indices to the queue. The sequence function takes one argument, the task index, and will be called once per index.
* Task submission with futures (`T1`, `T2`, `F`, and `R` are template parameters):
    * `std::future<R> submit_task(F&& task)`: Submit a function with no arguments into the task queue. To submit a function with arguments, enclose it in a lambda expression.
    * `BS::multi_future<R> submit_blocks(T1 first_index, T2 index_after_last, F&& block, std::size_t num_blocks = 0)`: Parallelize a loop by automatically splitting it into blocks. The block function takes two arguments, the start and end of the block, so that it is only called once per block, but it is up to the user make sure the block function correctly deals with all the indices in each block. Returns a `BS::multi_future` that contains the futures for all of the blocks.
    * `BS::multi_future<void> submit_loop(T1 first_index, T2 index_after_last, F&& loop, std::size_t num_blocks = 0)`: Parallelize a loop by automatically splitting it into blocks. The loop function takes one argument, the loop index, so that it is called many times per block. It must have no return value. Returns a `BS::multi_future` that contains the futures for all of the blocks.
    * `BS::multi_future<R> submit_sequence(T1 first_index, T2 index_after_last, F&& sequence)`: Submit a sequence of tasks enumerated by indices to the queue. The sequence function takes one argument, the task index, and will be called once per index. Returns a `BS::multi_future` that contains the futures for all of the tasks.
* Task management:
    * `void purge()`: Purge all the tasks waiting in the queue. Please note that there is no way to restore the purged tasks.
* Waiting for tasks (`R`, `P`, `C`, and `D` are template parameters):
    * `void wait()`: Wait for all tasks to be completed, both those that are currently running in the threads and those that are still waiting in the queue.
    * `bool wait_for(std::chrono::duration<R, P>& duration)`: Wait for tasks to be completed, but stop waiting after the specified duration has passed. Returns `true` if all tasks finished running, `false` if the duration expired but some tasks are still running.
    * `bool wait_until(std::chrono::time_point<C, D>& timeout_time)`: Wait for tasks to be completed, but stop waiting after the specified time point has been reached. Returns `true` if all tasks finished running, `false` if the time point was reached but some tasks are still running.
* Destructor:
    * `~thread_pool()`: Wait for all tasks to complete, then destroy all threads. If a cleanup function was set, it will run in each thread right before it is destroyed.

### Optional features and the template parameter

The thread pool has several optional features that must be explicitly enabled by passing a template parameter. The template parameter is a bitmask, so you can enable several features at once by combining them with the bitwise OR operator `|`. The bitmask flags are members of the `BS::tp` enumeration.

* **Task priority:** Enabled by turning on the `BS::tp::priority` flag in the template parameter. When enabled, the static member `priority_enabled` will be set to `true`.
    * When enabled, the priority of a task or group of tasks may be specified as an additional argument (at the end of the argument list) to all detach and submit functions. If the priority is not specified, the default value will be 0.
    * The priority is of type `BS::priority_t`, a signed 8-bit integer, with values between -128 and +127. The tasks will be executed in priority order from highest to lowest. Groups of parallelized tasks will all have the same priority.
    * The enumeration `BS::pr` contains some pre-defined priorities: `BS::pr::highest`, `BS::pr::high`, `BS::pr::normal`, `BS::pr::low`, and `BS::pr::lowest`.
* **Pausing:** Enabled by turning on the `BS::tp::pause` flag in the template parameter. When enabled, the static member `pause_enabled` will be set to `true`. Adds the following member functions:
    * `void pause()`: Pause the pool. The workers will temporarily stop retrieving new tasks out of the queue, although any tasks already executed will keep running until they are finished.
    * `void unpause()`: Unpause the pool. The workers will resume retrieving new tasks out of the queue.
    * `bool is_paused()`: Check whether the pool is currently paused.
* **Wait deadlock checks:** Enabled by turning on the `BS::tp::wait_deadlock_checks` flag in the template parameter. When enabled, the static member `wait_deadlock_checks_enabled` will be set to `true`.
    * When enabled, `wait()`, `wait_for()`, and `wait_until()` will check whether the user tried to call them from within a thread of the same pool, which would result in a deadlock. If so, they will throw the exception `BS::wait_deadlock` instead of waiting.
    * If the feature-test macro `__cpp_exceptions` is undefined, wait deadlock checks will be automatically disabled, and trying to enable this feature will result in a compilation error.

Convenience aliases are defined as follows:

* `BS::light_thread_pool` disables all optional features (equivalent to `BS::thread_pool` with the default template parameter, that is, `BS::thread_pool<BS::tp::none>`).
* `BS::priority_thread_pool` enables task priority (equivalent to `BS::thread_pool<BS::tp::priority>`).
* `BS::pause_thread_pool` enables pausing the pool (equivalent to `BS::thread_pool<BS::tp::pause>`).
* `BS::wdc_thread_pool` enables wait deadlock checks (equivalent to `BS::thread_pool<BS::tp::wait_deadlock_checks>`).

### The `BS::this_thread` class

The class `BS::this_thread` provides functionality analogous to `std::this_thread`. It contains the following static member functions:

* `static std::optional<std::size_t> get_index()`: Get the index of the current thread. The optional object will not have a value if the thread is not in a pool.
* `static std::optional<void*> get_pool()`: Get a pointer to the thread pool that owns the current thread. The optional object will not have a value if the thread is not in a pool.

If the [native extensions](#the-native-extensions) are enabled, the class will contain additional static member functions. Please see the relevant section for more information.

### The native extensions

The native extensions may be enabled by defining the macro `BS_THREAD_POOL_NATIVE_EXTENSIONS` at compilation time. If including the library as a header file, the macro must be defined before `#include "BS_thread_pool.hpp"`. If importing the library as a C&plus;&plus;20 module, the macro must be defined as a compiler flag. The native extensions use the operating system's native API, and are thus not portable; however, they should work on Windows, Linux, and macOS.

The native extensions add the following functions to the `BS` namespace:

* `bool set_os_process_affinity(std::vector<bool>& affinity)`: Set the processor affinity of the current process. The argument is an `std::vector<bool>` where each element corresponds to a logical processor. Returns `true` if the affinity was set successfully, `false` otherwise. Does not work on macOS.
* `std::optional<std::vector<bool>> BS::get_os_process_affinity()`: Get the processor affinity of the current process. The optional object will not have a value if the affinity could not be determined. Does not work on macOS.
* `bool BS::set_os_process_priority(BS::os_process_priority priority)`: Set the priority of the current process. The argument must be a member of the `BS::os_process_priority` enumeration, which contains the options `idle`, `below_normal`, `normal`, `above_normal`, `high`, and `realtime`. Returns `true` if the priority was set successfully, or `false` otherwise.
* `std::optional<BS::os_process_priority> BS::get_os_process_priority()`: Get the priority of the current process. The optional object will not have a value if the priority could not be determined, or it is not one of the pre-defined values in the `BS::os_process_priority` enumeration.

The native extensions also add the following static member functions to `BS::this_thread`:

* `bool set_os_thread_affinity(std::vector<bool>& affinity)`: Set the processor affinity of the current thread. The argument is an `std::vector<bool>` where each element corresponds to a logical processor. Note that the thread affinity must be a subset of the process affinity for the containing process of a thread. Does not work on macOS.
* `std::optional<std::vector<bool>> get_os_thread_affinity()`: Get the processor affinity of the current thread. The optional object will not have a value if the affinity could not be determined. Does not work on macOS.
* `bool set_os_thread_name(std::string& name)`: Set the name of the current thread. Note that on Linux thread names are limited to 16 characters, including the null terminator. Returns `true` if the name was set successfully, `false` otherwise.
* `std::optional<std::string> get_os_thread_name()`: Get the name of the current thread. The optional object will not have a value if the name could not be determined.
* `bool set_os_thread_priority(BS::os_thread_priority priority)`: Set the priority of the current thread. The argument must be a member of the `BS::os_thread_priority` enumeration, which contains the options `idle`, `lowest`, `below_normal`, `normal`, `above_normal`, `highest`, and `realtime`. Returns `true` if the priority was set successfully, or `false` otherwise.
* `std::optional<os_thread_priority> get_os_thread_priority()`: Get the priority of the current thread. The optional object will not have a value if the priority could not be determined, or it is not one of the pre-defined values in the `BS::os_thread_priority` enumeration.

Finally, the native extensions add the following member function to `BS::thread_pool`:

* `std::vector<std::thread::native_handle_type> get_native_handles()`: Get a vector containing the underlying implementation-defined thread handles for each of the pool's threads.

### The `BS::multi_future` class

`BS::multi_future<T>` is a helper class used to facilitate waiting for and/or getting the results of multiple futures at once. It is defined as a specialization of `std::vector<std::future<T>>`. This means that all of the member functions that can be used on an [`std::vector<std::future<T>>`](https://en.cppreference.com/w/cpp/container/vector) can also be used on a `BS::multi_future<T>`. For example, you may use a range-based for loop with a `BS::multi_future<T>`, since it has iterators.

In addition to inherited member functions, `BS::multi_future<T>` has the following specialized member functions (`R` and `P`, `C`, and `D` are template parameters):

* `[void or std::vector<T>] get()`: Get the results from all the futures stored in this `BS::multi_future`, rethrowing any stored exceptions. If the futures return `void`, this function returns `void` as well. If the futures return a type `T`, this function returns a vector containing the results.
* `std::size_t ready_count()`: Check how many of the futures stored in this `BS::multi_future` are ready.
* `bool valid()`: Check if all the futures stored in this `BS::multi_future` are valid.
* `void wait()`: Wait for all the futures stored in this `BS::multi_future`.
* `bool wait_for(std::chrono::duration<R, P>& duration)`: Wait for all the futures stored in this `BS::multi_future`, but stop waiting after the specified duration has passed. Returns `true` if all futures have been waited for before the duration expired, `false` otherwise.
* `bool wait_until(std::chrono::time_point<C, D>& timeout_time)`: Wait for all the futures stored in this `BS::multi_future` object, but stop waiting after the specified time point has been reached. Returns `true` if all futures have been waited for before the time point was reached, `false` otherwise.

### The `BS::synced_stream` class

`BS::synced_stream` is a utility class which can be used to synchronize printing to one or more output streams by different threads. It has the following member functions (`T` is a template parameter pack):

* `synced_stream()`: Construct a new synced stream which prints to `std::cout`.
* `synced_stream(T&... streams)`: Construct a new synced stream which prints to the given output streams.
* `void add_stream(std::ostream& stream)`: Add a stream to the list of output streams.
* `std::vector<std::ostream*>& get_streams()`: Get a reference to a vector containing pointers to the output streams to print to.
* `void print(T&... items)`: Print any number of items into the output streams. Ensures that no other threads print to the streams simultaneously, as long as they all exclusively use the same `BS::synced_stream` object to print.
* `void println(T&&... items)`: Print any number of items into the output streams, followed by a newline character.
* `void remove_stream(std::ostream& stream)`: Remove a stream from the list of output streams.

In addition, the class comes with two stream manipulators, which are meant to help the compiler figure out which template specializations to use with the class:

* `BS::synced_stream::endl`: An explicit cast of `std::endl`. Prints a newline character to the stream, and then flushes it. Should only be used if flushing is desired, otherwise a newline character should be used instead.
* `BS::synced_stream::flush`: An explicit cast of `std::flush`. Used to flush the stream.

### The `BS::version` class

`BS::version` is a utility class used to represent a version number. It has public members `major`, `minor`, and `patch`, as well as the following member functions:

* `constexpr version(std::uint64_t major, std::uint64_t minor, std::uint64_t patch)`: Construct a new version object with the specified major, minor, and patch numbers.
* `std::strong_ordering operator<=>(version&)`: 3-way comparison operator for two version numbers, in C&plus;&plus;20 and later. In C&plus;&plus;17, the operators `==`, `!=`, `<`, `<=`, `>`, `>=` are instead defined explicitly.
* `std::string to_string()`: Convert the version number to a string in the format `"major.minor.patch"`.
* `std::ostream& operator<<(std::ostream& stream, version& ver)`: Output the version string to a stream.

In addition, the library defines a `constexpr` object `BS::thread_pool_version` of type `BS::version`, which can be used to check the version of the library at compilation time.

Note that this feature is only available starting with v5.0.0 of the library; previous versions used the macros `BS_THREAD_POOL_VERSION_MAJOR`, `BS_THREAD_POOL_VERSION_MINOR`, and `BS_THREAD_POOL_VERSION_PATCH`, which are still defined for compatibility purposes, but are not accessible if the library is imported as a C&plus;&plus;20 module.

### Diagnostic variables

The library defines the following `constexpr` variables:

* `bool thread_pool_import_std`: Indicates whether the library imported the C&plus;&plus;23 Standard Library module using `import std`.
* `bool thread_pool_module`: Indicates whether the library was compiled as a C&plus;&plus;20 module.
* `bool thread_pool_native_extensions`: Indicates whether the native extensions are enabled.

### All names exported by the C&plus;&plus;20 module

When the library is imported as a C&plus;&plus;20 module using `import BS.thread_pool`, it exports the following names, in alphabetical order:

* `BS::binary_semaphore`
* `BS::common_index_type_t`
* `BS::counting_semaphore`
* `BS::light_thread_pool`
* `BS::multi_future`
* `BS::pause_thread_pool`
* `BS::pr`
* `BS::priority_t`
* `BS::priority_thread_pool`
* `BS::synced_stream`
* `BS::this_thread`
* `BS::thread_pool`
* `BS::thread_pool_import_std`
* `BS::thread_pool_module`
* `BS::thread_pool_native_extensions`
* `BS::thread_pool_version`
* `BS::tp`
* `BS::version`
* `BS::wait_deadlock`
* `BS::wdc_thread_pool`

If the native extensions are enabled, the following names are also exported:

* `BS::get_os_process_affinity`
* `BS::get_os_process_priority`
* `BS::os_process_priority`
* `BS::os_thread_priority`
* `BS::set_os_process_affinity`
* `BS::set_os_process_priority`

## Development tools

### The `compile_cpp.py` script

The Python script `compile_cpp.py`, in the `scripts` folder of [the GitHub repository](https://github.com/bshoshany/thread-pool), can be used to compile any C&plus;&plus; source file with different compilers on different platforms. It requires Python 3.12 or later.

The script was written by the author of the library to make it easier to test the library with different combinations of compilers, standards, and platforms using the built-in Visual Studio Code tasks. However, note that this script is not meant to replace CMake or any full-fledged build system, it's just a convenient script for developing single-header libraries like this one or other small projects.

The `compile_cpp.py` script also transparently handles C&plus;&plus;20 modules and importing the C&plus;&plus; Standard Library as a module in C&plus;&plus;23. Therefore, users of this library who wish to import it as a C&plus;&plus;20 module may find this script particularly useful.

The compilation parameters can be configured using the command line arguments and/or via an optional YAML configuration file `compile_cpp.yaml`. The command line arguments are as follows:

* Positional argument(s): the source file(s) to compile.
* `-h` or `--help`: Show the help message and exit.
* `-a` or `--arch`: The target architecture (MSVC only). Must be one of `[amd64, arm64]`, default is `amd64`.
* `-c` or `--compiler`: Which compiler to use. Must be one of `[cl, clang++, g++]`. The default is to determine it automatically based on the platform.
* `-d` or `--define`: Macros to define. Use this argument multiple times to define more than one macro. Additional macros can be defined in `compile_cpp.yaml`.
* `-f` or `--flag`: Extra compiler flags to add. Use this argument multiple times to add more than one flag. Additional flags can be specified in `compile_cpp.yaml`.
* `-i` or `--include`: The include folder to use. Use this argument multiple times to use more than one include folder. Additional include folders can be specified in `compile_cpp.yaml`.
* `-l` or `--as-module`: Enable this flag to compile the file as a C&plus;&plus;20 module.
* `-m` or `--module`: C&plus;&plus;20 module files to use if desired, in the format `module_name=module_file,dependent_files,...`. Use this argument multiple times to use more than one module. Additional modules can be specified in `compile_cpp.yaml`. The dependent files are only used to determine whether the module needs to be recompiled.
* `-o` or `--output`: The output folder and/or executable name. End with `/` to create the folder if it doesn't already exist. If not specified, the folder defined in `compile_cpp.yaml` will be used. If the executable name is not specified, it will be determined automatically in the format `source_[module_]type-compiler-standard` where:
    * `source` is the name of the first source file (without the extension).
    * `module_`, if present, indicates that the file is a C&plus;&plus;20 module.
    * `type` is one of `[debug, release]`.
    * `compiler` is one of `[clang, gcc, msvc]`.
    * `standard` is one of `[c++17, c++20, c++23]`.
* `-p` or `--pass`: Pass command line arguments to the compiled program when running it, if `-r` is specified. Use this argument multiple times to pass more than one argument to the program. Additional arguments can be specified in `compile_cpp.yaml`.
* `-r` or `--run`: Enable this flag to run the program after compiling it.
* `-s` or `--std`: Which C&plus;&plus; standard to use. Must be one of `[c++17, c++20, c++23]`. The default is `c++23`.
* `-t` or `--type`: Which mode to compile in. Must be one of `[debug, release]`. The default is `debug`.
* `-u` or `--std-module`: Specify the path to the standard library module (C&plus;&plus;23 only). Taken from `compile_cpp.yaml` if not specified. Use `auto` to auto-detect or `disable` to explicitly disable.
* `-v` or `--verbose`: Enable this flag to print the script's diagnostic messages.

The `compile_cpp.yaml` file includes the following fields:

* `defines`: A list of macros to define when compiling the source files.
* `flags`: A map of flags to pass to each compiler. The compiler should be one of `[cl, clang++, g++]`. The flags should be a list of strings.
* `includes`: A list of include folders.
* `modules`: A map of C&plus;&plus;20 modules in the format `module_name: [module_path, dependent files, ...]`. Will only be used in C&plus;&plus;20 or C&plus;&plus;23 mode. The dependent files are only used to determine whether the module needs to be recompiled.
* `output`: The output folder for the compiled files.
* `pass_args`: A list of arguments to pass to the program if running it after compilation.
* `std_module`: A map of paths to the standard library modules for each OS and compiler combination (C&plus;&plus;23 only). The OS should be one of `[Windows, Linux, Darwin]`. Use `Automatic` to determine the path automatically if possible.

Please see the `compile_cpp.yaml` file in the GitHub repository for an example of how to use it.

### Other included tools

The `scripts` folder of [the GitHub repository](https://github.com/bshoshany/thread-pool) contains two other Python scripts that are used in the development of the library:

* `test_all.py` performs the [automated tests](#automated-tests) in C&plus;&plus;17, C&plus;&plus;20, and C&plus;&plus;23 modes, using all compilers available in the system (Clang, GCC, and/or MSVC). Since there are so many tests, the test script does not perform the benchmarks, as that would take too long. Pass the optional argument `--compile-only` to only check that the program compiles successfully with all compilers, without running it.
* `clear_folder.py` is used to clean up output and temporary folders. It will create the folder if it does not already exist, so the outcome is always an empty folder.

In addition, for Visual Studio Code users, the GitHub repository includes three `.vscode` folders:

* `.vscode-windows`, to be used in Windows with Clang, GCC, and MSVC.
* `.vscode-linux`, to be used in Linux with Clang and GCC.
* `.vscode-macos`, to be used in macOS with LLVM Clang (not Apple Clang).

Each folder contains appropriate `c_cpp_properties.json`, `launch.json`, and `tasks.json` files that utilize the included Python scripts. Users are welcome to use these files in their own projects, but they may require some modifications to work on specific systems.

## About the project

### Bug reports and feature requests

This library is under continuous and active development. If you encounter any bugs, or if you would like to request any additional features, please feel free to [open a new issue on GitHub](https://github.com/bshoshany/thread-pool/issues) and I will look into it as soon as I can.

### Contribution and pull request policy

Contributions are always welcome. However, I release my projects in cumulative updates after editing and testing them locally on my system, so **my policy is to never accept any pull requests**. If you open a pull request, and I decide to incorporate your suggestion into the project, I will first modify your code to comply with the project's coding conventions (formatting, syntax, naming, comments, programming practices, etc.), and perform some tests to ensure that the change doesn't break anything. I will then merge it into the next release of the project, possibly together with some other changes. The new release will also include a note in `CHANGELOG.md` with a link to your pull request, and modifications to the documentation in `README.md` as needed.

### Starring the repository

If you found this project useful, please consider [starring it on GitHub](https://github.com/bshoshany/thread-pool/stargazers)! This allows me to see how many people are using my code, and motivates me to keep working to improve it.

### Acknowledgements

Many GitHub users have helped improve this project, directly or indirectly, via issues, pull requests, comments, and/or personal correspondence. Please see `CHANGELOG.md` for links to specific issues and pull requests that have been the most helpful. Thank you all for your contribution! 😊

### Copyright and citing

Copyright (c) 2024 [Barak Shoshany](https://baraksh.com/). Licensed under the [MIT license](https://github.com/bshoshany/thread-pool/blob/master/LICENSE.txt).

If you use this library in software of any kind, please provide a link to [the GitHub repository](https://github.com/bshoshany/thread-pool) in the source code and documentation.

If you use this library in published research, please cite it as follows:

* Barak Shoshany, *"A C++17 Thread Pool for High-Performance Scientific Computing"*, [doi:10.1016/j.softx.2024.101687](https://doi.org/10.1016/j.softx.2024.101687), [SoftwareX 26 (2024) 101687](https://www.sciencedirect.com/science/article/pii/S235271102400058X), [arXiv:2105.00613](https://arxiv.org/abs/2105.00613)

You can use the following BibTeX entry:

```bibtex
@article{Shoshany2024_ThreadPool,
    archiveprefix = {arXiv},
    author        = {Barak Shoshany},
    doi           = {10.1016/j.softx.2024.101687},
    eprint        = {2105.00613},
    journal       = {SoftwareX},
    pages         = {101687},
    title         = {{A C++17 Thread Pool for High-Performance Scientific Computing}},
    url           = {https://www.sciencedirect.com/science/article/pii/S235271102400058X},
    volume        = {26},
    year          = {2024}
}
```

Please note that the papers on [SoftwareX](https://www.sciencedirect.com/science/article/pii/S235271102400058X) and [arXiv](https://arxiv.org/abs/2105.00613) are not up to date with the latest version of the library. These publications are only intended to facilitate discovery of this library by scientists, and to enable citing it in scientific research. Documentation for the latest version is provided only by the `README.md` file in [the GitHub repository](https://github.com/bshoshany/thread-pool).

### About the author

My name is Barak Shoshany and I am a theoretical, mathematical, and computational physicist. I work as an Assistant Professor of Physics at Brock University in Ontario, Canada, and I am also a Sessional Lecturer at McMaster University. My research focuses on the nature of time and causality in general relativity and quantum mechanics, as well as symbolic and high-performance scientific computing. For more about me, please see [my personal website](https://baraksh.com/).

### Learning more about C&plus;&plus;

Beginner C&plus;&plus; programmers may be interested in [my lecture notes](https://baraksh.com/CSE701/notes/) for a graduate-level course taught at McMaster University, which teach modern C and C&plus;&plus; from scratch, including some of the advanced techniques and programming practices used in developing this library. I have been teaching this course every year since 2020, and the notes are continuously updated and improved based on student feedback.

### Other projects to check out

If you are a physicist or astronomer, you may be interested in my project [OGRe](https://github.com/bshoshany/OGRe): An Object-Oriented General Relativity Package for Mathematica, or its Python port [OGRePy](https://github.com/bshoshany/OGRePy): An Object-Oriented General Relativity Package for Python.
