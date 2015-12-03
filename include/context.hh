// -*-coding:utf-8-unix;-*-
// refactry from
// https://github.com/scylladb/seastar/blob/master/core/thread.cc
//
#pragma once
#ifdef __APPLE__
#define _XOPEN_SOURCE 700
#endif // __APPLE__
//
#define MMAP
#ifdef MMAP
#include <sys/mman.h>
#endif // MMAP
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
class Heap {
 public:
  Heap();
  //
  ~Heap();
  //
  void* Mmap(size_t size);
  //
 private:
  void* _data;
  size_t _size;
};
//
const size_t kDefaultStackSize(SIGSTKSZ);
//
template<typename Task, size_t kStackSize = kDefaultStackSize>
class Context {
 public:
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
  Heap _stack;
  jmp_buf _jmpbuf;
};
//
Heap::Heap()
    : _data(nullptr)
    , _size(0) {
  // nil
}
//
Heap::~Heap() {
  if (nullptr == _data or 0 == _size) {
#ifdef MMAP
    munmap(_data, _size);
#else // MMAP
    free(_data);
#endif // MMAP
  }
}
//
void* Heap::Mmap(size_t size) {
  if (0 < size) {
#ifdef MMAP
    _size = size;
    _data = mmap(nullptr, size, PROT_EXEC bitor PROT_READ bitor PROT_WRITE,
                 MAP_PRIVATE bitor MAP_ANONYMOUS, -1, 0);
#else // MMAP
    _data = malloc(size);
#endif // MMAP
  }
  return _data;
}
//
template<typename Task, size_t kStackSize>
bool Context<Task, kStackSize>::Init(Context& context) {
  void* stack = context._stack.Mmap(kStackSize);
  if (nullptr == stack) {
    return false;
  }
  typedef Context<Task, kStackSize> Self;
  ucontext_t initial_context;
  int done = getcontext(&initial_context);
  if (0 not_eq done) {
    return false;
  }
  //
  initial_context.uc_stack.ss_sp = stack;
  initial_context.uc_stack.ss_size = kStackSize;
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
