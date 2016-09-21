// -*-coding:utf-8-unix;-*-
#pragma once
//
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include "atomic.hh"
#include "macros.hh"
//
namespace NAMESPACE {
//
template<typename Impl>
class Thread {
 public:
  enum: int {
    kInit,
    kRunning,
    kJoinable,
    kDetached,
    kExited
  };
  Thread(): _state(kInit), _tid(0), _attr() {
    static_assert(__is_base_of(Thread<Impl>, Impl), "Impl must inherit from Thread<Impl>");
  }
  //
  virtual ~Thread() {
    if (kJoinable == _state) {
      pthread_join(_tid, nullptr);
    }
  }
  //
  typedef Thread<Impl> Self;
  //
  static auto Run(Self& impl) -> int {
    return impl.RunImpl(impl);
  }
  //
  static auto Run(Self& impl, int cpu) -> int {
    return impl.RunImpl(impl, cpu);
  }
  //
  static auto RunBackgroud(Self& impl) -> int {
    return impl.RunBackgroudImpl(impl);
  }
  //
  static auto RunBackgroud(Self& impl, int cpu) -> int {
    return impl.RunBackgroudImpl(impl, cpu);
  }
  //
  template<typename Value>
  auto Join(Value& value) -> int {
    if (kJoinable not_eq _state) {
      return -1;
    }
    static_assert(sizeof(Value) <= sizeof(void*), "sizeof(Value) should less than sizeof(void*)");
    Value* result(nullptr);
    auto done = pthread_join(_tid, reinterpret_cast<void**>(&result));
    if (0 not_eq done) {
      return -1;
    }
    if (nullptr == result) {
      return -1;
    }
    value = *result;
    _state = kExited;
    return 0;
  }
  //
  auto Yield() -> void {
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
  //
  auto Wait(int desired) -> int {
    int expect(kRunning);
    while(not atomic::CAS(&_state, &expect, desired)) {
      expect = kRunning;
    }
    return 0;
  }
  //
  auto Signal() -> int {
    // set state
    int expect(kInit);
    int desired(kRunning);
    while(not atomic::CAS(&_state, &expect, desired)) {
      expect = kInit;
    }
    return 0;
  }
  //
  auto RunLoop() -> void* {
    auto done = Signal();
    if (0 not_eq done) {
      return nullptr;
    }
    return static_cast<Impl*>(this)->Loop();
  }
  //
  static auto ThreadMain(void* arg) -> void* {
    auto task = static_cast<Self*>(arg);
    return task->RunLoop();
  }
  //
  auto RunImpl(Self& impl) -> int {
    if (kInit not_eq _state) {
      return -1;
    }
    auto done = pthread_create(&_tid, nullptr, ThreadMain, &impl);
    if (0 not_eq done) {
      return -1;
    }
    return Wait(kJoinable);
  }
  //
  auto RunImpl(Self& impl, int cpu) -> int {
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
    done = pthread_create(&_tid, &_attr, ThreadMain, &impl);
    pthread_attr_destroy(&_attr);
    if (0 not_eq done) {
      return -1;
    }
    return Wait(kJoinable);
  }
  //
  auto RunBackgroudImpl(Self& impl) -> int {
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
    done = pthread_create(&_tid, &_attr, ThreadMain, &impl);
    pthread_attr_destroy(&_attr);
    if (0 not_eq done) {
      return -1;
    }
    return Wait(kDetached);
  }
  //
  auto RunBackgroudImpl(Self& impl, int cpu) -> int {
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
    done = pthread_create(&_tid, &_attr, ThreadMain, &impl);
    pthread_attr_destroy(&_attr);
    if (0 not_eq done) {
      return -1;
    }
    return Wait(kDetached);
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
