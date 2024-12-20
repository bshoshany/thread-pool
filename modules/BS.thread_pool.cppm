/**
 * ██████  ███████       ████████ ██   ██ ██████  ███████  █████  ██████          ██████   ██████   ██████  ██
 * ██   ██ ██      ██ ██    ██    ██   ██ ██   ██ ██      ██   ██ ██   ██         ██   ██ ██    ██ ██    ██ ██
 * ██████  ███████          ██    ███████ ██████  █████   ███████ ██   ██         ██████  ██    ██ ██    ██ ██
 * ██   ██      ██ ██ ██    ██    ██   ██ ██   ██ ██      ██   ██ ██   ██         ██      ██    ██ ██    ██ ██
 * ██████  ███████          ██    ██   ██ ██   ██ ███████ ██   ██ ██████  ███████ ██       ██████   ██████  ███████
 *
 * @file BS.thread_pool.cppm
 * @author Barak Shoshany (baraksh@gmail.com) (https://baraksh.com/)
 * @version 5.0.0
 * @date 2024-12-19
 * @copyright Copyright (c) 2024 Barak Shoshany. Licensed under the MIT license. If you found this project useful, please consider starring it on GitHub! If you use this library in software of any kind, please provide a link to the GitHub repository https://github.com/bshoshany/thread-pool in the source code and documentation. If you use this library in published research, please cite it as follows: Barak Shoshany, "A C++17 Thread Pool for High-Performance Scientific Computing", doi:10.1016/j.softx.2024.101687, SoftwareX 26 (2024) 101687, arXiv:2105.00613
 *
 * @brief `BS::thread_pool`: a fast, lightweight, modern, and easy-to-use C++17/C++20/C++23 thread pool library. This module file wraps the header file BS_thread_pool.hpp inside a C++20 module so it can be imported using `import BS.thread_pool`.
 */

module;

// A macro indicating to the library that it is being imported as a module, as well as the version of the module file, which must match the version of the header file.
#define BS_THREAD_POOL_MODULE 5, 0, 0

#include "BS_thread_pool.hpp"

export module BS.thread_pool;

export namespace BS {
using BS::binary_semaphore;
using BS::common_index_type_t;
using BS::counting_semaphore;
using BS::light_thread_pool;
using BS::multi_future;
using BS::pause_thread_pool;
using BS::pr;
using BS::priority_t;
using BS::priority_thread_pool;
using BS::synced_stream;
using BS::this_thread;
using BS::thread_pool;
using BS::thread_pool_import_std;
using BS::thread_pool_module;
using BS::thread_pool_native_extensions;
using BS::thread_pool_version;
using BS::tp;
using BS::version;
using BS::wait_deadlock;
using BS::wdc_thread_pool;

#ifdef BS_THREAD_POOL_NATIVE_EXTENSIONS
using BS::get_os_process_affinity;
using BS::get_os_process_priority;
using BS::os_process_priority;
using BS::os_thread_priority;
using BS::set_os_process_affinity;
using BS::set_os_process_priority;
#endif
} // namespace BS
