set(GNUCC_COMPAT CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang" INTERNAL)

set(SANITIZE OFF CACHE STRING "Enable sanitizer")
set(CXX_STANDARD_UPTO 17 CACHE STRING "Latest C++ version to test")

add_library(requirements INTERFACE)
target_compile_definitions(requirements INTERFACE CATCH_CONFIG_ENABLE_ALL_STRINGMAKERS)

if(GNUCC_COMPAT)
  target_compile_options(requirements INTERFACE -Wall -Wextra -pedantic -Werror=unused)

  if(SANITIZE)
    # target_compile_options(requirements INTERFACE -fsanitize=${SANITIZE})
    # XXX: requires cmake 3.13
    # target_link_options(requirements INTERFACE -fsanitize=${SANITIZE})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=${SANITIZE}")
  endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  target_compile_options(requirements INTERFACE -Werror=return-type -Werror=undefined-inline)
endif()

include(cmake/catch2)

add_custom_target(unit_test)
set_target_properties(unit_test PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)

macro(add_unit_test testname)
  add_catch2_test(${testname} ${ARGN})
  target_link_libraries(${testname} PRIVATE requirements)
  target_link_libraries(${testname} PRIVATE flat_map)
  add_dependencies(unit_test ${testname})
endmacro()

macro(add_tests testname)
  add_unit_test(${testname}_17 ${ARGN})
  if(CXX_STANDARD_UPTO GREATER_EQUAL 20)
    add_unit_test(${testname}_20 ${ARGN})
    set_property(TARGET ${testname}_20 PROPERTY CXX_STANDARD 20)
  endif()
  if(CXX_STANDARD_UPTO GREATER_EQUAL 23)
    add_unit_test(${testname}_23 ${ARGN})
    set_property(TARGET ${testname}_23 PROPERTY CXX_STANDARD 23)
  endif()
endmacro()

add_tests(map_vector_test map_vector.cpp)
add_tests(map_deque_test map_deque.cpp)

add_tests(multimap_vector_test multimap_vector.cpp)
add_tests(multimap_deque_test multimap_deque.cpp)

add_tests(set_vector_test set_vector.cpp)
add_tests(set_deque_test set_deque.cpp)

add_tests(multiset_vector_test multiset_vector.cpp)
add_tests(multiset_deque_test multiset_deque.cpp)
