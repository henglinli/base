// -*- coding:utf-8-unix; -*-
#pragma once
#include <x86intrin.h>
#include <stdint.h>
#include <sched.h>
#include "macros.hh"
//
// http://www.1024cores.net/home/lock-free-algorithms/tricks/per-processor-data
//
namespace NAMESPACE {
//
class Processor {
 public:
  //
  inline static auto Current() -> const uint32_t {
    _timestap = __rdtscp(&_number);
    return _number;
  }
  //
  inline static auto Timestap() -> const uint64_t {
    return __rdtsc();
  }
  //
  inline static auto Rdtscp(uint32_t* cpu) -> const uint64_t {
    return __rdtscp(cpu);
  }
  //
  static auto Relax(uint32_t count) -> void {
    while(count--) {
      __pause();
    }
  }
  //
  static constexpr auto MaxCPUs() -> const size_t {
    return sizeof(cpu_set_t)*8;
  }
  //
  static auto Count() -> const uint32_t {
    cpu_set_t set;
    int done = sched_getaffinity(0, sizeof(set), &set);
    if (0 not_eq done) {
      return 1;
    }
    uint32_t count(0);
    size_t len = sizeof(set)/sizeof(uint64_t);
    uint64_t* start(reinterpret_cast<uint64_t*>(&set));
    while(len--) {
      // Note: _popcnt32 cannot compile with -fgnu-tm
      count += __builtin_popcount(*start++);
    }
    return count? count: 1;
  }
  //
 protected:
 private:
  //
  thread_local static uint32_t _number;
  thread_local static uint32_t _timestap;
  //
  DISALLOW_COPY_AND_ASSIGN(Processor);
}; // class Processor
//
thread_local uint32_t Processor::_number(0);
thread_local uint32_t Processor::_timestap(0);
} // namespace NAMESPACE
