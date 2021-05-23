set(GNUCC_COMPAT CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" INTERNAL)

set(SANITIZE OFF CACHE STRING "Enable sanitizer")

if(GNUCC_COMPAT)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

  if(SANITIZE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=${SANITIZE}")
    set(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} -fsanitize=${SANITIZE}")
  endif()
endif()

include(cmake/catch2)

add_custom_target(unit_test)
set_target_properties(unit_test PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)

macro(add_tests testname)
  add_catch2_test(${testname} ${ARGN})
  target_link_libraries(${testname} flat_map)
  add_dependencies(unit_test ${testname})
endmacro()

add_tests(vector_test vector.cpp)
add_tests(deque_test deque.cpp)
add_tests(map_test map.cpp)
