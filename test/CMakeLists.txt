if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
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
