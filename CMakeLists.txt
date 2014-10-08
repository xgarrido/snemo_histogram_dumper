# - Top level CMakeLists.txt for Geant4 validation program

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(histogram_pool_dumper)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(STATUS "${PROJECT_NAME} requires an out-of-source build.")
  message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
  message(STATUS "  CMakeCache.txt")
  message(STATUS "  CMakeFiles")
  message(STATUS "Once these files are removed, create a separate directory")
  message(STATUS "and run CMake from there, pointing it to:")
  message(STATUS "  ${CMAKE_SOURCE_DIR}")
  message(FATAL_ERROR "in-source build detected")
endif()

# - Third party
find_package(Bayeux 1.0.0 REQUIRED)

include_directories(${PROJECT_SOURCE_DIR} ${Bayeux_INCLUDE_DIRS})

add_library(hpd SHARED
  hpd_driver.h hpd_driver.cc)

target_link_libraries(hpd ${Bayeux_LIBRARIES})

install(FILES
  ${PROJECT_BINARY_DIR}/libhpd${CMAKE_SHARED_LIBRARY_SUFFIX}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  PERMISSIONS OWNER_READ OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
  )

add_executable(histogram_pool_dumper histogram_pool_dumper.cxx)

target_link_libraries(histogram_pool_dumper
  hpd)

install(FILES
  ${PROJECT_BINARY_DIR}/histogram_pool_dumper
  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
  PERMISSIONS OWNER_READ OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
  )

# end of CMakeLists.txt