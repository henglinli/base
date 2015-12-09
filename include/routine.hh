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
class Routine {
 public:
  bool Init(Task& task);
  //
  void SwitchIn(Task& to);
  //
  void SwitchOut(Task& from);
  //
 protected:
  static void RunTask(Task* task);
  //
 private:
  char _stack[kStackSize];
  jmp_buf *_link;
  ucontext_t _context;
};
//
template<typename Task, size_t kStackSize>
bool Routine<Task, kStackSize>::Init(Task& task) {
  _link = &(task._jmpbuf);
  //
  typedef Routine<Task, kStackSize> Self;
  _stack[sizeof(_stack) - 1] = 0;
  int done = getcontext(&_context);
  if (0 not_eq done) {
    return false;
  }
  //
  _context.uc_stack.ss_sp = _stack;
  _context.uc_stack.ss_size = sizeof(_stack);
  _context.uc_link = nullptr;
  //
  makecontext(&_context,
              reinterpret_cast<void (*)()>(Self::RunTask),
              1, &task);
  done = setjmp(*_link);
  if (0 == done) {
    setcontext(&_context);
  }
  return true;
}
//
template<typename Task, size_t kStackSize>
void Routine<Task, kStackSize>::SwitchIn(Task& to) {
  jmp_buf* jmpbuf = &(to._jmpbuf);
  int done = setjmp(*jmpbuf);
  if (0 == done) {
    longjmp(*jmpbuf, 1);
  }
}
//
template<typename Task, size_t kStackSize>
void Routine<Task, kStackSize>::SwitchOut(Task& from) {
  jmp_buf* jmpbuf = &(from._jmpbuf);
  int done = setjmp(*jmpbuf);
  if (0 == done) {
    longjmp(*_link, 1);
  }
}
//
template<typename Task, size_t kStackSize>
void Routine<Task, kStackSize>::RunTask(Task* task) {
  task->Run();
  longjmp(task->_jmpbuf, 1);
}
//
} // namespace NAMESPACE
