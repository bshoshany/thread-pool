# `BS::thread_pool`: a fast, lightweight, modern, and easy-to-use C++17 / C++20 / C++23 thread pool library

By **Barak Shoshany**\
Email: <baraksh@gmail.com>\
Website: <https://baraksh.com/>\
GitHub: <https://github.com/bshoshany>

* [Version history](#version-history)
    * [v5.0.0 (2024-12-19)](#v500-2024-12-19)
    * [v4.1.0 (2024-03-22)](#v410-2024-03-22)
    * [v4.0.1 (2023-12-28)](#v401-2023-12-28)
    * [v4.0.0 (2023-12-27)](#v400-2023-12-27)
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

### v5.0.0 (2024-12-19)

* A major new release with many new features, improvements, bug fixes, and performance optimizations! Please note that code written using previous releases may need to be modified to work with the new release. The changes needed to migrate to the new API are explicitly indicated below for your convenience.
* **Highlights:**
    * Added support for C++20 and C++23, while maintaining full C++17 compatibility. In C++20, the library can now optionally be imported as a module using `import BS.thread_pool` on Clang, GCC, and MSVC. In C++23, both the library itself and the test program can now optionally import the C++ Standard Library as a module using `import std` on supported compilers and platforms. Extensive documentation has been added to `README.md` on how to use these features, to ease the transition.
    * Optional features are now enabled via a bitmask template parameter instead of macros, using the flags `BS::tp::priority`, `BS::tp::pause`, and `BS::tp::wait_deadlock_checks`. This makes the optional features easier to use, allows multiple thread pools with different features to coexist, and makes the library compatible with C++20 modules. Exception handling is now disabled automatically if exceptions are disabled, instead of using a macro.
    * Added optional native extensions for non-portable features using the operating system's native API: setting the priority and affinity for processes and threads, and setting thread names. These have been tested on the latest versions of Windows, Ubuntu, and macOS.
    * This library is now back to being a true single-header library, with a single header file `BS_thread_pool.hpp`. The utility classes have been combined into the main header file. `BS::timer` has been removed, `BS::signaller` has been replaced with `BS::binary_semaphore` and `BS::counting_semaphore` (in C++17 mode only), and `BS::synced_stream` now supports multiple output streams.
    * Cleanup functions can now be defined to complement the initialization functions. Both initialization and cleanup functions can now optionally take the index of the thread as an argument.
    * Parallelization member functions no longer need type casting or template parameters if the start and end indices are of different types.
    * The worker function no longer incorrectly reads shared variables while the mutex is unlocked.
    * The type aliases `BS::this_thread::optional_index` and `BS::this_thread::optional_pool` have been removed. Instead, `BS::this_thread::get_index()` returns `std::optional<std::size_t>`, and `BS::this_thread::get_pool()` returns `std::optional<void*>`. The latter must be cast to the correct instantiation of the `BS::thread_pool` class template before using any member functions.
    * The thread pool version is now accessible using the object `BS::thread_pool_version`, a `constexpr struct` of type `BS::version` with the members `major`, `minor`, and `patch`. This works even if importing the library as a C++20 module, unlike the version macros.
    * The type `priority_t`, used to set priorities, is now defined as `std::int8_t`, which means it takes values from -128 to +127. The pre-defined priorities in `BS::pr`, such as `BS::pr::highest` or `BS::pr::lowest`, have been updated accordingly.
    * Exceptions thrown by detached tasks are now caught and prevented from propagating, so that they do not terminate the program. Exceptions thrown by submitted tasks are still rethrown when calling `get()` on the future, as before.
    * Parallelization member functions no longer destruct objects prematurely under certain circumstances.
    * The test program has been expanded with many new tests for both old and new features. It can also import both the thread pool module using `import BS.thread_pool` (in C++20 and later) and the C++ Standard Library module using `import std` (in C++23) if the appropriate macros are defined, and read default command line arguments from a `default_args.txt` file for debugging purposes.
    * Added new and improved benchmarks using a highly-optimized multithreaded algorithm which generates a plot of the Mandelbrot set, utilizing a normalized iteration count algorithm and linear interpolation to create smooth coloring.
    * The type `BS::concurrency_t` has been removed; use `std::size_t` instead.
* **C++20 and C++23 support:**
    * This library now officially supports C++20 and C++23 in addition to C++17. If compiled with C++20 and/or C++23 support (e.g. using the compiler flag `-std=c++23` in Clang/GCC or `/std:c++latest` on MSVC), the library will make use of newly available features for maximum performance, reliability, and usability.
        * To be clear, the library is still fully compatible with any C++17 standard-compliant compiler. I have no plans to remove C++17 support at the moment, as it is still [the most widely used C++ standard](https://www.jetbrains.com/lp/devecosystem-2023/cpp/) among developers, but that might change in the future.
    * If C++20 features are available, the library can be imported as a module using `import BS.thread_pool`. This is now the officially recommended way to use the library, as it has many benefits, such as faster compilation times, better encapsulation, no namespace pollution, no include order issues, easier maintainability, simpler dependency management, and more.
        * The module file itself is `BS.thread_pool.cppm`, located in the `modules` folder, and it is just a thin wrapper around the header file `BS_thread_pool.hpp`.
        * The `constexpr` flag `BS::thread_pool_module` indicates whether the thread pool library was compiled as a module.
        * To my knowledge, `BS::thread_pool` is one of the only popular C++ libraries that are [currently available as a C++20 module](https://arewemodulesyet.org/) (and certainly the only thread pool library). This feature has been tested with the latest versions of Clang, GCC, and MSVC. Unfortunately, C++20 modules are still (4 years later!) not fully implemented in all compilers, and each compiler implements them differently; for instructions on how to compile and import the `BS.thread_pool` module in each compiler, please see `README.md`.
        * Known issues:
            * GCC v14.2.0 (latest version at the time of writing) appears to have an internal compiler error when compiling programs containing modules (or at least, this particular module) with any optimization flags other than `-Og` enabled. Until this is fixed, if you wish to use compiler optimizations, please either include the library as a header file or use a different compiler.
            * On macOS, Apple Clang v16.0.0 (latest version at the time of writing) does not support C++20 modules. Please either install the latest version of LLVM Clang using [Homebrew](https://formulae.brew.sh/formula/llvm), or include the library as a header file.
            * Visual Studio Code's C/C++ extension v1.23.2 (latest version at the time of writing) does not yet support modules. My temporary solution for that, as demonstrated in the test program, is to define the macro `BS_THREAD_POOL_TEST_IMPORT_MODULE` (see below) when compiling the test program, but not when editing in the IDE. If the macro is enabled, the module is imported via `import BS.thread_pool`, otherwise the header file is included using `#include "BS_thread_pool.hpp"` as usual.
    * If C++23 features are available, both the library and the test program can now import the C++ Standard Library as a module using `import std`. To enable this, define the macro `BS_THREAD_POOL_IMPORT_STD` at compilation time. This is currently only officially supported by recent versions of MSVC with Microsoft STL or LLVM Clang (**not** Apple Clang) with LLVM libc++. It is not supported by GCC with any standard library, Clang with any standard library other than libc++, any compiler with GNU libstdc++, or any other compiler.
        * If `BS_THREAD_POOL_IMPORT_STD` is defined, then you must also import the library itself as a module. If the library is included as a header file, this will force the program that included the header file to also import `std`, which is not desirable and can lead to compilation errors if the program `#include`s any Standard Library header files.
        * Defining the macro before importing the module will not work, as modules cannot access macros defined in the program that imported them. Instead, define the macro as a compiler flag, e.g. `-D BS_THREAD_POOL_IMPORT_STD` (or `/D` for MSVC).
        * The `constexpr` flag `BS::thread_pool_import_std` indicates whether the thread pool library was compiled with `import std`. Note that the flag will be `false` if `BS_THREAD_POOL_IMPORT_STD` is defined but the compiler or standard library does not support importing the C++ Standard Library as a module.
    * If C++20 features are available, the pool will use `std::jthread` instead of `std::thread`. This allows considerable simplification and added safety, since the threads no longer need to be manually joined, and `std::stop_token` is used to stop the workers automatically when destructing the threads. This eliminates the need for the `destroy_threads()` member function, as well as the `workers_running` flag, which are now only used in C++17 mode.
    * If C++20 features are available, the library will use concepts to enforce the signature of the initialization function and to selectively enable member functions related to pausing only if pausing is enabled. In C++17 mode, the library will use SFINAE to achieve essentially the same effect.
    * If C++23 features are available, the task queue will use `std::move_only_function<void()>` instead of `std::function<void()>`. This allows `submit_task()` to work without using a shared pointer, which should increase performance.
    * **API migration:** All of the C++20/C++23 features listed above are either automatically applied based on compiler settings or optional. If you are still using C++17, or if you are using C++20 or C++23 but do not wish to import the thread pool library and/or the C++ Standard Library as a module, no changes are needed.
* **Optional features overhaul:**
    * All optional features are now enabled via a bitmask template parameter instead of macros. This works using `if constexpr`, `std::conditional_t`, and concepts (in C++20 and later) or SFINAE (in C++17).
        * This change makes the optional features much easier and more intuitive to use, as you no longer need to define any macros before including the header file.
        * Additionally, it allows you to have multiple thread pools in the same program with different optional features enabled or disabled. For example, you can have one pool with task priority enabled and another without.
        * Most importantly, this makes it possible to import the library as a C++20 module, as macros cannot be read by imported modules.
    * The bitmask flags are members of the `BS::tp` enumeration:
        * `BS::tp::priority` enables task priority (previously enabled via the macro `BS_THREAD_POOL_ENABLE_PRIORITY`, which has been removed).
        * `BS::tp::pause` enables pausing the pool (previously enabled via the macro `BS_THREAD_POOL_ENABLE_PAUSE`, which has been removed).
        * `BS::tp::wait_deadlock_checks` enables deadlock checks in `wait()`/`wait_for()`/`wait_until()` (previously enabled via the macro `BS_THREAD_POOL_ENABLE_WAIT_DEADLOCK_CHECK`, which has been removed).
        * The default is `BS::tp::none`, which disables all optional features.
    * Convenience aliases are defined as follows:
        * `BS::light_thread_pool` disables all optional features (equivalent to `BS::thread_pool` with the default template parameter, that is, `BS::thread_pool<BS::tp::none>`).
        * `BS::priority_thread_pool` enables task priority (equivalent to `BS::thread_pool<BS::tp::priority>`).
        * `BS::pause_thread_pool` enables pausing the pool (equivalent to `BS::thread_pool<BS::tp::pause>`).
        * `BS::wdc_thread_pool` enables wait deadlock checks (equivalent to `BS::thread_pool<BS::tp::wait_deadlock_checks>`).
        * There are no aliases with multiple features enabled; if this is desired, you must either pass the template parameter explicitly or define your own alias. Note that the parameter is a bitmask, so to enable multiple features, you need to use the bitwise OR operator `|`, e.g. `BS::thread_pool<BS::tp::priority | BS::tp::pause>` to enable both task priority and pausing.
    * The macro `BS_THREAD_POOL_DISABLE_EXCEPTION_HANDLING` has been removed. Exception handling is disabled automatically if exceptions are disabled, based on whether the feature-test macro `__cpp_exceptions` is defined.
    * The exception thrown by wait deadlock checks is now `BS::wait_deadlock` instead of `BS::thread_pool::wait_deadlock`, to avoid having to deal with different template parameters.
    * The macro `BS_THREAD_POOL_LIGHT_TEST` has been removed from the test program, as all optional features are now tested by enabling them selectively via the template parameter, so there is no need to compile with different macros.
    * If for some reason you forgot which options you enabled when creating the pool, the `static constexpr` members `priority_enabled`, `pause_enabled`, and `wait_deadlock_checks_enabled` can be used to check if the corresponding features are enabled.
    * **API migration:**
        * `BS::thread_pool` can still be used without the template parameter, for backwards compatibility; this will create a thread pool with all optional features disabled. Therefore, if you did not use any of the optional features in existing code, no changes are needed.
        * If your code uses any of the optional features by defining macros before including the header file, please remove these macros, and instead either use one of the convenience aliases above or define the template parameter explicitly using the `BS::tp` enumeration when creating the pool.
        * If you use wait deadlock checks, you must now catch the exception `BS::wait_deadlock` instead of `BS::thread_pool::wait_deadlock`.
* **Native extensions:**
    * While portability is one of my guiding principle when developing this library, non-portable features such as setting the thread priority using the operating system's native API are frequently requested by users. Starting with this release, the library includes native extensions, which are disabled by default.
    * Currently, the extensions provide the following functions (please see `README.md` for details on how to use them):
        * `BS::get_os_process_affinity()` and `BS::set_os_process_affinity()` to get and set the CPU affinity of the current process in a portable way. Should work on Windows and Linux, but not on macOS, as the native API does not allow it.
        * `BS::get_os_process_priority()` and `BS::set_os_process_priority()` to get and set the priority of the current process in a portable way. Should work on Windows, Linux, and macOS.
        * `BS::this_thread::get_os_thread_affinity()` and `BS::this_thread::set_os_thread_affinity()` to get and set the CPU affinity of the current thread in a portable way. Should work on Windows and Linux, but not on macOS, as the native API does not allow it.
        * `BS::this_thread::get_os_thread_priority()` and `BS::this_thread::set_os_thread_priority()` to get and set the priority of the current thread in a portable way. Should work on Windows, Linux, and macOS.
        * `BS::this_thread::get_os_thread_name()` and `BS::this_thread::set_os_thread_name()` to get and set the name of the current thread in a portable way, for debugging purposes. Should work on Windows, Linux, and macOS.
    * The native extensions may be enabled by defining the macro `BS_THREAD_POOL_NATIVE_EXTENSIONS` at compilation time.
        * Even if the macro is defined, the extensions are disabled automatically if a supported operating system (Windows, Linux, or macOS) is not detected.
        * Note that if you are using the library as a C++20 module, defining the macro before importing the module will not work, as modules cannot access macros defined in the program that imported them. Instead, define the macro as a compiler flag, e.g. `-D BS_THREAD_POOL_NATIVE_EXTENSIONS` (or `/D` for MSVC).
    * The macro `BS_THREAD_POOL_ENABLE_NATIVE_HANDLES` has been removed. The thread pool member function `get_native_handles()` is now part of the native extensions, so it is enabled using the macro `BS_THREAD_POOL_NATIVE_EXTENSIONS`.
    * Please note that the native extensions have only been tested on Windows 11 23H2, Ubuntu 24.10, and macOS 15.1. They have not been tested on older versions of these operating systems, other Linux distributions, or any other operating systems, and are therefore not guaranteed to work on every system. If you encounter any issues, please report them on the GitHub repository.
    * The test program only tests the native extensions if the macro `BS_THREAD_POOL_NATIVE_EXTENSIONS` is defined at compilation time. If importing the library as a module, please ensure that the macro is also enabled when compiling the module.
    * The `constexpr` flag `BS::thread_pool_native_extensions` indicates whether the thread pool library was compiled with native extensions enabled. Note that the flag will be `false` if `BS_THREAD_POOL_NATIVE_EXTENSIONS` is defined but the operating system is unsupported.
    * **API migration:** The native extensions are a brand new optional feature and do not require any changes to existing code.
* **Utility classes:**
    * This library is now back to being a true single-header library, with a single header file `BS_thread_pool.hpp`. The utility classes (previously in a separate header `BS_thread_pool_utils.hpp`, which has been removed) have been combined into the main header file.
    * The `BS::timer` class has been removed from the library, since it doesn't really have anything to do with multithreading directly. However, it is still available in the test program if you want to use it.
    * The `BS::signaller` class has been removed from the library, and replaced with `BS::binary_semaphore` and `BS::counting_semaphore`, which are C++17 polyfills for the C++20 classes `std::binary_semaphore` and `std::counting_semaphore`. If C++20 features are available, the polyfills are not used, and instead are just aliases for the standard library classes. The reason is that semaphores can do the same thing that the signaller class was previously used for, but are much more versatile.
    * The `BS::synced_stream` class now supports printing to more than one output stream.
    * **API migration:**
        * If you previously included the `BS_thread_pool_utils.hpp` header file, this is no longer needed. Only include the header `BS_thread_pool.hpp`, or better yet, in C++20 or later, import the library as a module using `import BS.thread_pool`.
        * If you previously used the `BS::timer` class, it is no longer available in the header file, but if you still need it you can copy it into your program directly from the test program `BS_thread_pool_test.cpp`.
        * If you previously used the `BS::signaller` class, you can replace it with `BS::binary_semaphore` or `BS::counting_semaphore`. Previously, you defined an object `BS::signaller signal`, and then used `signal.wait()` to wait for the signal, and `signal.ready()` to unblock all waiting threads. Now, you can define an object `BS::counting_semaphore signal(0)`, and use `signal.acquire()` to wait for the signal, and `signal.release(num_threads)` to unblock waiting threads; note that the number of threads to release must be passed explicitly, as the semaphore also allows you to unblock only some of them. Use `BS::binary_semaphore` if only one thread will be waiting at any given time.
        * If you previously used the `BS::synced_stream` class, no changes are needed.
* **Cleanup and initialization functions:**
    * Using the new `set_cleanup_func()` member function, it is now possible to provide the pool with a cleanup function to run in each thread right before it is destroyed, which will happen when the pool is destructed or reset. See [#152](https://github.com/bshoshany/thread-pool/issues/152).
    * Both initialization and cleanup functions can now optionally take the index of the thread as an argument.
    * Added a warning in the documentation that both initialization and cleanup functions must not throw any exceptions, as that will result in program termination. Any exceptions must be handled explicitly within the function.
    * **API migration:** No changes to existing code are needed.
* **Parallelization index types:**
    * All member functions which parallelize collections of tasks, namely `detach_blocks()`, `detach_loop()`, `detach_sequence()`, `submit_blocks()`, `submit_loop()`, and `submit_sequence()`, can now be called with start and end indices of different types.
    * Previously, the indices had to be of the same type, or the template parameter had to be explicitly specified; this is no longer needed, as the library will automatically cast the indices to a suitable common type.
    * This was already possible in v2.X.X and v3.X.X, where it was done using [`std::common_type`](https://en.cppreference.com/w/cpp/types/common_type), but I removed it in v4.X.X because `std::common_type` sometimes completely messed up the range of the loop. For example, the `std::common_type` of `int` and `unsigned int` is `unsigned int`, which means the loop will only use non-negative indices even if the `int` start index was negative, resulting in an integer overflow.
    * Starting with v5.0.0, the library uses a custom type trait `BS::common_index_type` to determine the common type of the indices. The common type of two signed integers or two unsigned integers is the larger of the integers, while the common type of a signed and an unsigned integer is a signed integer that can hold the full ranges of both integers. This avoids messing up the indices, except in the case of `std::uint64_t`, where there is no fundamental signed type that can hold its entire range. In this case, we choose `std::uint64_t` as the common type, since the most common use case is where the indices go from 0 to `x` where `x` has been previously defined as `std::size_t`. This will fail if the first index is negative; in that case, the user must cast the indices explicitly.
    * **API migration:** Existing code which uses type casting or explicit template parameters in parallelization functions does not need to be changed, but it can be simplified by removing the casting or template parameters. However, if one index is negative and the other is an unsigned 64-bit integer, casting is still needed (although you should probably not be doing this in the first place, as casting to either of the two types will result in potential narrowing or overflow).
* **`BS::this_thread`:**
    * `BS::this_thread` is now a class instead of a namespace, since defining it as a namespace proved to be incompatible with C++20 modules (at least in some compilers). Defining it as a class also results in a simpler implementation. However, the functionality remains the same, and since it only has static methods, the call syntax for `BS::this_thread::get_index()` and `BS::this_thread::get_pool()` is unchanged.
    * The type aliases `BS::this_thread::optional_index` and `BS::this_thread::optional_pool` have been removed. Instead, `BS::this_thread::get_index()` now returns the explicit type `std::optional<std::size_t>`, and `BS::this_thread::get_pool()` returns `std::optional<void*>`.
        * The rationale for this removal is that using `std::optional` explicitly provides more information about the type that is being returned, and most users are probably not using the explicit types anyway (either by using `auto` or by invoking the `std::optional` member functions directly on the returned object).
    * Note that `BS::this_thread::get_pool()` now returns an optional `void*` instead of `BS::thread_pool*`. The reason for that is that `BS::thread_pool` is now a template. Once you obtain the pool pointer, you must cast it to the desired instantiation of the template if you want to use any member functions. Note that you have to cast it to the correct type; if you cast a pointer to a `BS::light_thread_pool` into a pointer to a `BS::priority_thread_pool`, for example, your program will have undefined behavior.
    * **API migration:**
        * If your code uses the type aliases, please replace `BS::this_thread::optional_index` with `std::optional<std::size_t>` and `BS::this_thread::optional_pool` with `std::optional<void*>`.
        * If your code uses `BS::this_thread::get_pool()`, you must now cast the returned pointer to the correct instantiation of the `BS::thread_pool` class template before using any member functions.
* **Determining the library version:**
    * The library now defines the `constexpr` object `BS::thread_pool_version`, which can be used to check the version of the library at compilation time. This object is of type `BS::version`, with members `major`, `minor`, and `patch`, and all comparison operators defined as `constexpr`. It also has a `to_string()` member function and an `operator<<` overload for easy printing at runtime. For example, you can do `static_assert(BS::thread_pool_version == BS::version(5, 0, 0))`, or you can use it in `if constexpr` for conditional compilation.
    * The version macros `BS_THREAD_POOL_VERSION_MAJOR`, `BS_THREAD_POOL_VERSION_MINOR`, and `BS_THREAD_POOL_VERSION_PATCH` are still defined, since they can be used in conditional code inclusion, and for backwards compatibility. However, since C++20 modules cannot export macros, `BS::thread_pool_version` is the only way to check the version of the thread pool library if you are importing it as a module.
    * **API migration:** No changes needed in existing code; if you previously used the macros `BS_THREAD_POOL_VERSION_MAJOR`, `BS_THREAD_POOL_VERSION_MINOR`, and `BS_THREAD_POOL_VERSION_PATCH` to determine the version of the library when including it as a header file, you can still do so. However, if you wish to import the library as a C++20 module, you must use the object `BS::thread_pool_version` instead.
* **Task priority:**
    * The type `priority_t`, used to set priorities, is now defined as `std::int8_t`, which means it takes values from -128 to +127. The pre-defined priorities in `BS::pr`, such as `BS::pr::highest` or `BS::pr::lowest`, have been updated accordingly (also, it is now an `enum` instead of a namespace). The old priority type `std::int16_t` was unnecessarily large; having fewer priority values means less bookkeeping in the priority queue, which should also improve performance.
    * **API migration:** If you used the pre-defined priorities in `BS::pr`, no changes are needed. If you specified numerical priorities directly, you may need to adjust them to the new range of -128 to +127.
* **Miscellaneous:**
    * Exceptions thrown by detached tasks are now caught and prevented from propagating, so that they do not terminate the program. Exceptions thrown by submitted tasks are still rethrown when calling `get()` on the future, as before.
    * All member functions which parallelize collections of tasks, namely `detach_blocks()`, `detach_loop()`, `detach_sequence()`, `submit_blocks()`, `submit_loop()`, and `submit_sequence()`, now store the callable object inside an `std::shared_ptr`, and then pass that shared pointer to each subtask. Previously, the callable was passed using perfect forwarding, which under some circumstances resulted in mistakenly moving the callable during the first iteration of the loop, thus potentially destructing captured objects prematurely. The new shared pointer method resolves this issue, while also avoiding making copies of the callable. See [#149](https://github.com/bshoshany/thread-pool/issues/149).
    * Fixed incorrect reading of shared variables while the mutex is unlocked in the worker function. See [#159](https://github.com/bshoshany/thread-pool/issues/159).
    * Added documentation to `README.md` for all the new features. In addition, fixed some typos and other minor issues in the existing documentation.
    * Added instructions in `README.md` for installing the library using CMake with `FetchContent` instead of CPM. See [#155](https://github.com/bshoshany/thread-pool/pull/155).
    * The type `BS::concurrency_t` has been removed. In previous versions this type was defined to be the type of the value returned by `std::thread::hardware_concurrency()` (which is supposed to be `unsigned int`), for maximum portability. However, in practice this value is only used to indicate the size of arrays, so `std::size_t` is more appropriate, and this simplifies the code.
    * **API migration:** If you used `BS::concurrency_t` in your code, please replace it with `std::size_t`. If you previously cast to/from these two types, you can now remove the cast.
* **Tests:**
    * The test program `BS_thread_pool_test.cpp` will import the library as a C++20 module via `import BS.thread_pool` if the macro `BS_THREAD_POOL_TEST_IMPORT_MODULE` is defined, C++20 or later is detected, and a supported compiler is used.
    * The test program will also import the C++ Standard Library as a module using `import std` if the macro `BS_THREAD_POOL_IMPORT_STD` is defined during compilation, on supported compilers and platforms.
    * The new test `check_copy()` checks that the callable object does not get copied when parallelized into multiple tasks. It will succeed on previous versions of the library, but not if perfect forwarding is removed.
    * The new test `check_shared_ptr()` checks that captured shared pointers do not prematurely destruct. It will fail on previous versions.
    * The new test `check_task_destruct()` checks that a task is destructed immediately after it executes, and therefore does not artificially extend the lifetime of any captured objects.
    * The new test `check_common_index_type()` checks that the type trait `BS::common_index_type` (see above) works as expected.
    * The new tests `check_os_process_priorities()`, `check_os_thread_priorities()`, `check_os_process_affinity()`, `check_os_thread_affinity()`, and `check_os_thread_names()` check the corresponding features of the native extensions.
    * The new test `check_callables()` checks that different callable types are accepted by the thread pool.
    * New command line argument: `stdout`, to print to the standard output, enabled by default.
    * If the file `default_args.txt` exists in the same folder, the test program reads the default arguments from it (space separated in a single line). Command line arguments can still override these defaults. This is useful when debugging.
    * The test program will now detect and log the OS, compiler, standard library, C++ standard, available C++ features, whether the thread pool was imported as a C++20 module, and whether the standard library was imported as a module.
* **Benchmarks:**
    * Added new and improved benchmarks using a highly-optimized multithreaded algorithm which generates a plot of the Mandelbrot set, utilizing a normalized iteration count algorithm and linear interpolation to create smooth coloring.
    * These benchmarks are heavily CPU-intensive, and much less limited by memory and cache compared to the benchmarks in previous versions (which used vector or matrix operations). This results in a much higher speedup factor due to multithreading, utilizing every core and thread to their fullest extent. This makes these benchmarks more useful for optimizing the library, since they are more sensitive to the thread pool's own performance.
    * The full benchmarks are enabled using the command line argument `benchmarks`, which is enabled by default. The command line argument `plot` can be used to just plot the Mandelbrot set once, either instead of or in addition to doing the full benchmarks. This will plot the largest possible image that can be plotted in 5 seconds, and only measure the performance in pixels/ms for the entire plot.
    * If you want to see the actual plot, pass the `save` command line argument. The plot is saved to a BMP file, since I didn't want to depend on any 3rd-party libraries. This is off by default, since that file can get quite large.
* **Development:**
    * A Python script `compile_cpp.py` has been added to the repository, in the `scripts` folder. It can be used to compile any C++ source file with different compilers on different platforms. The compilation parameters can be configured using command line arguments and/or via an optional YAML configuration file `compile_cpp.yaml` which specifies defined macros, extra compiler flags (per compiler), include folders, modules, and the output folder.
    * I wrote this script to make it easier for me to test the library with different combinations of compilers, standards, and platforms using the built-in Visual Studio Code tasks. I also included three `.vscode` folders (one for each OS) in the repository, with appropriate `c_cpp_properties.json`, `launch.json`, and `tasks.json` files that utilize this script, in case you want to use it in your own projects. However, note that this script is not meant to replace CMake or any full-fledged build system, it's just a convenient script for developing single-header libraries like this one or other small projects.
    * The `compile_cpp.py` script also transparently handles C++20 modules and importing the C++ Standard Library as a module in C++23. Therefore, users of this library who wish to import it as a C+20 module may find this script particularly useful.
    * Another Python script `test_all.py` in the `scripts` folder replaces the old PowerShell test script. Tests are now performed in C++17, C++20, and C++23 modes, using all compilers available in the system (Clang, GCC, and/or MSVC). Since there are so many tests, the test script now no longer performs the benchmarks, as that would take too long.
    * A final Python script `clear_folder.py` in the `scripts` folder is used to clean up output and temporary folders, and integrates with VS Code tasks.

### v4.1.0 (2024-03-22)

* This library is now published in [SoftwareX](https://www.sciencedirect.com/journal/softwarex)! If you use it in published research, please cite it as follows: Barak Shoshany, *"A C++17 Thread Pool for High-Performance Scientific Computing"*, [doi:10.1016/j.softx.2024.101687](https://doi.org/10.1016/j.softx.2024.101687), [SoftwareX 26 (2024) 101687](https://www.sciencedirect.com/science/article/pii/S235271102400058X), [arXiv:2105.00613](https://arxiv.org/abs/2105.00613)
    * Updated the source files, as well as `README.md`, `CITATION.bib`, and `CITATION.cff` with the new citation.
* A new macro, `BS_THREAD_POOL_DISABLE_EXCEPTION_HANDLING`, allows the user to disable exception handling in `submit_task()` if it is not needed, or if exceptions are explicitly disabled in the codebase. See [#139](https://github.com/bshoshany/thread-pool/issues/139).
    * Note that this macro can be defined independently of `BS_THREAD_POOL_ENABLE_WAIT_DEADLOCK_CHECK`. Disabling exception handling removes the `try`-`catch` block from `submit_task()`, while enabling wait deadlock checks adds a `throw` expression to `wait()`, `wait_for()`, and `wait_until()`.
    * If the feature-test macro `__cpp_exceptions` is undefined, `BS_THREAD_POOL_DISABLE_EXCEPTION_HANDLING` is automatically defined, and `BS_THREAD_POOL_ENABLE_WAIT_DEADLOCK_CHECK` is automatically undefined.
* Replaced `#pragma once` with old-school include guards using the macros `BS_THREAD_POOL_HPP` and `BS_THREAD_POOL_UTILS_HPP`. There are two main reasons for this:
    1. Even though `#pragma once` is supported by the vast majority of modern compilers, it is still a non-standard feature, so using it technically made the library not standards compliant.
    2. Include guards make it possible to include the library twice in the same project (for example, once with priority enabled and once without) by undefining the include guard and putting the second include in its own namespace.
* Included a description of the destructor behavior for the `BS::thread_pool` class in `README.md`, in the library reference section. See [#143](https://github.com/bshoshany/thread-pool/issues/143).
* Removed unnecessary locking in `reset()` if pausing is not enabled.

### v4.0.1 (2023-12-28)

* Fixed linkage issue caused by the global variables `BS::this_thread::get_index` and `BS::this_thread::get_pool` not being defined as `inline`. See [#134](https://github.com/bshoshany/thread-pool/issues/134) and [137](https://github.com/bshoshany/thread-pool/issues/137).
* Fixed redundant cast in the `BS::thread_pool::blocks` class, and added `-Wuseless-cast` to the GCC warning flags in `BS_thread_pool_test.ps1` to catch similar issues in the future. See [#133](https://github.com/bshoshany/thread-pool/pull/133).
* Each of the three files `BS_thread_pool_test.cpp`, `BS_thread_pool.hpp`, and `BS_thread_pool_utils.hpp` now contains three macros indicating the major, minor, and patch version of the file. In addition, `BS_thread_pool_test.cpp` now checks whether the versions of all three files match, and aborts compilation if they do not.

### v4.0.0 (2023-12-27)

* A major new release with numerous changes, additions, fixes, and improvements. Many frequently requested features have been added, and performance has been optimized. Please note that code written using previous releases will need to be modified to work with the new release. The changes needed to migrate to the new API are explicitly indicated below for your convenience.
* Highlights:
    * The light thread pool has been removed. However, by default, the thread pool is in "light mode". Optional features that may affect performance must be enabled by defining suitable macros.
    * This library now ships with two stand-alone header files:
        * `BS_thread_pool.hpp` contains the main `BS::thread_pool` class and the `BS::multi_future` helper classes, and is the only file needed to use the thread pool itself.
        * `BS_thread_pool_utils.hpp` contains the additional utility classes `BS::signaller`, `BS::synced_stream`, and `BS::timer`, which are fully independent of the thread pool itself and can be used either with or without it.
    * It is now possible to assign priorities to tasks. Tasks with higher priorities will be executed first.
    * Member functions for submitting tasks and loops have been renamed for consistency, e.g. `detach_task()` and `submit_task()`, where the prefix `detach` means no future will be returned and `submit` means a future or `BS::multi_future` will be returned.
    * There are now two ways to parallelize loops into blocks:
        * `detach_blocks()` and `submit_blocks()` behave the same as loop parallelization in previous releases, running the loop function once per block.
        * `detach_loop()` and `submit_loop()` have a simpler syntax, where the loop function is run once per index, so the user doesn't have to manually run the internal loop for each block.
    * The new member functions `detach_sequence()` and `submit_sequence()` allow submitting a sequence of tasks enumerated by indices.
    * It is now possible to run an initialization function in each thread before it starts to execute any submitted tasks.
    * Tasks submitted with `detach_task()` or `submit_task()` can no longer have arguments. Task with arguments must be enclosed inside lambda expressions. This simplifies the API and provides better readability. Tasks can still have return values.
    * Various ways to obtain information about the threads in the pool have been introduced:
        * The member function `get_thread_ids()` obtains the unique thread identifiers, and `get_native_handles()` obtains the underlying implementation-defined thread handles.
        * The new namespace `BS::this_thread` allows obtaining the thread's index in the pool using `BS::this_thread::get_index()` and a pointer to the pool that owns the thread using `BS::this_thread::get_pool()`.
    * Member functions for waiting for tasks have been renamed for brevity: `wait()`/`wait_for()`/`wait_until()`. In addition, these functions can now optionally throw an exception if the user tries to call them from within a thread of the same pool, which would result in a deadlock.
    * The first index must now be specified explicitly when parallelizing blocks, loops, and sequences, and it must not be greater than the last index. Also, both indices must now have the same type, or the template parameter should be explicitly specified.
    * Optimized the way `detach_blocks()`, `submit_blocks()`, `detach_loop()`, and `submit_loop()` split the range of the loop into blocks.
    * Added a utility class `BS::signaller` to allow simple signalling between threads.
    * `BS::multi_future<T>` is now a specialization of `std::vector<std::future<T>>` with additional member functions.
* Breaking changes:
    * The light thread pool has been removed. The original idea was that the light thread pool will allow the user to sacrifice functionality for increased performance. However, in my testing I found that there was no actual performance benefit to the light thread pool. Therefore, there is no reason to keep it.
        * However, by default, the thread pool is in "light mode". Optional features that may affect performance due to additional checks or more complicated algorithms must be enabled by defining suitable macros before including the library:
            * `BS_THREAD_POOL_ENABLE_PAUSE` to enable pausing.
            * `BS_THREAD_POOL_ENABLE_PRIORITY` to enable task priority.
            * `BS_THREAD_POOL_ENABLE_WAIT_DEADLOCK_CHECK` to enable wait deadlock checks.
        * **API migration:**
            * If you previously used `BS_thread_pool_light.hpp`, simply use `BS_thread_pool.hpp` instead.
            * If you previously used the pausing feature, define the macro `BS_THREAD_POOL_ENABLE_PAUSE` before including `BS_thread_pool.hpp` to enable it.
    * Member functions have been renamed for better consistency. Each function has a `detach` variant which does not return a future, and a `submit` variant which does return a future (or a `BS::multi_future`):
        * `detach_task()` and `submit_task()` for single tasks.
        * `detach_blocks()` and `submit_blocks()` for loops to be split into blocks, where the loop function is executed once per block and must have an internal loop, as in previous releases.
        * `detach_loop()` and `submit_loop()` for loops to be split into blocks, where the loop function is executed once per index and the pool takes care of the internal loop.
        * `detach_sequence()` and `submit_sequence()` for sequences of enumerated tasks.
        * **API migration:** Use the new names of the functions:
            * `push_task()` -> `detach_task()`
            * `submit()` -> `submit_task()`
            * `push_loop()` -> `detach_blocks()`
            * `parallelize_loop()` -> `submit_blocks()`
    * `wait_for_tasks()`, `wait_for_tasks_duration()`, and `wait_for_tasks_until()` have been renamed to `wait()`, `wait_for()`, and `wait_until()` respectively.
        * **API migration:** Use the new names of the functions:
            * `wait_for_tasks()` -> `wait()`
            * `wait_for_tasks_duration()` -> `wait_for()`
            * `wait_for_tasks_until()` -> `wait_until()`
    * Functions for parallelizing loops no longer have dedicated overloads for the special case where the first index is 0. These overloads essentially amount to giving the first function argument a default value, which is not allowed in C++, and can be confusing. In addition, indicating the first index explicitly is better for readability.
        * **API migration:** Add the first index 0 manually as the first argument if it was omitted.
    * Functions for parallelizing loops no longer allow the last index to be smaller than the first index. Previously, e.g. `detach_blocks(5, 0, ...)` was equivalent to `detach_blocks(0, 5, ...)`. However, this led to confusing results. Since the first argument is the first index and the second argument is the index *after* the last index (i.e. 0 to 5 actually means 0, 1, 2, 3, 4), the user might get the wrong impression that `detach_blocks(5, 0, ...)` will count 5, 4, 3, 2, 1 instead. This option was removed to avoid this confusion.
        * Sometimes the user might actually want to make a loop that counts down instead of up. This cannot be done by flipping the order of the arguments to e.g. `detach_blocks()` (nor could it be done in previous releases). However, it can be done by simply defining a suitable loop function. For example, if you call `detach_blocks(0, 10, loop, 2)` and define the loop function as `for (T i = 9 - start; i > 9 - end; --i)`, then the first block will count 9, 8, 7, 6, 5 and the second block will count 4, 3, 2, 1, 0.
        * `detach_loop()`, `submit_loop()`, `detach_sequence()`, and `submit_sequence()` work the same way. The first index must be smaller than the last index, but you can count down by writing a suitable loop or sequence function.
        * **API migration:** Any loop parallelization that used a first index greater than the last index will work exactly the same after switching the first and second arguments so that the smaller index appears first.
    * Functions for parallelizing loops no longer accept first and last indices of different types. The reason for allowing this previously was that otherwise, writing something like `detach_blocks(0, x, ...)` where `x` is not an `int` would result in a compilation error, since `0` is by default an `int` and therefore the arguments `0` and `x` have different types. However, this behavior, which used [`std::common_type`](https://en.cppreference.com/w/cpp/types/common_type) to determine the common type of the two indices, sometimes completely messed up the range of the loop. For example, the `std::common_type` of `int` and `unsigned int` is `unsigned int`, which means the loop will only use non-negative indices even if the `int` start index was negative, resulting in an integer overflow.
        * **API migration:** If you want to invoke e.g. `detach_blocks(0, x, ...)` where `x` is not an `int`, you can either:
            * Make the `0` have the desired type using a cast or a suffix. For example, if `x` is an `unsigned int`, write `(unsigned int)0` or `0U` instead of `0`.
            * Specify the template parameter explicitly. For example, if `x` is a `size_t`, write `detach_blocks<size_t>(0, x, ...)`.
    * `detach_task()` and `submit_task()` no longer accept arguments for the submitted task. Instead, you must enclose the function in a [lambda expression](https://en.cppreference.com/w/cpp/language/lambda). In other words, instead of `detach_task(task, args...)` you should write `detach_task([] { task(args...); })`, indicating in the capture list `[]` whether to capture the task itself, and each of the arguments, by value or reference. Please see `README.md` for examples. This was changed for the following reasons:
        1. Consistency with `detach_blocks()` and `submit_blocks()`, as well as the new `detach_loop()`, `submit_loop()`, `detach_sequence()`, and `submit_sequence()`, which do not accept function arguments either.
        2. In my own multithreaded projects, I find that I almost always need the task to have access to variables in the local scope. This is much simpler, easier, and more concise to do with a lambda capture list, especially an implicit capture `[=]` or `[&]`, than by defining a function that takes arguments and then passing these arguments.
        3. Similarly, I find that I mostly submit tasks defined as a lambda on the spot, rather than creating them as separate functions, because it's faster to code and makes it clear exactly what the task does without having to look elsewhere.
        4. When users post issues to this repository asking for help with their own code that uses the thread pool, the solution often turns out to be "just wrap that in a lambda". Such issues can be avoided if lambdas must be used to begin with.
        5. Submitting member functions, which previously required the awkward syntax `detach_task(&class::function, &object, args...)`, can now be achieved with the much simpler and more readable syntax `detach_task([] { object.function(args...); })` with the appropriate captures.
        6. Passing arguments by reference, which previously required using [`std::ref`](https://en.cppreference.com/w/cpp/utility/functional/ref), e.g. `detach_task(task, std::ref(arg))`, can now be achieved with the much simpler and more readable syntax `detach_task([&arg] { task(arg); })`.
        7. The new syntax allows specifying the priority of the task easily, as the second argument - otherwise, it would have been hard to distinguish the priority from a task argument, making the API more complicated and confusing. This syntax will also permit adding additional arguments to the member functions as needed in the future.
        * **API migration:** Enclose all tasks with arguments inside a lambda expression. All submitted tasks must have no arguments, but they can still have return values.
            * Alternatively, [`std::bind`](https://en.cppreference.com/w/cpp/utility/functional/bind) can also be used, if the old syntax is preferred to a lambda. Just wrap it around the task and its arguments: instead of `detach_task(task, args...)`, write `detach_task(std::bind(task, args...))`. This achieves the same effect, and can be used to easily convert v3.x.x code to v4.0.0 using a simple regular expression search and replace:
                * `push_task\((.*?)\)` -> `detach_task(std::bind($1))`
                * `submit\((.*?)\)` -> `submit_task(std::bind($1))`
    * `BS::synced_stream` and `BS::timer` have been moved to `BS_thread_pool_utils.hpp`.
        * **API migration:** Include the new header file if either of these utility classes are used.
* `BS_thread_pool.hpp` new features:
    * A new optional feature, enabled by defining the macro `BS_THREAD_POOL_ENABLE_PRIORITY`, allows assigning priority to tasks. The priority is a number of type `BS::priority_t`, which is a signed 16-bit integer, so it can have any value between -32,768 and 32,767. The tasks will be executed in priority order from highest to lowest.
        * To assign a priority to a task, add the priority as the last argument to any of the `detach` or `submit` functions. If the priority is not specified, the default value will be 0.
        * The namespace `BS::pr` contains some pre-defined priorities for users who wish to avoid magic numbers and enjoy better future-proofing. In order of decreasing priority, the pre-defined priorities are: `BS::pr::highest`, `BS::pr::high`, `BS::pr::normal`, `BS::pr::low`, and `BS::pr::lowest`.
        * Please see `README.md` for more information, including performance considerations.
    * The new member functions `detach_loop()` and `submit_loop()` facilitate loop parallelization without having to worry about internal loops in the loop function. In previous releases, the loop function had to be of the form `[](T start, T end) { for (T i = start; i < end; ++i) loop(i); }`. This behavior has been preserved in `detach_blocks()` and `submit_blocks()`. However, the new `detach_loop()` and `submit_loop()` allow much simpler loop functions of the form `[](T i) { loop(i) }`, greatly simplifying the interface.
        * Performance-wise, due to fewer function calls, `detach_blocks()` and `submit_blocks()` are generally faster. However, the difference is usually not significant, and with compiler optimizations there may be no difference at all. In any case, `detach_loop()` and `submit_loop()` are provided as convenience functions, but performance-critical applications can stick with `detach_blocks()` and `submit_blocks()`.
    * The new member functions `detach_sequence()` and `submit_sequence()` facilitate submitting a sequence of tasks enumerated by indices. This is a bit similar to `detach_loop()` and `submit_loop()`, except that the range of indices is not split into blocks with each block containing a smaller range of indices. Instead, there is exactly one task per index. This can be used, for example, to submit a sequence of tasks with each one independently processing a single array element. `detach_sequence()` does not return a future, while `submit_sequence()` returns a `BS::multi_future`.
    * It is now possible to run an initialization function in each thread before it starts to execute any submitted tasks. The function must take no arguments and have no return value. It will only be executed exactly once, when the thread is first constructed. It can be passed as an argument to the constructor or to `reset()`. See [#104](https://github.com/bshoshany/thread-pool/issues/104), [#105](https://github.com/bshoshany/thread-pool/pull/105), [#113](https://github.com/bshoshany/thread-pool/issues/113), and [#119](https://github.com/bshoshany/thread-pool/issues/119).
    * Added a member function `get_thread_ids()` which returns a vector containing the unique identifiers for each of the pool's threads, as obtained by [`std::thread::get_id()`](https://en.cppreference.com/w/cpp/thread/get_id). See [#126](https://github.com/bshoshany/thread-pool/issues/126).
    * A new optional feature, enabled by defining the macro `BS_THREAD_POOL_ENABLE_NATIVE_HANDLES`, adds a member function `get_native_handles()` which returns a vector containing the underlying implementation-defined thread handles for each of the pool's threads. These can then be used in an implementation-specific way to manage the threads at the OS level; however, note that this will generally **not** be portable code. See [#122](https://github.com/bshoshany/thread-pool/issues/122).
        * This feature is disabled by default since it uses [std::thread::native_handle()](https://en.cppreference.com/w/cpp/thread/thread/native_handle), which is in the C++ standard library, but is **not** guaranteed to be present on all systems.
    * A new namespace `BS::this_thread` was created to provide functionality similar to `std::this_thread`.
        * `BS::this_thread::get_index()` can be used to get the index of the current thread. If this thread belongs to a `BS::thread_pool` object, it will have an index from 0 to `BS::thread_pool::get_thread_count() - 1`. Otherwise, for example if this thread is the main thread or an independent [`std::thread`](https://en.cppreference.com/w/cpp/thread/thread), [`std::nullopt`](https://en.cppreference.com/w/cpp/utility/optional/nullopt) will be returned.
        * `BS::this_thread::get_pool()` can be used to get the pointer to the thread pool that owns the current thread. If this thread belongs to a `BS::thread_pool` object, a pointer to that object will be returned. Otherwise, `std::nullopt` will be returned.
        * Note that both functions return an [`std::optional`](https://en.cppreference.com/w/cpp/utility/optional) object.
    * `BS::multi_future<T>` is now defined as a specialization of `std::vector<std::future<T>>`. This means that all of the member functions that can be used on an [`std::vector`](https://en.cppreference.com/w/cpp/container/vector) can also be used on a `BS::multi_future`. For example, it is now possible to use a range-based `for` loop with a `BS::multi_future` object, since it has iterators.
        * In addition to inherited member functions, `BS::multi_future` has the following specialized member functions, most of which are new in this release: `get()`, `ready_count()`, `valid()`, `wait()`, `wait_for()`, and `wait_until()`. Please see `README.md` for more information. See also [#128](https://github.com/bshoshany/thread-pool/issues/128).
    * A new optional feature, enabled by defining the macro `BS_THREAD_POOL_ENABLE_WAIT_DEADLOCK_CHECK`, allows `wait()`, `wait_for()`, and `wait_until()` to check whether the user tried to call them from within a thread of the same pool, which would result in a deadlock. If so, they will throw the exception `BS::thread_pool::wait_deadlock` instead of waiting.
* `BS_thread_pool_utils.hpp`:
    * The utility classes `BS::synced_stream` and `BS::timer` now reside in this header file instead of the main one.
    * `BS::timer` has a new member function, `current_ms()`, which can be used to obtain the number of milliseconds that have elapsed so far, but keep the timer ticking.
    * The new utility class `BS::signaller` allows simple signalling between threads. It can be used to make one or more threads wait, using the `wait()` member function. When another thread uses the `ready()` member function, all waiting threads stop waiting. This class is really just a convenient wrapper around [`std::promise`](https://en.cppreference.com/w/cpp/thread/promise), which contains both the promise and its future.
* `BS_thread_pool.hpp` bug fixes and minor changes:
    * Optimized locking in the worker function. This should result in increased performance.
    * Optimized the way `detach_blocks()`, `submit_blocks()`, `detach_loop()`, and `submit_loop()` split the range of the loop into blocks. All blocks are now guaranteed to have one of two sizes, differing by 1, with the larger blocks always first. See [#96](https://github.com/bshoshany/thread-pool/issues/96).
        * For example, in previous releases, 100 indices were split into 15 blocks as 14 blocks of size 6 and one additional block of size 16, which was suboptimal. Now they are split into 10 blocks of size 7 and 5 blocks of size 6, which means the tasks are as evenly distributed as possible.
    * Fixed a bug that caused paused pools to have high idle CPU usage if pausing was used. See [#120](https://github.com/bshoshany/thread-pool/issues/120).
    * The worker now destructs the task object as soon as it finishes executing. See [#124](https://github.com/bshoshany/thread-pool/issues/124) and [#129](https://github.com/bshoshany/thread-pool/pull/129).
    * Added Markdown inline code formatting in all comments whenever applicable, which makes the comments look nicer when displayed as a tooltip in [Visual Studio Code](https://code.visualstudio.com/) or other supporting IDEs.
    * The `BS::thread_pool::blocks` helper class has been moved into the main thread pool class, and now returns a degenerate object (zero blocks) if `index_after_last <= first_index`.
* `BS_thread_pool_test.cpp`:
    * Removed tests for the light thread pool.
    * Added/modified tests for all new/changed features.
    * Many of the previous tests have been simplified and optimized.
    * The program now takes command line arguments:
        * `help`: Show a help message and exit.
        * `log`: Create a log file.
        * `tests`: Perform standard tests.
        * `deadlock` Perform long deadlock tests.
        * `benchmarks`: Perform benchmarks.
        * If no options are entered, the default is: `log tests benchmarks`.
    * By default, the test program enables all the optional features by defining the suitable macros, so it can test them. However, if the macro `BS_THREAD_POOL_LIGHT_TEST` is defined during compilation, the optional features will not be tested.
    * Instead of using a pre-defined list to specify the number of loop blocks to try in the benchmarks, the program now simply keeps increasing the number of blocks until it finds the optimal value. Often, the optimal number of blocks is much higher than the number of hardware threads, but if the number is too high it will result in diminishing returns.
    * `check_loop_no_return()` now checks that the loop modifies all the indices exactly once, to detect cases where an index has been modified more than once, e.g. if the same loop index was erroneously placed in more than one block.
    * Instead of defining `_CRT_SECURE_NO_WARNINGS`, the program now uses [`localtime_s`](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/localtime-s-localtime32-s-localtime64-s) instead of [`std::localtime`](https://en.cppreference.com/w/cpp/chrono/c/localtime) if MSVC is detected to avoid generating a warning.
    * On macOS, the test program will exit with [`std::terminate()`](https://en.cppreference.com/w/cpp/error/terminate) instead of [`std::quick_exit()`](https://en.cppreference.com/w/cpp/utility/program/quick_exit) if any tests failed. This is because macOS does not implement `std::quick_exit()` for some reason. Note that as a result, the number of failed tests cannot be returned by the program on macOS. Unfortunately, [`std::exit()`](https://en.cppreference.com/w/cpp/utility/program/exit) cannot be used here, as it might get stuck if a deadlock occurs. See [#106](https://github.com/bshoshany/thread-pool/pull/106)
    * The log file now uses the name of the executable file, followed by the date and time, so it's easy to distinguish between log files generated by different builds of the test (since the test script names them based on the compiler used). Also, the program now checks if the log file failed to open for some reason, and writes only to the standard output in that case.
    * The benchmarks now display a progress bar.
    * The test program will now detect the OS and compiler used.
* `BS_thread_pool_test.ps1`:
    * The script will compile and run a light version of the test, with no optional features enabled, in addition to the main test, for each compiler.
    * The source and build folders will now be determined relative to the script folder, to ensure that the script works no matter which folder it is executed from.
    * The script now checks that the include files `BS_thread_pool.hpp` and `BS_thread_pool_utils.hpp` are present before attempting to compile the test program.
* `README.md`:
    * Added/modified documentation for all new/changed features.
    * Revised many of the existing examples and explanations.
    * Added a complete library reference at the end of the documentation.
    * Added instructions for installing the package using Meson and CMake with CPM. The installation instructions with various package managers and build systems were moved to the end, before the reference.
* Miscellaneous:
    * A `.clang-tidy` file is now included, with all the checks that are enabled in this project. The pull request template has been updated to suggest that authors lint their code using this file before submitting the pull request.
* This release is dedicated to my wife (since December 1, 2023), Pauline. Her endless love, support, and encouragement have been a great source of motivation for working on this and other projects. I am so lucky and honored to [`my_future.share()`](https://en.cppreference.com/w/cpp/thread/future/share) with her ❤️

### v3.5.0 (2023-05-25)

* `BS_thread_pool.hpp` and `BS_thread_pool_light.hpp`:
    * Added a new member function, `purge()`, to the full (non-light) thread pool. This function purges all the tasks waiting in the queue. Tasks that are currently running will not be affected, but any tasks still waiting in the queue will be removed and will never be executed by the threads. Please note that there is no way to restore the purged tasks.
    * Fixed a bug which caused `wait_for_tasks()` to only block the first thread that called it. Now it blocks every thread that calls it, which is the expected behavior. In addition, all related deadlock have now been completely resolved. This also applies to the variants `wait_for_tasks_duration()` and `wait_for_tasks_until()` in the non-light version. See [#110](https://github.com/bshoshany/thread-pool/pull/110).
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
    * Fixed an issue that caused a compilation error when using MSVC and including `windows.h`. See [#72](https://github.com/bshoshany/thread-pool/pull/72).
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
        * Instead of setting the test parameters to achieve a fixed single-threaded mean execution time of 300 ms, the test now aims to achieve a fixed multithreaded mean execution time of 50 ms when the number of blocks is equal to the number of threads. This allows for more reliable results on very fast CPUs with a very large number of threads, where the mean execution time when using all the threads could previously be below a statistically significant value.
        * The number of vectors is now restricted to be a multiple of the number of threads, so that the blocks are always all of the same size.
* `README.md`:
    * Added instructions and examples for the new features described above.
    * Rewrote the documentation for `parallelize_loop()` to make it clearer.

### v3.1.0 (2022-07-13)

* `BS_thread_pool.hpp`:
    * Fixed an issue where `wait_for_tasks()` would sometimes get stuck if `push_task()` was executed immediately before `wait_for_tasks()`.
    * Both the thread pool constructor and the `reset()` member function now determine the number of threads to use in the pool as follows. If the parameter is a positive number, then the pool will be created with this number of threads. If the parameter is non-positive, or a parameter was not supplied, then the pool will be created with the total number of hardware threads available, as obtained from `std::thread::hardware_concurrency()`. If the latter returns a non-positive number for some reason, then the pool will be created with just one thread. See [#51](https://github.com/bshoshany/thread-pool/issues/51) and [#52](https://github.com/bshoshany/thread-pool/issues/52).
    * Added the `[[nodiscard]]` attribute to classes and class members, in order to warn the user when accidentally discarding an important return value, such as a future or the return value of a function with no useful side-effects. For example, if you use `submit()` and don't save the future it returns, the compiler will now generate a warning. (If a future is not needed, then you should use `push_task()` instead.)
    * Removed the `explicit` specifier from all constructors, as it prevented the default constructor from being used with static class members. See [#48](https://github.com/bshoshany/thread-pool/issues/48).
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
    * The benchmarks have been simplified. There's now only one test: filling a specific number of vectors of fixed size with random values. This may be replaced with something more practical in a future released, but at least on the systems I've tested on, it does demonstrate a very significant multithreading speedup.
    * In addition to multithreaded tests with different numbers of tasks, the benchmark now also includes a single-threaded test. This allows for more accurate benchmarks compared to previous versions, as the (slight) parallelization overhead is now taken into account when calculating the maximum speedup.
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
