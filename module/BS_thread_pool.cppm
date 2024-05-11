module;

#ifndef BS_THREAD_POOL_USE_STD_MODULE
#include <BS_thread_pool.hpp>
#include <BS_thread_pool_utils.hpp>
#endif

export module BS_thread_pool;

#ifdef BS_THREAD_POOL_USE_STD_MODULE
import std;

extern "C++" {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winclude-angled-in-module-purview"
#include <BS_thread_pool.hpp>
#include <BS_thread_pool_utils.hpp>
#pragma clang diagnostic pop
}
#endif

export namespace BS {
namespace this_thread {
    using this_thread::get_index;
    using this_thread::get_pool;
    using this_thread::optional_index;
    using this_thread::optional_pool;
}

    using BS::thread_pool;
    using BS::multi_future;
    using BS::signaller;
    using BS::synced_stream;
    using BS::timer;
}