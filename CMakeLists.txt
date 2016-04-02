cmake_minimum_required (VERSION 3.2)
project(runtime)
# for test
enable_testing()
# for gtest
option(BUILD_GTEST "Builds the googletest subproject" ON)
option(BUILD_GMOCK "Builds the googlemock subproject" OFF)
add_definitions(-DGTEST_USE_OWN_TR1_TUPLE=1)
add_subdirectory(third_party/googletest)
#
include_directories(
  ${gtest_SOURCE_DIR}/include
  ${runtime_SOURCE_DIR}/include
  )
#
if(UNIX)
  if(APPLE)
    set(CMAKE_CXX_COMPILER "clang++")
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Weffc++ -fno-exceptions -fno-rtti")
    #-fno-exceptions
    set(CMAKE_EXE_LINKER_FLAGS "-stdlib=libc++")
    #set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
    add_definitions()
  else()
    set(CMAKE_CXX_COMPILER "g++")
    #set(CMAKE_CXX_FLAGS "-std=c++11 -fno-exceptions -Wall -Wextra -Weffc++")
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Weffc++ -fno-exceptions -fno-rtti")
    set(CMAKE_EXE_LINKER_FLAGS "-pthread")
    add_definitions()
  endif()
endif()
# for test
add_subdirectory(test)
#
