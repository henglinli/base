// -*-coding:utf-8-unix;-*-
#pragma once
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include "macros.hh"
//
namespace NAMESPACE {
//
template<typename Task, typename Value>
class Thread {
  static_assert(sizeof(Value) <= sizeof(void*), "sizeof(Value) should less than sizeof(void*)");
 public:
  Thread(): _tid(-1) {}
  //
  int Run(Task& task) {
    return pthread_create(&_tid, nullptr, ThreadMain, static_cast<void*>(&task));
  }
  //
  int Run(Task& task, int cpu) {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpu, &set);
    pthread_attr_setaffinity_np(&attr, sizeof(set), &set);
    return pthread_create(&_tid, &attr, ThreadMain, static_cast<void*>(&task));
  }
  //
  int Join(Value& ptr) {
    Value* result(nullptr);
    auto done = pthread_join(_tid, reinterpret_cast<void**>(&result));
    if (0 not_eq done) {
      return done;
    }
    ptr = *result;
    return done;
  }
  //
  int Cancel() {
    return pthread_cancel(_tid);
  }
  //
  int RunBackgroud(Task& task) {
    auto done = pthread_create(&_tid, nullptr, ThreadMain, static_cast<void*>(&task));
    if (0 not_eq done) {
      return done;
    }
    return pthread_detach(_tid);
  }
  //
  static void Yield() {
#if __APPLE__
    pthread_yield_np();
#elif __linux__
    pthread_yield();
#else
#error NOT supported platform!
#endif
  }
  //
 protected:
  static void* ThreadMain(void* arg) {
    auto task = static_cast<Task*>(arg);
    return task->Loop();
  }
  //
 private:
  pthread_t _tid;
  //
  DISALLOW_COPY_AND_ASSIGN(Thread);
}; // class Thread
//
} // namespace NAMESPACE
