cmake_minimum_required (VERSION 2.8)
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
    set(CMAKE_CXX_FLAGS "-g -ggdb -O0 -Wall -Wextra -Weffc++ -fno-exceptions -fno-rtti -mcx16 -fsanitize=address -fno-omit-frame-pointer")
    #-fno-exceptions
    set(CMAKE_EXE_LINKER_FLAGS "-stdlib=libc++")
    #set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
    add_definitions()
  else()
    #set(CMAKE_CXX_FLAGS "-std=c++11 -fno-exceptions -Wall -Wextra -Weffc++")
    set(CMAKE_CXX_FLAGS "-g -ggdb -O0 -Wall -Wextra -Weffc++ -fno-exceptions -fno-rtti -mcx16 -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "-pthread")
    add_definitions()
  endif()
endif()
# for test
add_subdirectory(test)
#
