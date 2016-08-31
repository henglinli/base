// -*-coding:utf-8-unix;-*-
//
#pragma once
#ifdef __APPLE__
#define _XOPEN_SOURCE 700
#endif // __APPLE__
//
#include "macros.hh"
#include "gnutm/queue.hh"
// split stack function
extern "C" {
  void __splitstack_getcontext(void* context[10]);
  void __splitstack_setcontext(void* context[10]);
  void *__splitstack_makecontext(size_t, void* context[10], size_t*);
  void __splitstack_freecontext(void* context[10]);
  void *__splitstack_resetcontext(void* context[10], size_t*);
  void *__splitstack_find(void*, void*, size_t*, void**, void**, void**);
  void __splitstack_block_signals(int*, int*);
  void __splitstack_block_signals_context(void* context[10], int*, int*);
}
//
namespace NAMESPACE {
//
const size_t kStackSize(SIGSTKSZ);
const size_t kPageSize(4096);
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
      return static_cast<Task*>(this)->Run();
    }
  };
  //
  Fiber()
      : _link(nullptr)
      , _env()
      , _stack_context()
      , _stack(nullptr)
      , _stack_size(0) {}
  //
  template<typename Task>
      bool Init(Routine<Task>& task, size_t stack_size = kStackSize) {
    static_assert(__is_base_of(Routine<Task>, Task), "Task must inherit from Routine<Task>");
    //
    auto ok = Makestack(stack_size);
    if (not ok) {
      return false;
    }
    //
    Run(task);
    //
    return true;
  }
  //
  template<typename Task>
      void Run(Routine<Task>& task) {
    bool initialized(false);
    Jmpbuf env;
    auto done = __builtin_setjmp(env);
    if (0 == done) {
      auto done = __builtin_setjmp(_env);
      if (0 == done) {
        size_t frame_size = static_cast<char*>(_env[FP]) - static_cast<char*>(_env[SP]);
        if (frame_size bitand (16-1)) {
          frame_size += 16 - (frame_size bitand (16-1));
        }
        assert(frame_size < 4096);
        printf("frame_size %lu\n", frame_size);
        _env[SP] = reinterpret_cast<char*>(_stack) + _stack_size - frame_size;
        printf("%p\n", _env[SP]);
        SwitchIn();
      }
      // Note: new stack go here
      if (initialized) {
        task.RunTask();
      }
      initialized = true;
      printf("env %p\n", &env);
      Longjmp(env);
      __builtin_unreachable();
    }
  }
  //
  [[gnu::noinline]] void SwitchIn() {
    __builtin_longjmp(_env, 1);
  }
  //
protected:
  typedef void* Jmpbuf[kGCCJmpBufferSize];
  //
  [[gnu::noinline]] void Longjmp(Jmpbuf env) {
    __builtin_longjmp(env, 1);
  }
  //
  bool Makestack(size_t stack_size) {
    stack_size = stack_size > kStackSize ? stack_size : kStackSize;
    size_t reminder = stack_size % kPageSize;
    if (0 not_eq reminder) {
      stack_size += kPageSize - reminder;
    }
    _stack = __splitstack_makecontext(stack_size, _stack_context, &_stack_size);
    if (nullptr == _stack) {
      return false;
    }
    printf("%p %lu\n", (char*)_stack + _stack_size , _stack_size);
    int block(0);
    __splitstack_block_signals_context(_stack_context, &block, nullptr);
    //
    return true;
  }
  //
private:
  Fiber* _link;
  Jmpbuf _env;
  void* _stack_context[10];
  void* _stack;
  size_t _stack_size;
  //
  DISALLOW_COPY_AND_ASSIGN(Fiber);
};
} // namespace NAMESPACE
