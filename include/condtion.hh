// -*-coding:utf-8-unix;-*-
#pragma once
//
#include <pthread.h>
#include "macros.hh"
//
namespace NAMESPACE {
//
template<typename Impl>
class Condition {
 public:
  Condition() = default;
  ~Condition() = default;
  //
  static auto Wait(Condition<Impl>& impl) -> int {
    return impl.Wait();
  }
  //
  static auto Signal(Condition<Impl>& impl) -> int {
    return impl.Signal();
  }
  //
 protected:
  //
  auto Wait() -> int {
    return static_cast<Impl*>(this)->WaitImpl();
  }
  //
  auto Signal() -> int {
    return static_cast<Impl*>(this)->SignalImpl();
  }
  //
 private:
  DISALLOW_COPY_AND_ASSIGN(Condition);
};
//
class PthreadCond final: public Condition<PthreadCond> {
 public:
  PthreadCond(): _mutex(PTHREAD_MUTEX_INITIALIZER), _cond(PTHREAD_COND_INITIALIZER) {
    pthread_mutex_init(&_mutex, nullptr);
    pthread_cond_init(&_cond, nullptr);
  }
  //
  ~PthreadCond() {
    pthread_cond_destroy(&_cond);
    pthread_mutex_destroy(&_mutex);
  }
  //
 protected:
  //
  friend class Condition<PthreadCond>;
  //
  auto WaitImpl() -> int {
    auto done = pthread_mutex_lock(&_mutex);
    if (0 not_eq done) {
      return -1;
    }
    auto result = pthread_cond_wait(&_cond, &_mutex);
    done = pthread_mutex_unlock(&_mutex);
    if (0 not_eq done) {
      return -1;
    }
    return result;
  }
  //
  auto SignalImpl() -> int {
    auto done = pthread_mutex_lock(&_mutex);
    if (0 not_eq done) {
      return -1;
    }
    auto result = pthread_cond_signal(&_cond);
    done = pthread_mutex_unlock(&_mutex);
    if (0 not_eq done) {
      return -1;
    }
    return result;
  }
  //
 private:
  pthread_mutex_t _mutex;
  pthread_cond_t _cond;
  //
  DISALLOW_COPY_AND_ASSIGN(PthreadCond);
};
//
} // namespace NAMESPACE
