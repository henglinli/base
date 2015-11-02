// -*-coding:utf-8-unix;-*-
#pragma once
#include <pthread.h>
#include <stdint.h>
#include "macros.hh"
//
namespace base {
//
class Task {
 public:
  virtual int* Loop() = 0;
};
//
template<typename Task>
class Thread {
 public:
  int Run(Task* task) {
    return pthread_create(&_tid, nullptr,
                          ThreadMain, static_cast<void*>(task));
  }
  //
  int Join() {
    int done = pthread_join(_tid, reinterpret_cast<void**>(&_result));
      if (0 == done) {
        return (*_result);
      }
      return done;
  }
  //
 protected:
  static void* ThreadMain(void* arg) {
    Task* task = static_cast<Task*>(arg);
    return task->Loop();
  }
  //
 private:
  int* _result;
  pthread_t _tid;
  pthread_attr_t _attr;
}; // class Thread
//
} // namespace base
