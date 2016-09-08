// -*-coding:utf-8-unix;-*-
//
#pragma once
#ifdef __APPLE__
#define _XOPEN_SOURCE 700
#endif // __APPLE__
//
#include <stddef.h>
#include <assert.h>
#include "queue.hh"
#include "macros.hh"
//
namespace NAMESPACE {
//
namespace {
//
constexpr size_t kPageSize(4*1024);
//
constexpr size_t kDefaultStackSize(4*kPageSize);
//
constexpr size_t kGCCJmpBufferSize(5);
//
}
//
class Fiber final: public TailQ<Fiber>::Node {
public:
  //
  enum {
    FP = 0, // word 0 is frame address
    PC = 1, // word 1 is resume address
    SP = 2, // word 2 is stack address
  };
  //
  typedef void* Jmpbuf[kGCCJmpBufferSize];
  //
  thread_local static Fiber fiber0;
  thread_local static Fiber* current;
  thread_local static TailQ<Fiber> idle_list;
  //
  static auto Fork() -> Fiber*;
  static auto Join(Fiber*& fiber) -> void;
  //
  template<typename Task>
  auto Switch() -> void;
  //
protected:
  //
  Fiber(): _parent(nullptr), _children(), _env(), _stack() {}
  //
  [[gnu::noinline]] static auto Longjmp(Jmpbuf env) -> void {
    __builtin_longjmp(env, 1);
  }
  //
private:
  Fiber* _parent;
  TailQ<Fiber> _children;
  Jmpbuf _env;
  char _stack[kDefaultStackSize-sizeof(Jmpbuf)-2*sizeof(TailQ<Fiber>)-2*sizeof(Fiber*)];
  //
  DISALLOW_COPY_AND_ASSIGN(Fiber);
};
//
thread_local Fiber Fiber::fiber0;
thread_local Fiber* Fiber::current(nullptr);
thread_local TailQ<Fiber> Fiber::idle_list;
//
namespace {
struct Initiator {
  Initiator() {
    Fiber::current = &Fiber::fiber0;
  }
};
static Initiator initiator;
}
//
auto Fiber::Fork() -> Fiber* {
  auto fiber = idle_list.Pop();
  static_assert(sizeof(*fiber) == kDefaultStackSize, "invalid stack size");
  if (nullptr == fiber) {
    auto tmp = __builtin_malloc(sizeof(*fiber));
    if (nullptr == tmp) {
      return nullptr;
    }
    fiber = static_cast<Fiber*>(tmp);
  }
  //
  current->_children.Push(fiber);
  fiber->_parent = current;
  fiber->_children.Init();
  //
  return fiber;
}
//
auto Fiber::Join(Fiber*& fiber) -> void {
  if (nullptr not_eq fiber) {
    idle_list.Push(fiber);
    fiber = nullptr;
  }
}
//
template<typename Task>
auto Fiber::Switch() -> void {
  auto parent = current;
  current = this;
  //
  Jmpbuf env;
  auto done = __builtin_setjmp(env);
  if (0 == done) {
    done = __builtin_setjmp(_env);
    if (0 == done) {
      size_t frame_size = static_cast<char*>(_env[FP]) - static_cast<char*>(_env[SP]);
      if (frame_size bitand (16-1)) {
        frame_size += 16 - (frame_size bitand (16-1));
      }
      assert(frame_size < 4096);
      _env[SP] = _stack + sizeof(_stack) - frame_size;
      Longjmp(_env);
    }
    // Note: new stack go here
    Task task;
    task.Run();
    Longjmp(env);
  }
  current = parent;
}
//
} // namespace NAMESPACE
