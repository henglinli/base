// -*-coding:utf-8-unix;-*-
#pragma once
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include "macros.hh"
//
namespace NAMESPACE {
//
class Thread {
 public:
  template<typename Task>
  struct Routine {
    void* RunLoop() {
      return static_cast<Task*>(this)->Loop();
    }
  };
  //
  Thread(): _tid(-1)
          , _detach_state(PTHREAD_CREATE_JOINABLE)
          , _attr() {}
  //
  template<typename Task>
  int Run(Task& task) {
    static_assert(__is_base_of(Routine<Task>, Task), "Task must inherit from Routine<Task>");
    return pthread_create(&_tid, nullptr, ThreadMain<Task>, static_cast<void*>(&task));
  }
  //
  template<typename Task>
  int Run(Task& task, int cpu) {
    static_assert(__is_base_of(Routine<Task>, Task), "Task must inherit from Routine<Task>");
    auto done = pthread_attr_init(&_attr);
    if (0 not_eq done) {
      return -1;
    }
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpu, &set);
    done = pthread_attr_setaffinity_np(&_attr, sizeof(set), &set);
    if (0 not_eq done) {
      return -1;
    }
    auto result = pthread_create(&_tid, &_attr, ThreadMain<Task>, static_cast<void*>(&task));
    done = pthread_attr_destroy(&_attr);
    if (0 not_eq done) {
      return -1;
    }
    return result;
  }
  //
  template<typename Value>
  int Join(Value& ptr) {
    static_assert(sizeof(Value) <= sizeof(void*), "sizeof(Value) should less than sizeof(void*)");
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
  template<typename Task>
  int RunBackgroud(Task& task) {
    auto done = pthread_attr_init(&_attr);
    if (0 not_eq done) {
      return -1;
    }
    done = pthread_attr_setdetachstate(&_attr, PTHREAD_CREATE_DETACHED);
    if (0 not_eq done) {
      return -1;
    }
    auto result = pthread_create(&_tid, &_attr, ThreadMain<Task>, static_cast<void*>(&task));
    done = pthread_attr_destroy(&_attr);
    if (0 not_eq done) {
      return -1;
    }
    return result;
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
  template<typename Task>
  static void* ThreadMain(void* arg) {
    auto task = static_cast<Routine<Task>*>(arg);
    return task->RunLoop();
  }
  //
 private:
  pthread_t _tid;
  int _detach_state;
  pthread_attr_t _attr;
  //
  DISALLOW_COPY_AND_ASSIGN(Thread);
}; // class Thread
//
} // namespace NAMESPACE
