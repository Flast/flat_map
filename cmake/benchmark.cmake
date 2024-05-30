find_package(benchmark QUIET)

if(NOT benchmark_FOUND)
  include(FetchContent)

  set(BENCHMARK_ENABLE_TESTING Off)
  FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark
    GIT_TAG v1.8.4
  )

  FetchContent_MakeAvailable(benchmark)
endif()

function(add_benchmark benchname)
  add_executable(${benchname} ${ARGN})
  target_link_libraries(${benchname} benchmark::benchmark)
endfunction()
