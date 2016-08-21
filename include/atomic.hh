// -*-coding:utf-8-unix;-*-
#pragma once
#include "macros.hh"
//
namespace NAMESPACE {
  namespace atomic {
    // Load
    template<typename Int>
    Int Load(Int* ptr) {
      static_assert(sizeof(Int) <= sizeof(int128_t), "most 128 bit Integer type only");
      return __atomic_load_n(ptr, __ATOMIC_ACQUIRE);
    };
    // Store
    template<typename Int>
    void Store(Int* ptr, Int value) {
      static_assert(sizeof(Int) <= sizeof(int128_t), "most 128 bit Integer type only");
      __atomic_store_n(ptr, value, __ATOMIC_RELEASE);
    };
    // Exchage
    template<typename Int>
    Int Exchange(Int* ptr, Int value) {
      static_assert(sizeof(Int) <= sizeof(int128_t), "most 128 bit Integer type only");
      return __atomic_exchange_n(ptr, value, __ATOMIC_RELEASE);
    }
    // CAS
    template<typename Int>
    bool CAS(Int* ptr, Int* expected, Int value) {
      static_assert(sizeof(Int) <= sizeof(int128_t), "most 128 bit Integer type only");
      return __atomic_compare_exchange_n(ptr, expected, value, false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE);
    }
  } // namespace atomic
//
} // namespace NAMESPACE
