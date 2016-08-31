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
    void RunTask() {
      static_cast<Task*>(this)->Run();
    }
  };
  //
  Fiber()
      : _link(nullptr)
      , _env()
      , _context()
      , _stack_base(nullptr) {}
  //
  ~Fiber() {
    if (nullptr not_eq _stack_base) {
      __splitstack_releasecontext(_context);
    }
  }
  //
  bool MakeStack(size_t stack_size = kDefaultStackSize) {
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
      void RunTask(Routine<Task>& task) {
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
      __builtin_unreachable();
    }
  }
  //
protected:
  typedef void* Jmpbuf[kGCCJmpBufferSize];
  //
  [[gnu::noinline]] static void Longjmp(Jmpbuf env) {
    __builtin_longjmp(env, 1);
  }
  //
private:
  Fiber* _link;
  Jmpbuf _env;
  void* _context[10];
  void* _stack_base;
  //
  DISALLOW_COPY_AND_ASSIGN(Fiber);
};
} // namespace NAMESPACE
