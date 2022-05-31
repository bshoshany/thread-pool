cmake_minimum_required(VERSION 3.14)

project(bshoshany-thread-poolExamples CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

if(PROJECT_IS_TOP_LEVEL)
  find_package(bshoshany-thread-pool REQUIRED)
endif()

add_custom_target(run-examples)

function(add_example NAME)
  add_executable("${NAME}" "${NAME}.cpp")
  target_link_libraries("${NAME}" PRIVATE bshoshany-thread-pool::bshoshany-thread-pool)
  target_compile_features("${NAME}" PRIVATE cxx_std_17)
  add_custom_target("run_${NAME}" COMMAND "${NAME}" VERBATIM)
  add_dependencies("run_${NAME}" "${NAME}")
  add_dependencies(run-examples "run_${NAME}")
endfunction()

add_example(base)
add_example(multiple_futures)
add_example(monitoring_tasks)
add_example(pausing_workers)

add_folders(Example)
