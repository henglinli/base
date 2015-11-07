// -*-coding:utf-8-unix;-*-
#include <stdint.h>
#include "macros.hh"
//
// see http://www.1024cores.net/home/lock-free-algorithms/tricks/per-processor-date
//
namespace NAMESPACE {
//
const uint32_t kPeriod(20);
//
class Processor {
 public:
  //
  static uint32_t Current() {
    if (0 == --_period) {
      _period = kPeriod; // subject to tweaking
      __builtin_ia32_rdtscp(&_number);
    }
    return _number;
  }
 protected:
 private:
  //
  static __thread uint32_t _number;
  static __thread uint32_t _period;
};
//
__thread uint32_t Processor::_number = 0;
__thread uint32_t Processor::_period = 1;
//
} // namespace NAMESPACE
