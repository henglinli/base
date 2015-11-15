// -*- coding:utf-8-unix; -*-
#include <stdint.h>
#include "macros.hh"
#ifdef __clang__
#include <x86intrin.h>
#else
#define __rdtscp __builtin_ia32_rdtscp
#endif // __clang__
//
// http://www.1024cores.net/home/lock-free-algorithms/tricks/per-processor-data
//
namespace NAMESPACE {
//
class Processor {
 public:
  static inline uint32_t Current() {
    _timestap = __rdtscp(&_number);
    return _number;
  }
  //
  static inline uint32_t Timestap() {
    _timestap = __rdtscp(&_number);
    return _timestap;
  }
  //
  static inline uint32_t Rdscp(uint32_t* cpu) {
    return __rdtscp(cpu);
  }
 protected:
 private:
  static __thread uint32_t _number;
  static __thread uint32_t _timestap;
}; // class Processor
//
__thread uint32_t Processor::_number = 0;
__thread uint32_t Processor::_timestap = 0;
} // namespace NAMESPACE
