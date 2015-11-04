// -*-coding:utf-8-unix;-*-
#pragma once
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
#if defined(__x86_64__) and defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_16)
typedef __int128_t int128_t;
typedef __uint128_t uint128_t;
#else
#error "Not supported platform!"
#endif
