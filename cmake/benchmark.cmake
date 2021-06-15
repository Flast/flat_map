find_package(benchmark QUIET)

if(benchmark_FOUND)
  add_library(benchmarkLib ALIAS benchmark::benchmark)
  add_library(benchmarkInc ALIAS benchmark::benchmark)
else()
  include(ExternalProject)

  ExternalProject_Add(
    benchmarkBuild
    GIT_REPOSITORY https://github.com/google/benchmark
    GIT_TAG v1.5.5
    PREFIX external
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/external
               -DCMAKE_BUILD_TYPE=Release
               -DBENCHMARK_ENABLE_TESTING=Off
  )

  add_library(benchmarkLib STATIC IMPORTED)
  set_target_properties(benchmarkLib PROPERTIES IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/external/lib/libbenchmark.a)
  add_library(benchmarkInc INTERFACE)
  target_include_directories(benchmarkInc INTERFACE ${CMAKE_BINARY_DIR}/external/include)
  add_dependencies(benchmarkInc benchmarkBuild)
endif()

function(add_benchmark benchname)
  add_executable(${benchname} ${ARGN})
  target_link_libraries(${benchname} benchmarkInc benchmarkLib)
endfunction()
