// -*-coding:utf-8-unix;-*-
#pragma once
#include <pthread.h>
#include <stdint.h>
#include "macros.hh"
//
namespace NAMESPACE {
//
template<typename Value>
class _Task {
 public:
  virtual Value* Loop() = 0;
};
//
template<typename Task, typename Value>
class Thread {
 public:
  int Run(Task* task) {
    return pthread_create(&_tid, nullptr,
                          ThreadMain, static_cast<void*>(task));
  }
  //
  int Join(Value* ptr) {
    int done = pthread_join(_tid, reinterpret_cast<void**>(&_result));
      if (0 not_eq done) {
        return done;
      }
      *ptr = *_result;
      return done;
  }
  //
  int RunBackgroud(Task* task) {
    int done = pthread_create(&_tid, nullptr,
                              ThreadMain, static_cast<void*>(task));
    if (0 not_eq done) {
      return done;
    }
    return pthread_detach(_tid);
  }
  //
 protected:
  static void* ThreadMain(void* arg) {
    Task* task = static_cast<Task*>(arg);
    return task->Loop();
  }
  //
 private:
  Value* _result;
  pthread_t _tid;
}; // class Thread
//
} // namespace NAMESPACE
