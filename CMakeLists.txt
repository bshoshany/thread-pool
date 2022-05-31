cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    bshoshany-thread-pool
    VERSION 3.0.0
    DESCRIPTION "BS::thread_pool: a fast, lightweight, and easy-to-use C++17 thread pool library"
    HOMEPAGE_URL "https://github.com/bshoshany"
    LANGUAGES NONE
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(bshoshany-thread-pool_bshoshany-thread-pool INTERFACE)
add_library(bshoshany-thread-pool::bshoshany-thread-pool ALIAS bshoshany-thread-pool_bshoshany-thread-pool)

set_property(
    TARGET bshoshany-thread-pool_bshoshany-thread-pool PROPERTY
    EXPORT_NAME bshoshany-thread-pool
)

target_include_directories(
    bshoshany-thread-pool_bshoshany-thread-pool ${warning_guard}
    INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_compile_features(bshoshany-thread-pool_bshoshany-thread-pool INTERFACE cxx_std_17)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Examples ----

if(PROJECT_IS_TOP_LEVEL)
  option(BUILD_EXAMPLES "Build examples tree." "${bshoshany-thread-pool_DEVELOPER_MODE}")
  if(BUILD_EXAMPLES)
    add_subdirectory(example)
  endif()
endif()

# ---- Developer mode ----

if(NOT bshoshany-thread-pool_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of bshoshany-thread-pool"
  )
endif()

include(cmake/dev-mode.cmake)
