find_package(Catch2 3 QUIET)

if(NOT Catch2_FOUND)
  include(FetchContent)

  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/catch2
    GIT_TAG v3.5.4
  )

  FetchContent_MakeAvailable(Catch2)
endif()

function(add_catch2_test testname)
  add_executable(${testname} ${ARGN})
  target_link_libraries(${testname} PRIVATE Catch2::Catch2WithMain)
  add_test(NAME ${testname} COMMAND ${testname})
  set_tests_properties(${testname} PROPERTIES LABELS ${testname} TIMEOUT 30)
  set_target_properties(${testname} PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
endfunction()
