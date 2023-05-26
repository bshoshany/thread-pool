# `BS::thread_pool`: a fast, lightweight, and easy-to-use C++17 thread pool library

By Barak Shoshany\
Email: <baraksh@gmail.com>\
Website: <https://baraksh.com/>\
GitHub: <https://github.com/bshoshany>

* [Version history](#version-history)
    * [v3.5.0 (2023-05-25)](#v350-2023-05-25)
    * [v3.4.0 (2023-05-12)](#v340-2023-05-12)
    * [v3.3.0 (2022-08-03)](#v330-2022-08-03)
    * [v3.2.0 (2022-07-28)](#v320-2022-07-28)
    * [v3.1.0 (2022-07-13)](#v310-2022-07-13)
    * [v3.0.0 (2022-05-30)](#v300-2022-05-30)
    * [v2.0.0 (2021-08-14)](#v200-2021-08-14)
    * [v1.9 (2021-07-29)](#v19-2021-07-29)
    * [v1.8 (2021-07-28)](#v18-2021-07-28)
    * [v1.7 (2021-06-02)](#v17-2021-06-02)
    * [v1.6 (2021-05-26)](#v16-2021-05-26)
    * [v1.5 (2021-05-07)](#v15-2021-05-07)
    * [v1.4 (2021-05-05)](#v14-2021-05-05)
    * [v1.3 (2021-05-03)](#v13-2021-05-03)
    * [v1.2 (2021-04-29)](#v12-2021-04-29)
    * [v1.1 (2021-04-24)](#v11-2021-04-24)
    * [v1.0 (2021-01-15)](#v10-2021-01-15)

## Version history

### v3.5.0 (2023-05-25)

* `BS_thread_pool.hpp` and `BS_thread_pool_light.hpp`:
    * Added a new member function, `purge()`, to the full (non-light) thread pool. This function purges all the tasks waiting in the queue. Tasks that are currently running will not be affected, but any tasks still waiting in the queue will be removed and will never be executed by the threads. Please note that there is no way to restore the purged tasks.
    * Fix a bug which caused `wait_for_tasks()` to only block the first thread that called it. Now it blocks every thread that calls it, which is the expected behavior. In addition, all related deadlock have now been completely resolved. This also applies to the variants `wait_for_tasks_duration()` and `wait_for_tasks_until()` in the non-light version. See [#110](https://github.com/bshoshany/thread-pool/pull/110).
        * Note: You should never call `wait_for_tasks()` from within a thread of the same thread pool, as that will cause it to wait forever! This fix is relevant for situations when `wait_for_tasks()` is called from an auxiliary `std::thread` or a separate thread pool.
    * `push_task()` and `submit()` now avoid creating unnecessary copies of the function object. This should improve performance, especially if large objects are involved. See [#90](https://github.com/bshoshany/thread-pool/pull/90).
    * Optimized the way condition variables are used by the thread pool class. Shared variables are now modified while owning the mutex, but condition variables are notified after the mutex is released, if possible. See [#84](https://github.com/bshoshany/thread-pool/pull/84).
    * Instead of a variable `tasks_total` to keep track of the total number of tasks (queued + running), the thread pool class now uses a variable `tasks_running` to keep track only of the number of running tasks, with the number of tasks in the queue obtained via `tasks.size()`. This makes more sense in terms of the internal logic of the class.
    * All atomic variables have been converted to non-atomic. They are now all governed by `tasks_mutex`, so they do not need to be atomic. This eliminates redundant locking, and may improve performance a bit.
    * `running` has been renamed to `workers_running` and `task_done_cv` has been renamed to `tasks_done_cv`.
    * The worker now only notifies thi condition variable `tasks_done_cv` if all the tasks are done, not just a single task. Checking if the tasks are done is cheaper than notifying the condition variable, so since the worker no longer notifies the condition variable every single time it finishes a task, this should improve performance a bit if `wait_for_tasks()` is used.
* `BS_thread_pool_test.cpp`:
    * Combined the tests for the full and light versions into one program. The file `BS_thread_pool_light_test.cpp` has been removed.
    * The tests for the light version are now much more comprehensive. The only features that are not tested in the light version are those that do not exist in it.
    * Added a test for the new `purge()` member function.
    * Added a test to ensure that `push_task()` and `submit()` do not create unnecessary copies of the function object.
    * Added a test to ensure that `push_task()` and `submit()` correctly accept arguments passed by value, reference, and constant reference.
    * Added a test to ensure that `wait_for_tasks()` blocks all external threads that call it.
    * `_CRT_SECURE_NO_WARNINGS` is now set only if it has not already been defined, to prevent errors in MSVC projects which already have it set as part of the default build settings. See [#72](https://github.com/bshoshany/thread-pool/pull/72).
* `README.md`:
    * Added documentation for the new `purge()` member function.
    * Added an explanation for how to pass arguments by reference or constant reference when submitting functions to the queue, using the wrappers `std::ref()` and `std::cref()` respectively. See [#83](https://github.com/bshoshany/thread-pool/issues/83).
    * Added a link to [my lecture notes](https://baraksh.com/CSE701/notes.php) for a course taught at McMaster University, for the benefit of beginner C++ programmers who wish to learn some of the advanced techniques and programming practices used in developing this library.
    * Removed the sample test results, since the complete log file (including the deadlock tests) is now over 500 lines long.
* Other:
    * A `.clang-format` file with the project's formatting conventions is now included in the GitHub repository. The pull request template now asks to format any new code using this file, so that it is consistent with the rest of the library.
    * A PowerShell script, `BS_thread_pool_test.ps1`, is now provided in the GitHub repository to make running the test on multiple compilers and operating systems easier. Since it is written in PowerShell, it is fully portable and works on Windows, Linux, and macOS. The script will automatically detect if Clang, GCC, and/or MSVC are available, compile the test program using each available compiler, and then run each compiled test program 5 times and report on any errors. The pull request template now recommends using this script for testing.
    * Since the root folder has become a bit crowded, the header files `BS_thread_pool.hpp` and `BS_thread_pool_light.hpp` have been moved to the `include` subfolder, and the test file `BS_thread_pool_test.cpp` has been moved to the `tests` subfolder, which also contains the new test script `BS_thread_pool_test.ps1`.

### v3.4.0 (2023-05-12)

* `BS_thread_pool.hpp` and `BS_thread_pool_light.hpp`:
    * Resolved an issue which could have caused `tasks_total` to not be synchronized in some cases. See [#70](https://github.com/bshoshany/thread-pool/pull/70).
    * Resolved a deadlock which could rarely be caused when the pool was destructed or reset. See [#93](https://github.com/bshoshany/thread-pool/pull/93), [#100](https://github.com/bshoshany/thread-pool/pull/100), [#107](https://github.com/bshoshany/thread-pool/pull/107), and [#108](https://github.com/bshoshany/thread-pool/pull/108).
    * Resolved a deadlock which could be caused when `wait_for_tasks()` was called more than once.
    * Two new member functions have been added to the non-light version: `wait_for_tasks_duration()` and `wait_for_tasks_until()`. They allow waiting for the tasks to complete, but with a timeout. `wait_for_tasks_duration()` will stop waiting after the specified duration has passed, and `wait_for_tasks_until()` will stop waiting after the specified time point has been reached.
    * Renamed `BS_THREAD_POOL_VERSION` in `BS_thread_pool_light.hpp` to `BS_THREAD_POOL_LIGHT_VERSION` and removed the `[light]` tag. This allows including both header files in the same program in case we want to use both the light and non-light thread pools simultaneously.
* `BS_thread_pool_test.cpp` and `BS_thread_pool_light_test.cpp`:
    * Fixed an issue that caused a compilation error when using MSVC and including `Windows.h`. See [#72](https://github.com/bshoshany/thread-pool/pull/72).
    * The number and size of the vectors in the performance test (`BS_thread_pool_test.cpp` only) are now guaranteed to be multiples of the number of threads, for optimal performance.
    * In `count_unique_threads()`, moved the condition variables and mutexes to the function scope to prevent cluttering the global scope.
    * Three new tests have been added to `BS_thread_pool_test.cpp` to check the deadlocks issue that were resolved in this release (see above). The tests rely on the new wait for tasks with timeout feature, so they are not available in the light version.
        * One test checks for deadlocks when calling `wait_for_tasks()` more than once.
        * Two tests check for deadlocks when destructing and resetting the pool respectively. They are turned off by default, since they take a long time to complete, but can be turned on by setting `enable_long_deadlock_tests` to `true`.
    * Two new tests have been added to the non-light version to check the new member functions `wait_for_tasks_duration()` and `wait_for_tasks_until()`.
    * The test programs now return the number of failed tests upon exit, instead of just 1 if any number of tests failed, which was the case in previous versions. Also, if any tests failed, `std::quick_exit()` is invoked instead of `return`, to avoid getting stuck due to any lingering tasks or deadlocks.
* `README.md`:
    * Added documentation for the two new member functions, `wait_for_tasks_duration()` and `wait_for_tasks_until()`.
    * Fixed Markdown rendering incorrectly on Visual Studio. See [#77](https://github.com/bshoshany/thread-pool/pull/77).
    * The sample performance tests are now taken from a 40-core / 80-thread dual-CPU computing node, which is a more typical use case for high-performance scientific software.

### v3.3.0 (2022-08-03)

* `BS_thread_pool.hpp`:
    * The public member variable `paused` of `BS::thread_pool` has been made private for future-proofing (in case future versions implement a more involved pausing mechanism) and better encapsulation. It is now accessible only via the `pause()`, `unpause()`, and `is_paused()` member functions. In other words:
        * Replace `pool.paused = true` with `pool.pause()`.
        * Replace `pool.paused = false` with `pool.unpause()`.
        * Replace `if (pool.paused)` (or similar) with `if (pool.is_paused())`.
    * The public member variable `f` of `BS::multi_future` has been renamed to `futures` for clarity, and has been made private for encapsulation and simplification purposes. Instead of operating on the vector `futures` itself, you can now use the `[]` operator of the `BS::multi_future` to access the future at a specific index directly, or the `push_back()` member function to append a new future to the list. The `size()` member function tells you how many futures are currently stored in the object.
    * The explicit casts of `std::endl` and `std::flush`, added in v3.2.0 to enable flushing a `BS::synced_stream`, caused ODR (One Definition Rule) violations if `BS_thread_pool.hpp` was included in two different translation units, since they were mistakenly not defined as `inline`. To fix this, I decided to make them static members of `BS::synced_stream` instead of global variables, which also makes the code better organized in my opinion. These objects can now be accessed as `BS::synced_stream::endl` and `BS::synced_stream::flush`. I also added an example for how to use them in `README.md`. See [#64](https://github.com/bshoshany/thread-pool/issues/64).
* `BS_thread_pool_light.hpp`:
    * This package started out as a very lightweight thread pool, but over time has expanded to include many additional features, and at the time of writing it has a total of 340 lines of code, including all the helper classes. Therefore, I have decided to bundle a light version of the thread pool in a separate and stand-alone header file, `BS_thread_pool_light.hpp`, with only 170 lines of code (half the size of the full package). This file does not contain any of the helper classes, only a new `BS::thread_pool_light` class, which is a minimal thread pool with only the 5 most basic member functions:
        * `get_thread_count()`
        * `push_loop()`
        * `push_task()`
        * `submit()`
        * `wait_for_tasks()`
    * A separate test program `BS_thread_pool_light_test.cpp` tests only the features of the lightweight `BS::thread_pool_light` class. In the spirit of minimalism, it does not generate a log file and does not do any benchmarks.
    * To be perfectly clear, each header file is 100% stand-alone. If you wish to use the full package, you only need `BS_thread_pool.hpp`, and if you wish to use the light version, you only need `BS_thread_pool_light.hpp`. Only a single header file needs to be included in your project.

### v3.2.0 (2022-07-28)

* `BS_thread_pool.hpp`:
    * Main `BS::thread_pool` class:
        * Added a new member function, `push_loop()`, which does the same thing as `parallelize_loop()`, except that it does not return a `BS::multi_future` with the futures for each block. Just like `push_task()` vs. `submit()`, this avoids the overhead of creating the futures, but the user must use `wait_for_tasks()` or some other method to ensure that the loop finishes executing, otherwise bad things will happen.
        * `push_task()` and `submit()` now utilize perfect forwarding in order to support more types of tasks - in particular member functions, which in previous versions could not be submitted unless wrapped in a lambda. To submit a member function, use the syntax `submit(&class::function, &object, args)`. More information can be found in `README.md`. See [#9](https://github.com/bshoshany/thread-pool/issues/9).
        * `push_loop()` and `parallelize_loop()` now have overloads where the first argument (the first index in the loop) is omitted, in which case it is assumed to be 0. This is for convenience, as the case where the first index is 0 is very common.
    * Helper classes:
        * `BS::synced_stream` now utilizes perfect forwarding in the member functions `print()` and `println()`.
        * Previously, it was impossible to pass the flushing manipulators `std::endl` and `std::flush` to `print()` and `println()`, since the compiler could not figure out which template specializations to use. The new objects `BS::endl` and `BS::flush` are explicit casts of these manipulators, whose sole purpose is to enable passing them to `print()` and `println()`.
        * `BS::multi_future::get()` now rethrows exceptions generated by the futures, even if the futures return `void`. See [#62](https://github.com/bshoshany/thread-pool/pull/62).
        * Added a new helper class, `BS::blocks`, which is used by `parallelize_loop()` and `push_loop()` to divide a range into blocks. This class is not documented in `README.md`, as it most likely will not be of interest to most users, but it is still publicly available, in case you want to parallelize something manually but still benefit from the built-in algorithm for splitting a range into blocks.
* `BS_thread_pool_test.cpp`:
    * Added plenty of new tests for the new features described above.
    * Fixed a bug in `count_unique_threads()` that caused it to get stuck on certain systems.
    * `dual_println()` now also flushes the stream using `BS::endl`, so that if the test gets stuck, the log file will still contain everything up to that point. (Note: It is a common misconception that `std::endl` and `'\n'` are interchangeable. `std::endl` not only prints a newline character, it also flushes the stream, which is not always desirable, as it may reduce performance.)
    * The performance test has been modified as follows:
        * Instead of generating random vectors using `std::mersenne_twister_engine`, which proved to be inconsistent across different compilers and systems, the test now generates each element via an arbitrarily-chosen numerical operation. In my testing, this provided much more consistent results.
        * Instead of using a hard-coded vector size, a suitable vector size is now determined dynamically at runtime.
        * Instead of using `parallelize_loop()`, the test now uses the new `push_loop()` function to squeeze out a bit more performance.
        * Instead of setting the test parameters to achieve a fixed single-threaded mean execution time of 300 ms, the test now aims to achieve a fixed multi-threaded mean execution time of 50 ms when the number of blocks is equal to the number of threads. This allows for more reliable results on very fast CPUs with a very large number of threads, where the mean execution time when using all the threads could previously be below a statistically significant value.
        * The number of vectors is now restricted to be a multiple of the number of threads, so that the blocks are always all of the same size.
* `README.md`:
    * Added instructions and examples for the new features described above.
    * Rewrote the documentation for `parallelize_loop()` to make it clearer.

### v3.1.0 (2022-07-13)

* `BS_thread_pool.hpp`:
    * Fixed an issue where `wait_for_tasks()` would sometimes get stuck if `push_task()` was executed immediately before `wait_for_tasks()`.
    * Both the thread pool constructor and the `reset()` member function now determine the number of threads to use in the pool as follows. If the parameter is a positive number, then the pool will be created with this number of threads. If the parameter is non-positive, or a parameter was not supplied, then the pool will be created with the total number of hardware threads available, as obtained from `std::thread::hardware_concurrency()`. If the latter returns a non-positive number for some reason, then the pool will be created with just one thread. See [#51](https://github.com/bshoshany/thread-pool/issues/51) and [#52](https://github.com/bshoshany/thread-pool/issues/52).
    * Added the `[[nodiscard]]` attribute to classes and class members, in order to warn the user when accidentally discarding an important return value, such as a future or the return value of a function with no useful side-effects. For example, if you use `submit()` and don't save the future it returns, the compiler will now generate a warning. (If a future is not needed, then you should use `push_task()` instead.)
    * Removed the `explicit` specifier from all constructors, as it prevented the default constructor from being used with static class members. See [#48](https://github.com/bshoshany/thread-pool/issues/48>).
* `BS_thread_pool_test.cpp`:
    * Improved `count_unique_threads()` using condition variables, to ensure that each thread in the pool runs at least one task regardless of how fast it takes to run the tasks.
    * When appropriate, `check()` now explicitly reports what the obtained result was and what it was expected to be.
    * `check_task_monitoring()` and `check_pausing()` now explicitly report the results of the monitoring at each step.
    * Changed all instances of `std::vector<std::atomic<bool>>` to `std::unique_ptr<std::atomic<bool>[]>`. See [#44](https://github.com/bshoshany/thread-pool/issues/44).
    * Converted a few more C-style casts to C++ cast expressions.
* `README.md`:
    * Added instructions for using this package with the [Conan](https://conan.io/) C/C++ package manager. Please refer to [this package's page on ConanCenter](https://conan.io/center/bshoshany-thread-pool) to learn how to use Conan to include this package in your project with various build systems.
* If you found this project useful, please consider [starring it on GitHub](https://github.com/bshoshany/thread-pool/stargazers)! This allows me to see how many people are using my code, and motivates me to keep working to improve it.

### v3.0.0 (2022-05-30)

* This is a major new release with many changes and improvements! Please note that code written using previous releases will need to be slightly modified to work with the new release. The changes needed to migrate to the new API are explicitly indicated below for your convenience.
* Breaking changes to the library header file:
    * The header file has been renamed to `BS_thread_pool.hpp` to avoid potential conflict with other thread pool libraries.
        * **API migration:** The library must now be included by invoking `#include "BS_thread_pool.hpp"`.
    * All the definitions in the library, including the `thread_pool` class and the helper classes, are now located in the namespace `BS`. This namespace will also be used for my other C++ projects, and is intended to ensure consistency between my projects while avoiding potential name conflicts with other libraries.
        * **API migration:** The thread pool class should now be invoked as `BS::thread_pool`. Alternatively, it is possible to employ `using BS::thread_pool` or even `using namespace BS` and then invoke `thread_pool` directly. Same for the `BS::synced_stream` and `BS::timer` helper classes.
    * The macro `THREAD_POOL_VERSION`, which contains the version number and release date of the library, has been renamed to `BS_THREAD_POOL_VERSION` to avoid potential conflicts.
        * **API migration:** The version must now be read from the macro `BS_THREAD_POOL_VERSION`.
    * The public member `sleep_duration` has been removed. The thread pool now uses condition variables instead of sleep to facilitate waiting. This significantly improves performance (by 10%-50% in my testing), drastically decreases idle CPU utilization, and eliminates the need to set an optimal sleep time. This was a highly-requested change; see [issue #1](https://github.com/bshoshany/thread-pool/issues/1), [issue #12](https://github.com/bshoshany/thread-pool/issues/12), and [pull request #23](https://github.com/bshoshany/thread-pool/pull/23).
        * **API migration:** Remove any code that relates to the public member `sleep_duration`.
    * The template specializations for `submit()` have been merged. Now instead of two versions, one for functions with a return value and one for functions without a return value, there is just one version, which can accept any function. This makes the code more compact (and elegant). If a function with no return value is submitted, an `std::future<void>` is returned (the previous version returned an `std::future<bool>`)
        * **API migration:** To wait for a task with no return value, simply call `wait()` or `get()` on the corresponding `std::future<void>`.
    * `parallelize_loop()` now returns a future in the form of a new `BS::multi_future` helper class template. The member function `wait()` of this future allows waiting until all of the loop's blocks finish executing. In previous versions, calling `parallelize_loop()` both parallelized the loop and waited for the blocks to finish; now it is possible to do other stuff while the loop executes.
        * **API migration:** Since `parallelize_loop()` no longer automatically blocks, you should either store the result in a `BS::multi_future` object and call its `wait()` member function, or simply call `parallelize_loop().wait()` to reproduce the old behavior.
* Non-breaking changes to the library header file:
    * It is now possible to use `parallelize_loop()` with functions that have return values and get these values from all blocks at once through the `get()` member function of the `BS::multi_future`.
    * The template specializations for `push_task()` have been merged. Now instead of two versions, one for functions with arguments and one for functions without arguments, there is just one version, which can accept any function.
    * Constructors have been made `explicit`. See [issue #28](https://github.com/bshoshany/thread-pool/issues/28).
    * `submit()` now uses `std::make_shared` instead of `new` to create the shared pointer. This means only one memory allocation is performed instead of two, which should improve performance. In addition, all unique pointers are now created using `std::make_unique`.
    * A new helper class template, `BS::multi_future`, has been added. It's basically just a wrapper around `std::vector<std::future<T>>`. This class is used by the new implementation of `parallelize_loop()` to allow waiting for the entire loop, consisting of multiple tasks with their corresponding futures, to finish executing.
    * `BS::multi_future` can also be used independently to handle multiple futures at once. For example, you can now keep track of several groups of tasks by storing their futures inside separate `BS::multi_future` objects and use either `wait()` to wait for all tasks in a specific group to finish or `get()` to get an `std::vector` with the return values of every task in the group.
    * Integer types are now chosen in a smarter way to improve portability, allow for better compatibility with 32-bit systems, and prevent potential conversion errors.
    * Added a new type, `BS::concurrency_t`, equal to the return type of `std::thread::hardware_concurrency()`. This is probably pointless, since the C++ standard requires this to be `unsigned int`, but it seems to me to make the code slightly more portable, in case some non-conforming compiler chooses to use a different integer type.
    * C-style casts have been converted to C++ cast expressions for added clarity.
    * Miscellaneous minor optimizations and style improvements.
* Changes to the test program:
    * The program has been renamed to `BS_thread_pool_test.cpp` to avoid potential conflict with other thread pool libraries.
    * The program now returns `EXIT_FAILURE` if any of the tests failed, for automation purposes. See [pull request #42](https://github.com/bshoshany/thread-pool/pull/42).
    * Fixed incorrect check order in `check_task_monitoring()`. See [pull request #43](https://github.com/bshoshany/thread-pool/pull/43).
    * Added a new test for `parallelize_loop()` with a return value.
    * Improved some of the tests to make them more reliable. For example, `count_unique_threads()` now uses futures (stored in a `BS::multi_future<void>` object).
    * The program now uses `std::vector` instead of matrices, for both consistency checks and benchmarks, in order to simplify the code and considerably reduce its length.
    * The benchmarks have been simplified. There's now only one test: filling a specific number of vectors of fixed size with random values. This may be replaced with something more practical in a future released, but at least on the systems I've tested on, it does demonstrate a very significant multi-threading speedup.
    * In addition to multi-threaded tests with different numbers of tasks, the benchmark now also includes a single-threaded test. This allows for more accurate benchmarks compared to previous versions, as the (slight) parallelization overhead is now taken into account when calculating the maximum speedup.
    * The program decides how many vectors to use for benchmarking by testing how many are needed to reach a target duration in the single-threaded test. This ensures that the test takes approximately the same amount of time on different systems, and is thus more consistent and portable.
    * Miscellaneous minor optimizations and style improvements.
* Changes to `README.md`:
    * Many sections have been rewritten and/or polished.
    * Explanations and examples of all the new features have been added.
    * Added an acknowledgements section.
* Miscellaneous changes:
    * Added a `CITATION.bib` file (in BibTeX format) to the GitHub repository. You can use it to easily cite this package if you use it in any research papers.
    * Added a `CITATION.cff` file (in YAML format) to the GitHub repository. This should add [an option to get a citation in different formats](https://docs.github.com/en/github/creating-cloning-and-archiving-repositories/creating-a-repository-on-github/about-citation-files) directly from GitHub repository by clicking on "cite this repository" on the sidebar to the right.
    * Added templates for GitHub issues and pull requests.

### v2.0.0 (2021-08-14)

* From now on, version numbers will adhere to the [Semantic Versioning](https://semver.org/) specification in the format **major.minor.patch**.
* A file named `thread_pool_test.cpp` has been added to the package. It will perform automated tests of all aspects of the package, and benchmark some multithreaded matrix operations. Please run it on your system and [submit a bug report](https://github.com/bshoshany/thread-pool/issues) if any of the tests fail. In addition, the code is thoroughly documented, and is meant to serve as an extensive example of how to properly use the package.
* The package is now available through [vcpkg](https://github.com/microsoft/vcpkg). Instructions for how to install it have been added to `README.md`. See [this pull request](https://github.com/bshoshany/thread-pool/pull/18).
* The package now defines a macro `THREAD_POOL_VERSION`, which returns the version number and release date of the thread pool library as a string.
* `parallelize_loop()` has undergone some major changes (and is now incompatible with v1.x):
    * The second argument is now the index **after** the last index, instead of the last index itself. This is more consistent with C++ conventions (e.g. standard library algorithms) where the range is always `[first, last)`. For example, for an array with `n` indices, instead of `parallelize_loop(0, n - 1, ...)` you should now write `parallelize_loop(0, n, ...)`.
    * The `loop` function is now only called once per block, instead of once per index, as was the case before. This should provide a performance boost due to significantly reducing the number of function calls, and it also allows you to conserve resources by using them only once per block instead of once per index (an example can be found in the `random_matrix_generator` class in `thread_pool_test.cpp`). It also means that `loop` now takes two arguments: the first index in the block and the index after the last index in the block. Thus, `loop(start, end)` should typically involve a loop of the form `for (T i = start; i < end; i++)`.
    * The first and last indices can now be of two different integer types. Previously, `parallelize_loop(0, i, ...)` did not work if `i` was not an `int`, because `0` was interpreted as `int`, and the two arguments had to be of the same type. Therefore, one had to use casting, e.g. `parallelize_loop((size_t)0, i)`, to make it work. Now this is no longer necessary; the common type is inferred automatically using `std::common_type_t`.

### v1.9 (2021-07-29)

* Fixed a bug in `reset()` which caused it to create the wrong number of threads.

### v1.8 (2021-07-28)

* The version history has become too long to be included in `README.md`, so I moved it to a separate file, `CHANGELOG.md`.
* A button to open this repository directly in Visual Studio Code has been added to the badges in `README.md`.
* An internal variable named `promise` has been renamed to `task_promise` to avoid any potential errors in case the user invokes `using namespace std`.
* `submit()` now catches exceptions thrown by the submitted task and forwards them to the future. See [this issue](https://github.com/bshoshany/thread-pool/issues/14).
* Eliminated compiler warnings that appeared when using the `-Weffc++` flag in GCC. See [this pull request](https://github.com/bshoshany/thread-pool/pull/17).

### v1.7 (2021-06-02)

* Fixed a bug in `parallelize_loop()` which prevented it from actually running loops in parallel, see [this issue](https://github.com/bshoshany/thread-pool/issues/11).

### v1.6 (2021-05-26)

* Since MSVC does not interpret `and` as `&&` by default, the previous release did not compile with MSVC unless the `/permissive-` or `/Za` compiler flags were used. This has been fixed in this version, and the code now successfully compiles with GCC, Clang, and MSVC. See [this pull request](https://github.com/bshoshany/thread-pool/pull/10).

### v1.5 (2021-05-07)

* This library now has a DOI for citation purposes. Information on how to cite it in publications has been added to the source code and to `README.md`.
* Added GitHub badges to `README.md`.

### v1.4 (2021-05-05)

* Added three new public member functions to monitor the tasks submitted to the pool:
    * `get_tasks_queued()` gets the number of tasks currently waiting in the queue to be executed by the threads.
    * `get_tasks_running()` gets the number of tasks currently being executed by the threads.
    * `get_tasks_total()` gets the total number of unfinished tasks - either still in the queue, or running in a thread.
    * Note that `get_tasks_running() == get_tasks_total() - get_tasks_queued()`.
    * Renamed the private member variable `tasks_waiting` to `tasks_total` to make its purpose clearer.
* Added an option to temporarily pause the workers:
    * When public member variable `paused` is set to `true`, the workers temporarily stop popping new tasks out of the queue, although any tasks already executed will keep running until they are done. Set to `false` again to resume popping tasks.
    * While the workers are paused, `wait_for_tasks()` will wait for the running tasks instead of all tasks (otherwise it would wait forever).
    * By utilizing the new pausing mechanism, `reset()` can now change the number of threads on-the-fly while there are still tasks waiting in the queue. The new thread pool will resume executing tasks from the queue once it is created.
* `parallelize_loop()` and `wait_for_tasks()` now have the same behavior as the worker function with regards to waiting for tasks to complete. If the relevant tasks are not yet complete, then before checking again, they will sleep for `sleep_duration` microseconds, unless that variable is set to zero, in which case they will call `std::this_thread::yield()`. This should improve performance and reduce CPU usage.
* Merged [this commit](https://github.com/bshoshany/thread-pool/pull/8): Fixed weird error when using MSVC and including `windows.h`.
* The `README.md` file has been reorganized and expanded.

### v1.3 (2021-05-03)

* Fixed [this issue](https://github.com/bshoshany/thread-pool/issues/3): Removed `std::move` from the `return` statement in `push_task()`. This previously generated a `-Wpessimizing-move` warning in Clang. The assembly code generated by the compiler seems to be the same before and after this change, presumably because the compiler eliminates the `std::move` automatically, but this change gets rid of the Clang warning.
* Fixed [this issue](https://github.com/bshoshany/thread-pool/issues/5): Removed a debugging message printed to `std::cout`, which was left in the code by mistake.
* Fixed [this issue](https://github.com/bshoshany/thread-pool/issues/6): `parallelize_loop()` no longer sends references for the variables `start` and `stop` when calling `push_task()`, which may lead to undefined behavior.
* A companion paper is now published at <a href="https://arxiv.org/abs/2105.00613">arXiv:2105.00613</a>, including additional information such as performance tests on systems with up to 80 hardware threads. The `README.md` has been updated, and it is now roughly identical in content to the paper.

### v1.2 (2021-04-29)

* The worker function, which controls the execution of tasks by each thread, now sleeps by default instead of yielding. Previously, when the worker could not find any tasks in the queue, it called `std::this_thread::yield()` and then tried again. However, this caused the workers to have high CPU usage when idle, [as reported by some users](https://github.com/bshoshany/thread-pool/issues/1). Now, when the worker function cannot find a task to run, it instead sleeps for a duration given by the public member variable `sleep_duration` (in microseconds) before checking the queue again. The default value is `1000` microseconds, which I found to be optimal in terms of both CPU usage and performance, but your own optimal value may be different.
* If the constructor is called with an argument of zero for the number of threads, then the default value, `std::thread::hardware_concurrency()`, is used instead.
* Added a simple helper class, `timer`, which can be used to measure execution time for benchmarking purposes.
* Improved and expanded the documentation.

### v1.1 (2021-04-24)

* Cosmetic changes only. Fixed a typo in the Doxygen comments and added a link to the GitHub repository.

### v1.0 (2021-01-15)

* Initial release.
