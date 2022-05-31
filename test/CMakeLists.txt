cmake_minimum_required(VERSION 3.14)

project(bshoshany-thread-poolTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)
include(../cmake/windows-set-path.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(bshoshany-thread-pool REQUIRED)
  enable_testing()
endif()

# ---- Tests ----

add_executable(bshoshany-thread-pool_test source/BS_thread_pool_test.cpp)
target_link_libraries(bshoshany-thread-pool_test PRIVATE bshoshany-thread-pool::bshoshany-thread-pool)
target_compile_features(bshoshany-thread-pool_test PRIVATE cxx_std_17)

add_test(NAME bshoshany-thread-pool_test COMMAND bshoshany-thread-pool_test)

# ---- End-of-file commands ----

add_folders(Test)
