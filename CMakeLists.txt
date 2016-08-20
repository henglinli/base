cmake_minimum_required (VERSION 2.8)
project(runtime)
#
foreach(p
    CMP0054 # CMake 3.1
    CMP0056 # export EXE_LINKER_FLAGS to try_run
    )
  if(POLICY ${p})
    cmake_policy(SET ${p} NEW)
  endif()
endforeach()
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
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Weffc++ -fno-exceptions -fno-rtti -mcx16 -fsanitize=address -fno-omit-frame-pointer")
    #-fno-exceptions
    set(CMAKE_EXE_LINKER_FLAGS "-stdlib=libc++")
    #set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
    add_definitions()
  else()
    set(CMAKE_CXX_FLAGS_DEBUG "-fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Weffc++ -fno-exceptions -fno-rtti -mcx16 -std=gnu++11")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "-pthread -fsplit-stack -fsanitize=address -fno-omit-frame-pointer -fuse-ld=gold")
    add_definitions()
  endif()
endif()
# Coverage build type
  set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_DEBUG}" CACHE STRING
    "Flags used by the C++ compiler during coverage builds."
    FORCE)
  set(CMAKE_EXE_LINKER_FLAGS_COVERAGE
    "${CMAKE_EXE_LINKER_FLAGS_DEBUG}" CACHE STRING
    "Flags used for linking binaries during coverage builds."
    FORCE)
  set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE
    "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}" CACHE STRING
    "Flags used by the shared libraries linker during coverage builds."
    FORCE)
  mark_as_advanced(
    CMAKE_CXX_FLAGS_COVERAGE
    CMAKE_EXE_LINKER_FLAGS_COVERAGE
    CMAKE_SHARED_LINKER_FLAGS_COVERAGE)
  set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel Coverage."
    FORCE)
  # for coverage
  set(CMAKE_CXX_FLAGS_COVERAGE "--coverage")
# for test
add_subdirectory(test)
#
