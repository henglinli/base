// -*-coding:utf-8-unix;-*-
//
#pragma once
#ifdef __APPLE__
#define _XOPEN_SOURCE 700
#endif // __APPLE__
//
#include <stddef.h>
#include <assert.h>
#include "macros.hh"
#include "gnutm/queue.hh"
// split stack function
extern "C" {
  void __splitstack_getcontext(void* context[10]);
  void __splitstack_setcontext(void* context[10]);
  void *__splitstack_makecontext(size_t, void* context[10], size_t*);
  void __splitstack_releasecontext(void* context[10]);
  void *__splitstack_resetcontext(void* context[10], size_t*);
  void *__splitstack_find(void*, void*, size_t*, void**, void**, void**);
  void __splitstack_block_signals(int*, int*);
  void __splitstack_block_signals_context(void* context[10], int*, int*);
}
//
namespace NAMESPACE {
//
const size_t kDefaultStackSize(12*1024);
//
const size_t kPageSize(4*1024);
//
const size_t kGCCJmpBufferSize(5);
//
enum {
  FP = 0, // word 0 is frame address
  PC = 1, // word 1 is resume address
  SP = 2, // word 2 is stack address
};
//
class Fiber final: public gnutm::StailQ<Fiber>::Node {
public:
  template<typename Task>
  struct Routine {
    auto RunTask() -> void {
      static_cast<Task*>(this)->Run();
    }
  };
  //
  typedef void* Jmpbuf[kGCCJmpBufferSize];
  //
  thread_local static Jmpbuf env0;
  thread_local static Jmpbuf* current;
  //
  Fiber(): _link(nullptr), _env(), _context(), _stack_base(nullptr) {}
  //
  ~Fiber() {
    if (nullptr not_eq _stack_base) {
      __splitstack_releasecontext(_context);
    }
  }
  //
  auto MakeStack(size_t stack_size = kDefaultStackSize) -> bool;
  //
  template<typename Task>
  auto RunTask(Routine<Task>& task) -> void;
  //
  template<typename Task>
  auto Init(Routine<Task>& task) -> bool {
    static_assert(__is_base_of(Routine<Task>, Task), "Task must inherit from Routine<Task>");
    if (nullptr == _stack_base) {
      auto ok = MakeStack(kDefaultStackSize);
      if (not ok) {
        return false;
      }
    }
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
        _env[SP] = reinterpret_cast<char*>(_stack_base) - frame_size;
        Longjmp(_env);
      }
      // Note: new stack go here
      done = __builtin_setjmp(_env);
      if (0 == done) {
        Longjmp(env);
      }
      while(true) {
        task.RunTask();
        int google(0);
        printf("%p\n", &google);
        SwitchOut();
      }
    }
    //
    return true;
  }
  //
  auto SwitchIn() -> void {
    printf("from=%p\n", Fiber::current);
    auto from = Fiber::current;
    _link = Fiber::current;
    Fiber::current = &_env;
    Switch(from, &_env);
  }
  //
  auto SwitchOut() -> void {
    Fiber::current = _link;
    Switch(&_env, Fiber::current);
  }
  //
protected:
  //
  [[gnu::noinline]] static auto Longjmp(Jmpbuf env) -> void {
    __builtin_longjmp(env, 1);
  }
  //
  inline auto Switch(Jmpbuf* from, Jmpbuf* to) -> void {
    auto done = __builtin_setjmp(*from);
    if (0 == done) {
      Longjmp(*to);
    }
  }
  //
private:
  Jmpbuf* _link;
  Jmpbuf _env;
  void* _context[10];
  void* _stack_base;
  //
  DISALLOW_COPY_AND_ASSIGN(Fiber);
};
//
thread_local Fiber::Jmpbuf Fiber::env0;
thread_local Fiber::Jmpbuf* Fiber::current(nullptr);
//
namespace {
struct Initiator {
  Initiator() {
    Fiber::current = &Fiber::env0;
  }
};
static Initiator initiator;
}
//
auto Fiber::MakeStack(size_t stack_size) -> bool {
  stack_size = stack_size > kDefaultStackSize ? stack_size : kDefaultStackSize;
  size_t reminder = stack_size % kPageSize;
  if (0 not_eq reminder) {
    stack_size += kPageSize - reminder;
  }
  size_t size(0);
  _stack_base = __splitstack_makecontext(stack_size, _context, &size);
  if (nullptr == _stack_base) {
    return false;
  }
  // get stack base
  _stack_base = static_cast<char*>(_stack_base) + size;
  int block(0);
  __splitstack_block_signals_context(_context, &block, nullptr);
  return true;
}
//
template<typename Task>
auto Fiber::RunTask(Routine<Task>& task) -> void {
  static_assert(__is_base_of(Routine<Task>, Task), "Task must inherit from Routine<Task>");
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
      _env[SP] = reinterpret_cast<char*>(_stack_base) - frame_size;
      Longjmp(_env);
    }
    // Note: new stack go here
    task.RunTask();
    Longjmp(env);
  }
}
//
} // namespace NAMESPACE
