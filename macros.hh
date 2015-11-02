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
#ifndef __SIZEOF_INT128__
#error "Not supported platform!"
#else
typedef __int128_t int128_t;
typedef __uint128_t uint128_t;
#endif
