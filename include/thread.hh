// -*-coding:utf-8-unix;-*-
#pragma once
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include "macros.hh"
//
namespace NAMESPACE {
//
template<typename Impl>
class Thread {
 public:
  enum: int {
    kInit,
    kJoinable,
    kDetached,
    kExited
  };
  Thread(): _state(kInit), _tid(0), _attr() {
    static_assert(__is_base_of(Thread<Impl>, Impl), "Impl must inherit from Thread<Impl>");
  }
  //
  ~Thread() {
    if (kJoinable == _state) {
      pthread_join(_tid, nullptr);
    }
  }
  //
  typedef Thread<Impl> Self;
  //
  static int Run(Self& impl) {
    return impl.RunImpl(impl);
  }
  //
  static int Run(Self& impl, int cpu) {
    return impl.RunImpl(impl, cpu);
  }
  //
  static int RunBackgroud(Self& impl) {
    return impl.RunBackgroudImpl(impl);
  }
  //
  static int RunBackgroud(Self& impl, int cpu) {
    return impl.RunBackgroudImpl(impl, cpu);
  }
  //
  template<typename Value>
  int Join(Value& value) {
    if (kJoinable not_eq _state) {
      return -1;
    }
    static_assert(sizeof(Value) <= sizeof(void*), "sizeof(Value) should less than sizeof(void*)");
    Value* result(nullptr);
    auto done = pthread_join(_tid, reinterpret_cast<void**>(&result));
    if (0 not_eq done) {
      return done;
    }
    value = *result;
    _state = kExited;
    return done;
  }
  //
  void Yield() {
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
  void* RunLoop() {
    return static_cast<Impl*>(this)->Loop();
  }
  //
  static void* ThreadMain(void* arg) {
    auto task = static_cast<Self*>(arg);
    return task->RunLoop();
  }
  //
  int RunImpl(Self& impl) {
    if (kInit not_eq _state) {
      return -1;
    }
    auto done = pthread_create(&_tid, nullptr, ThreadMain, static_cast<void*>(&impl));
    if (0 == done) {
      _state = kJoinable;
    }
    return done;
  }
  //
  int RunImpl(Self& impl, int cpu) {
    if (kInit not_eq _state) {
      return -1;
    }
    auto done = pthread_attr_init(&impl._attr);
    if (0 not_eq done) {
      return -1;
    }
    // setaffinity
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpu, &set);
    done = pthread_attr_setaffinity_np(&_attr, sizeof(set), &set);
    if (0 not_eq done) {
      return -1;
    }
    auto result = pthread_create(&_tid, &_attr, ThreadMain, static_cast<void*>(&impl));
    done = pthread_attr_destroy(&_attr);
    if (0 not_eq done) {
      return -1;
    }
    if (0 == result) {
      _state = kJoinable;
    }
    return result;
  }
  //
  int RunBackgroudImpl(Self& impl) {
    if (kInit not_eq _state) {
      return -1;
    }
    auto done = pthread_attr_init(&_attr);
    if (0 not_eq done) {
      return -1;
    }
    // setdetachstate
    done = pthread_attr_setdetachstate(&_attr, PTHREAD_CREATE_DETACHED);
    if (0 not_eq done) {
      return -1;
    }
    auto result = pthread_create(&_tid, &_attr, ThreadMain, static_cast<void*>(&impl));
    done = pthread_attr_destroy(&_attr);
    if (0 not_eq done) {
      return -1;
    }
    if (0 == result) {
      _state = kDetached;
    }
    return result;
  }
  //
  int RunBackgroudImpl(Self& impl, int cpu) {
    if (kInit not_eq _state) {
      return -1;
    }
    auto done = pthread_attr_init(&_attr);
    if (0 not_eq done) {
      return -1;
    }
    // setaffinity
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpu, &set);
    done = pthread_attr_setaffinity_np(&_attr, sizeof(set), &set);
    if (0 not_eq done) {
      return -1;
    }
    // setdetachstate
    done = pthread_attr_setdetachstate(&_attr, PTHREAD_CREATE_DETACHED);
    if (0 not_eq done) {
      return -1;
    }
    // create thread
    auto result = pthread_create(&_tid, &_attr, ThreadMain, static_cast<void*>(&impl));
    done = pthread_attr_destroy(&_attr);
    if (0 not_eq done) {
      return -1;
    }
    if (0 == result) {
      _state = kDetached;
    }
    return result;
  }
  //
 private:
  int _state;
  pthread_t _tid;
  pthread_attr_t _attr;
  //
  DISALLOW_COPY_AND_ASSIGN(Thread);
};
//
} // namespace NAMESPACE
