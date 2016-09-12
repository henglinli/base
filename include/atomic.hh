// -*-coding:utf-8-unix;-*-
#pragma once
#include "macros.hh"
//
namespace NAMESPACE {
namespace atomic {
// Load
template<typename Int>
inline auto Load(Int* ptr) -> Int {
  static_assert(sizeof(Int) <= sizeof(int128_t), "most 128 bit Integer type only");
  return __atomic_load_n(ptr, __ATOMIC_ACQUIRE);
};
// Store
template<typename Int>
inline auto Store(Int* ptr, Int value) -> void {
  static_assert(sizeof(Int) <= sizeof(int128_t), "most 128 bit Integer type only");
  __atomic_store_n(ptr, value, __ATOMIC_RELEASE);
};
// Exchage
template<typename Int>
inline auto Exchange(Int* ptr, Int value) -> Int {
  static_assert(sizeof(Int) <= sizeof(int128_t), "most 128 bit Integer type only");
  return __atomic_exchange_n(ptr, value, __ATOMIC_RELEASE);
}
// CAS
template<typename Int>
inline auto CAS(Int* ptr, Int* expected, Int value) -> bool {
  static_assert(sizeof(Int) <= sizeof(int128_t), "most 128 bit Integer type only");
  return __atomic_compare_exchange_n(ptr, expected, value, false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE);
}
} // namespace atomic
//
} // namespace NAMESPACE
