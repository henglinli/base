// -*-coding:utf-8-unix;-*-
#pragma once
// namespace
#ifndef NAMESPACE
#define NAMESPACE rt
#endif
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
//
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;      \
  void operator=(const TypeName&) = delete
// do nothingfsaf saf
template<typename... T>
void DoNothing(T...) {
  // nilsf
}
//
#ifdef NDEBUG
#define printf(...) do {} while(0)
#else // NDEBUG
// printf
#endif // NDEBUG
//
#ifdef __linux
#define os_linux 1
#else // __linux
#ifdef __linux__
#define os_linux 1
#else // __linux__
#ifdef __gnu_linux__
#define os_linux 1
#else // __gnu_linux__
#ifdef linux
#define os_linux 1
#endif // linux
#endif // __gnu_linux__
#endif // __linux__
#endif // __linux
