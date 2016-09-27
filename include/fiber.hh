// -*-coding:utf-8-unix;-*-
//
#pragma once
#ifdef __APPLE__
#define _XOPEN_SOURCE 700
#endif // __APPLE__
//
#include <errno.h>
#include "queue.hh"
#include "mpmc/queue.hh"
#include "processor.hh"
#ifdef __linux__
#include "linux/futex.hh"
#endif // __linux__
//
#ifdef __clang__
extern "C" int __cxa_thread_atexit(void (*func)(void*), void *obj, void *dso_symbol) {
  int __cxa_thread_atexit_impl(void (*)(void*), void *, void *);
  return __cxa_thread_atexit_impl(func, obj, dso_symbol);
}
#endif // __clang__
namespace NAMESPACE {
//
const size_t kDefautQSize = 1<<16;
const size_t kPageSize = 4*1024;
const size_t kDefaultStackSize = 4*kPageSize;
//
template<size_t kQSize = kDefautQSize, size_t kStackSize = kDefaultStackSize>
class Fiber final: public TailQ<Fiber<kQSize, kStackSize> >::Node {
public:
  //
  typedef Fiber<kQSize, kStackSize> Self;
  thread_local static Fiber fiber0;
  thread_local static Fiber* parent;
  static mpmc::BoundedQ<Self, kQSize> idleq;
  //
  static auto Fork(void(*routine)(void*), void* arg) -> Self*;
  //
  inline auto Detach() -> void {
    _state = kDetached;
  }
  //
  static auto Join() -> void;
  //
  static auto Switch(Self* fiber) -> void;
  //
protected:
  //
  static const size_t kGCCJmpBufferSize = 5;
  //
  enum {
    FP = 0, // word 0 is frame address
    PC = 1, // word 1 is resume address
    SP = 2, // word 2 is stack address
  };
  //
  typedef void* Jmpbuf[kGCCJmpBufferSize];
  //
  enum: int {
    kInit,
    kJoinable,
    kDetached,
    kFinished,
    kError
  };
  //
  Fiber(): _routine(nullptr), _arg(nullptr), _state(kInit)
         , _children(), _env(), _stack() {}
  //
  [[gnu::noinline]] static auto Longjmp(Jmpbuf env) -> void {
    __builtin_longjmp(env, 1);
  }
  //
  static auto New() -> Self*;
  //
  auto Execute() -> void;
  //
  auto SpinWait() -> void;
  //
private:
  void(*_routine)(void*);
  void* _arg;
  int _state;
  TailQ<Self> _children;
  Jmpbuf _env;
  char _stack[kStackSize-sizeof(typename TailQ<Self>::Node)-sizeof(Jmpbuf)
              -sizeof(TailQ<Self>)-3*sizeof(void*)];
  //
  DISALLOW_COPY_AND_ASSIGN(Fiber);
};
//
template<size_t kQSize, size_t kStackSize>
thread_local Fiber<kQSize, kStackSize> Fiber<kQSize, kStackSize>::fiber0;
template<size_t kQSize, size_t kStackSize>
thread_local Fiber<kQSize, kStackSize>* Fiber<kQSize, kStackSize>::parent(&Fiber<kQSize, kStackSize>::fiber0);
template<size_t kQSize, size_t kStackSize>
mpmc::BoundedQ<Fiber<kQSize, kStackSize>, kQSize> Fiber<kQSize, kStackSize>::idleq;
//
template<size_t kQSize, size_t kStackSize>
auto Fiber<kQSize, kStackSize>::New() -> Self* {
  auto fiber = idleq.Pop();
  static_assert(sizeof(*fiber) == kDefaultStackSize, "invalid stack size");
  if (nullptr == fiber) {
#ifdef __clang__
    auto tmp = new Self;
#else // __clang__
    auto tmp = __builtin_malloc(sizeof(*fiber));
#endif // __clang__
    if (nullptr == tmp) {
      return nullptr;
    }
    fiber = static_cast<Self*>(tmp);
  }
  return fiber;
}
//
template<size_t kQSize, size_t kStackSize>
auto Fiber<kQSize, kStackSize>::Fork(void(*routine)(void*), void* arg) -> Self* {
  auto fiber = New();
  //
  fiber->_routine = routine;
  fiber->_arg = arg;
  fiber->_children.Init();
  atomic::Store(&(fiber->_state), static_cast<int>(kInit));
  //
  parent->_children.Push(fiber);
  //
  return fiber;
}
//
template<size_t kQSize, size_t kStackSize>
auto Fiber<kQSize, kStackSize>::SpinWait() -> void {
  const int kSpinCount(30);
  int state(kInit);
  int done(0);
  while(true) {
    // spin
    for (auto i(kSpinCount); 0 < i; --i) {
      state = atomic::Load(&_state);
      if (kFinished == state) {
        // _state is kFinished, spin failed
        return;
      }
      Processor::Relax();
    }
    // spin done, wait
    done = Linux::Futex::Wait(&_state, kJoinable);
    if (0 not_eq done and EAGAIN not_eq errno) {
      // wait failed
      atomic::Store(&_state, static_cast<int>(kError));
      return;
    }
  }
}
//
template<size_t kQSize, size_t kStackSize>
auto Fiber<kQSize, kStackSize>::Join() -> void {
  int state(kInit);
  for (auto fiber = parent->_children.Pop();
       nullptr != fiber; fiber = parent->_children.Pop()) {
    state = atomic::Load(&(fiber->_state));
    switch (state) {
      case kInit: {
        Switch(fiber);
        break;
      }
      case kJoinable: {
        fiber->SpinWait();
        break;
      }
      default: {
        break;
      }
    }
  }
}
//
template<size_t kQSize, size_t kStackSize>
auto Fiber<kQSize, kStackSize>::Execute() -> void {
  Jmpbuf env;
  auto done = __builtin_setjmp(env);
  if (0 == done) {
    done = __builtin_setjmp(_env);
    if (0 == done) {
      size_t frame_size = static_cast<char*>(_env[FP]) - static_cast<char*>(_env[SP]);
      if (frame_size bitand (16-1)) {
        frame_size += 16 - (frame_size bitand (16-1));
      }
      _env[SP] = _stack + sizeof(_stack) - frame_size;
      Longjmp(_env);
    }
    // Note: new stack go here
    _routine(_arg);
    Longjmp(env);
  }
}
//
template<size_t kQSize, size_t kStackSize>
auto Fiber<kQSize, kStackSize>::Switch(Self* fiber) -> void {
  int expect(kInit);
  int desired(kJoinable);
  if (atomic::CAS(&(fiber->_state), &expect, desired)) {
    auto prev = parent;
    parent = fiber;
    fiber->Execute();
    parent = prev;
    //
    const int kWaiters(1);
    auto waiters = Linux::Futex::Wake(&(fiber->_state), kWaiters);
    if (0 > waiters) {
      desired = kFinished;
    } else {
      desired = kError;
    }
    atomic::Store(&(fiber->_state), desired);
    //
    idleq.Push(fiber);
  }
}
//
} // namespace NAMESPACE
