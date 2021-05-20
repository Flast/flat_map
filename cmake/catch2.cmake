find_package(Catch2 QUIET)

if(Catch2_FOUND)
  add_library(Catch2Target ALIAS Catch2::Catch2)
else()
  include(ExternalProject)

  ExternalProject_Add(
    Catch2Build
    GIT_REPOSITORY https://github.com/catchorg/catch2
    GIT_TAG v2.13.6
    PREFIX external
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/external
  )

  add_library(Catch2Target INTERFACE)
  target_include_directories(Catch2Target INTERFACE ${CMAKE_BINARY_DIR}/external/include)
  add_dependencies(Catch2Target Catch2Build)
endif()

function(add_catch2_test testname)
  add_executable(${testname} ${ARGN})
  target_link_libraries(${testname} Catch2Target)
  target_compile_options(${testname} PRIVATE "-DCATCH_CONFIG_MAIN")
  add_test(NAME ${testname} COMMAND ${testname})
  set_tests_properties(${testname} PROPERTIES LABELS ${testname} TIMEOUT 30)
  set_target_properties(${testname} PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
endfunction()
