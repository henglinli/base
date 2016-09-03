// -*- coding:utf-8-unix; -*-
#pragma once
#include <x86intrin.h>
#include <stdint.h>
#include <sched.h>
#include "gnutm/queue.hh"
//
// http://www.1024cores.net/home/lock-free-algorithms/tricks/per-processor-data
//
namespace NAMESPACE {
//
template<typename Task>
class Processor {
 public:
  //
  Processor(): _task_queue() {}
  //
  inline static auto Current() -> uint32_t {
    _timestap = __rdtscp(&_number);
    return _number;
  }
  //
  inline static auto Timestap() -> uint64_t {
    return __rdtsc();
  }
  //
  inline static auto Rdtscp(uint32_t* cpu) -> uint64_t {
    return __rdtscp(cpu);
  }
  //
  static auto Relax(uint32_t count) -> void {
    while(count--) {
      __pause();
    }
  }
  //
  static constexpr auto MaxCPUs() -> size_t {
    return sizeof(cpu_set_t)*8;
  }
  //
  static auto Count() -> uint32_t {
    cpu_set_t set;
    int done = sched_getaffinity(0, sizeof(set), &set);
    if (0 not_eq done) {
      return 1;
    }
    uint32_t count(0);
    size_t len = sizeof(set)/sizeof(uint64_t);
    uint64_t* start(reinterpret_cast<uint64_t*>(&set));
    while(len--) {
      count += __popcntq(*start++);
    }
    return count? count: 1;
  }
  //
  inline auto Push(Task* task) -> void {
    _task_queue.Push(task);
  }
  //
  inline auto Pop() -> Task* {
    return _task_queue.Pop();
  }
  //
 protected:
 private:
  gnutm::StailQ<Task> _task_queue;
  //
  thread_local static uint32_t _number;
  thread_local static uint32_t _timestap;
  //
  DISALLOW_COPY_AND_ASSIGN(Processor);
}; // class Processor
//
template<typename Task>
thread_local uint32_t Processor<Task>::_number(0);
template<typename Task>
thread_local uint32_t Processor<Task>::_timestap(0);
} // namespace NAMESPACE
