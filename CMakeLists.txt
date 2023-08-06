cmake_minimum_required(VERSION 3.10)

project(flat_map VERSION 0.1.0 LANGUAGES CXX)


include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(flat_map INTERFACE)
add_library(flat_map::flat_map ALIAS flat_map)

target_compile_features(flat_map INTERFACE cxx_std_17)
target_include_directories(flat_map INTERFACE
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/flat_map>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

install(TARGETS flat_map
  EXPORT flat_mapTargets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
  FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/flat_map/__concepts.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/flat_map/__config.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/flat_map/__flat_tree.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/flat_map/__fwd.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/flat_map/__memory.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/flat_map/__tuple.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/flat_map/__type_traits.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/flat_map/enum.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/flat_map/flat_map.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/flat_map/flat_multimap.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/flat_map/flat_set.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/flat_map/flat_multiset.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/flat_map/tied_sequence.hpp
  DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}/flat_map
)

install(EXPORT flat_mapTargets
  FILE flat_mapTargets.cmake
  NAMESPACE flat_map::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/flat_map
)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/flat_mapConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/flat_map"
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/flat_mapConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY AnyNewerVersion
)

install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/flat_mapConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/flat_mapConfigVersion.cmake
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/flat_map
)

export(EXPORT flat_mapTargets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/flat_mapTargets.cmake"
  NAMESPACE flat_map::
)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/flat_mapConfigVersion.cmake"
  DESTINATION
    "${CMAKE_INSTALL_LIBDIR}/cmake/flat_map"
)

enable_testing()

add_subdirectory(test)
add_subdirectory(bench)
