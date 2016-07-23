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
/*
  usage
  struct Task {
    void Run() {
        // dosomething here
    }
  };
  Task t;
  Cotoutine r;
  r.Init(t);
  r.SwitchIn();
*/
const size_t kDefaultStackSize(SIGSTKSZ);
//
template<size_t kStackSize = kDefaultStackSize>
class Coroutine {
public:
  typedef Coroutine<kStackSize> Self;
  //
  Coroutine()
    : _stack(nullptr)
    , _link()
    , _context() {
    //
  }
  //
  ~Coroutine() {
    if (nullptr not_eq _stack) {
      delete[] _stack;
    }
  }
  //
  template<typename Task>
  bool Init(Task& task) {
    //
    if (nullptr == _stack) {
      _stack = new char[kStackSize+1];
    }
    //
    int done = getcontext(&_context);
    if (0 not_eq done) {
      return false;
    }
    _context.uc_stack.ss_sp = _stack;
    _context.uc_stack.ss_size = kStackSize;
    _context.uc_link = nullptr;
    //
    ucontext_t tmp;
    makecontext(&_context, reinterpret_cast<void (*)()>(Self::RunTask<Task>), 3, &task, &_link, &tmp);
    swapcontext(&tmp, &_context);
    return true;
  }
  //
  void Switch(Coroutine& other) {
    int done = _setjmp(other._link);
    if (0 == done) {
      _longjmp(_link, 1);
    }
  }
  //
 protected:
  template<typename Task>
  static void RunTask(Task* task, jmp_buf *cur, ucontext_t* prv) {
    int done = _setjmp(*cur);
    if (0 == done) {
      setcontext(prv);
    }
    task->Run();
  }
  //
 private:
  char* _stack;
  jmp_buf _link;
  ucontext_t _context;
};
//
} // namespace NAMESPACE
