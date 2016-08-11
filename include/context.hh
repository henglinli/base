// -*-coding:utf-8-unix;-*-
// refactry from
// https://github.com/scylladb/seastar/blob/master/core/thread.cc
//
#pragma once
#ifdef __APPLE__
#define _XOPEN_SOURCE 700
#endif // __APPLE__
//
#include <ucontext.h>
#include <setjmp.h>
#include "macros.hh"
// split stack function
extern void __splitstack_getcontext(void *context[10]);
extern void __splitstack_setcontext(void *context[10]);
extern void *__splitstack_makecontext(size_t, void *context[10], size_t *);
extern void *__splitstack_resetcontext(void *context[10], size_t *);
extern void *__splitstack_find(void *, void *, size_t *, void **, void **, void **);
extern void __splitstack_block_signals(int *, int *);
extern void __splitstack_block_signals_context(void *context[10], int *, int *);
//
namespace NAMESPACE {
//
// usage
// struct _ContextTask
//     : public Context<_ContextTask, 8196> {
//   void Run();
// };
//
//
template<typename Task, size_t kStackSize>
class Context {
 public:
  int Make() {
    int done = getcontext(&_context);
    if (0 != done) {
      return done;
    }
    int block(0);
    size_t size(0);
    _context.uc_stack.ss_sp = __splitstack_makecontext(kStackSize, _stack_context, &size);
    _context.uc_stack.ss_size = size;
    _context.uc_stack.ss_flags = 0;
    _context.uc_link = nullptr;
    //
    block = 0;
    __splitstack_block_signals_context(_stack_context, &block, nullptr);
    makecontext(&_context, Exec, 1, this);
    return 0;
  }
  //
  int Set() {
    int block(0);
    __splitstack_block_signals (&block, nullptr);
    __splitstack_setcontext (_stack_context);
    setcontext (&_context);
  }
  //
  template<typename T, size_t S>
  [[gnu::no_split_stack]] void Swap(Context<T, S>& other);
  //
 protected:
  static void Exec(Task *task) {
    task->Run();
  }
 private:
  template<typename, size_t>
  friend class Routine;
  jmp_buf _jmpbuf;
  ucontext_t _context;
  void *_stack_context[10];
};
  template<typename Task, size_t kStackSize>
  template<typename T, size_t S>
  void Context<Task, kStackSize>::Swap(Context<T, S>& other) {
    __splitstack_getcontext(_stack_context);
    __splitstack_setcontext(other._stack_context);
    swapcontext(&_context, &other._context);
    __splitstack_setcontext(_stack_context);
  }
  //
} // namespace NAMESPACE
