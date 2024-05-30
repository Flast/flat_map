include(${PROJECT_SOURCE_DIR}/cmake/benchmark.cmake)

add_custom_target(bench)
set_target_properties(bench PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)

macro(add_bench benchname)
  add_benchmark(${benchname} ${ARGN})
  set_target_properties(${benchname} PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
  target_link_libraries(${benchname} flat_map)
  add_dependencies(bench ${benchname})
endmacro()

add_bench(map_construction map_construction.cpp)
add_bench(map_insertion map_insertion.cpp)
add_bench(map_merge map_merge.cpp)
