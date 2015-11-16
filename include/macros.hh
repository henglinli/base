// -*-coding:utf-8-unix;-*-
#pragma once
// namespace
#ifndef NAMESPACE
#define NAMESPACE rt
#endif
// nullptr
#ifndef nullptr
#include <cstddef>
#define nullptr NULL
#endif // nullptr
// align
#ifndef Align
#define Align(Type, bytes) \
  Type __attribute__((aligned (bytes)))
#endif // Align
// __init128
// gcc -dM -E - < /dev/null
#ifndef GCC_VERSION
#define GCC_VERSION (__GNUC__ * 10000           \
                     + __GNUC_MINOR__ * 100     \
                     + __GNUC_PATCHLEVEL__)
#endif
#if defined(__x86_64__)
typedef __int128_t int128_t;
typedef __uint128_t uint128_t;
#else
#error "Only support X86_64 platform!"
#endif
// do nothing
template<typename T>
void DoNothing(T) {
  // nil
}
//
template<typename T1, typename T2>
void DoNothing(T1, T2) {
  // nil
}

