// -*-coding:utf-8-unix;-*-
// refactry from
// https://github.com/scylladb/seastar/blob/master/core/thread.cc
//
#pragma once
#ifdef __APPLE__
#define _XOPEN_SOURCE 700
#endif
#include <ucontext.h>
#include <setjmp.h>
#include "macros.hh"
//
namespace NAMESPACE {
//
// usage
// struct _ContextTask
//     : public Context<_ContextTask, 1> {
//   void Run();
// };
//
const size_t kDefaultStackSize(SIGSTKSZ);
//
template<typename Task, size_t kStackSize = kDefaultStackSize>
class Context {
 public:
  //
  static bool Init(Context& context);
  //
  static void SwitchIn(Context& to);
  //
  static void SwitchOut(Context& from);
  //
 protected:
  static void Routine(Context* context);
  //
 private:
  char _stack[kStackSize];
  jmp_buf _jmpbuf;
};
//
template<typename Task, size_t kStackSize>
bool Context<Task, kStackSize>::Init(Context& context) {
  typedef Context<Task, kStackSize> Self;
  // TODO(lee): why stack overflow if NOT memset
  memset(context._stack, 0, sizeof(context._stack));
  ucontext_t initial_context;
  int done = getcontext(&initial_context);
  if (0 not_eq done) {
    return false;
  }
  //
  initial_context.uc_stack.ss_sp = context._stack;
  initial_context.uc_stack.ss_size = sizeof(context._stack);
  initial_context.uc_link = nullptr;
  //
  makecontext(&initial_context,
              reinterpret_cast<void (*)()>(Self::Routine),
              1, &context);
  done = setjmp(context._jmpbuf);
  if (0 == done) {
    setcontext(&initial_context);
  }
  return true;
}
//
template<typename Task, size_t kStackSize>
void Context<Task, kStackSize>::SwitchIn(Context& to) {
  int done = setjmp(to._jmpbuf);
  if (0 == done) {
    longjmp(to._jmpbuf, 1);
  }
}
//
template<typename Task, size_t kStackSize>
void Context<Task, kStackSize>::SwitchOut(Context& from) {
  int done = setjmp(from._jmpbuf);
  if (0 == done) {
    longjmp(from._jmpbuf, 1);
  }
}
//
template<typename Task, size_t kStackSize>
void Context<Task, kStackSize>::Routine(Context* context) {
  static_cast<Task*>(context)->Run();
  longjmp(context->_jmpbuf, 1);
}
//
} // namespace NAMESPACE
