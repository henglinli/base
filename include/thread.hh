// -*-coding:utf-8-unix;-*-
#pragma once
#include <pthread.h>
#include <stdint.h>
#include "macros.hh"
//
namespace NAMESPACE {
//
template<typename Value>
class _Worker {
 public:
  virtual Value* Loop() = 0;
};
//
template<typename Task, typename Value>
class Thread {
 public:
  Thread(): _result(nullptr), _tid() {}
  //
  int Run(Task& task) {
    return pthread_create(&_tid, nullptr, ThreadMain, static_cast<void*>(&task));
  }
  //
  int Join(Value* ptr) {
    auto done = pthread_join(_tid, reinterpret_cast<void**>(&_result));
    if (0 not_eq done) {
      return done;
    }
    *ptr = *_result;
    return done;
  }
  //
  inline int Cancel() {
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
  Value* _result;
  pthread_t _tid;
  //
  DISALLOW_COPY_AND_ASSIGN(Thread);
}; // class Thread
//
} // namespace NAMESPACE
