// -*- coding:utf-8-unix; -*-
#pragma once
#include <stdint.h>
#include "mpmc/queue.hh"
//
// http://www.1024cores.net/home/lock-free-algorithms/tricks/per-processor-data
//
namespace NAMESPACE {
//
template<typename Task>
class Processor {
 public:
  static inline uint32_t Current() {
    _timestap = __builtin_ia32_rdtscp(&_number);
    return _number;
  }
  //
  static inline uint32_t Timestap() {
    _timestap = __builtin_ia32_rdtscp(&_number);
    return _timestap;
  }
  //
  static inline uint32_t Rdscp(uint32_t* cpu) {
    return __builtin_ia32_rdtscp(cpu);
  }
  //
  inline void Push(Task* task) {
    _task_queue.Push(task);
  }
  //
  inline Task* Pop() {
    return _task_queue.Pop();
  }
  //
 protected:
 private:
  mpmc::Queue<Task> _task_queue;
  //
  static __thread uint32_t _number;
  static __thread uint32_t _timestap;
}; // class Processor
//
template<typename Task>
__thread uint32_t Processor<Task>::_number = 0;
template<typename Task>
__thread uint32_t Processor<Task>::_timestap = 0;
} // namespace NAMESPACE
