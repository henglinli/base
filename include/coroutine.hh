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
#include "list.hh"
// split stack function
extern "C" {
  void __splitstack_getcontext(void *context[10]);
  void __splitstack_setcontext(void *context[10]);
  void *__splitstack_makecontext(size_t, void *context[10], size_t *);
  void *__splitstack_resetcontext(void *context[10], size_t *);
  void *__splitstack_find(void *, void *, size_t *, void **, void **, void **);
  void __splitstack_block_signals(int *, int *);
  void __splitstack_block_signals_context(void *context[10], int *, int *);
}
//
namespace NAMESPACE {
  /*
    usage
    struct Task: pulbic Coroutine<Task> {
    void Run() {
    // dosomething here
    }
    };
    Task t;
  */
  const size_t kStackSize(SIGSTKSZ);
  //
  class Context: public List<Context>::Node {
  public:
    Context() = default;
    ~Context() = default;
    //
  private:
    Context *_link;
    jmp_buf _jmpbuf;
    void *_stack_context[10];
    //
    template<typename>
    friend class Coroutine;
    //
    DISALLOW_COPY_AND_ASSIGN(Context);
  };
  //
  static thread_local List<Context> context_list;
  static thread_local Context context0;
  static thread_local Context *current(&context0);
  //
  template<typename Runner>
  class Coroutine {
  public:
    Coroutine() = default;
    ~Coroutine() = default;
    //
    static bool Init(Coroutine<Runner>& coroutine) {
      return coroutine.init(coroutine);
    }
    //
    [[gnu::no_split_stack]] void SwitchIn();
    [[gnu::no_split_stack]] void SwitchOut();
    //
  protected:
    [[gnu::no_split_stack]] bool init(Coroutine<Runner>& coroutine);
    //
    [[gnu::no_split_stack]] static void run(Runner *runner);
    //
  private:
    Context _context;
    //
    DISALLOW_COPY_AND_ASSIGN(Coroutine);
  };
  //
  template<typename Runner>
  void Coroutine<Runner>::run(Runner *runner) {
    runner->Run();
    current = runner->_context._link;
    _longjmp(current->_jmpbuf, 1);
  }
  //
  template<typename Runner>
  bool Coroutine<Runner>::init(Coroutine<Runner>& coroutine) {
    ucontext_t context;
    auto done = getcontext(&context);
    if (0 != done) {
      return false;
    }
    size_t size(0);
    context.uc_stack.ss_sp = __splitstack_makecontext(kStackSize, _context._stack_context, &size);
    context.uc_stack.ss_size = size;
    context.uc_stack.ss_flags = 0;
    context.uc_link = nullptr;
    //
    int block(0);
    __splitstack_block_signals_context(_context._stack_context, &block, nullptr);
    //
    auto func(reinterpret_cast<void(*)()>(Coroutine<Runner>::run));
    makecontext(&context, func, 1, &coroutine);
    //
    auto prev = current;
    _context._link = prev;
    current = &_context;
    //
    done = _setjmp(prev->_jmpbuf);
    if (0 == done) {
      __splitstack_block_signals(&block, nullptr);
      __splitstack_setcontext(_context._stack_context);
      setcontext(&context);
    }
    context_list.Append(&_context);
    return true;
  }
  //
  template<typename Runner>
  void Coroutine<Runner>::SwitchIn() {
    auto prev = current;
    current = &_context;
    _context._link = prev;
    auto done = _setjmp(prev->_jmpbuf);
    if (0 == done) {
      _longjmp(_context._jmpbuf, 1);
    }
  }
  //
  template<typename Runner>
  void Coroutine<Runner>::SwitchOut() {
    current = _context._link;
    auto done = _setjmp(_context._jmpbuf);
    if (0 == done) {
      _longjmp(current->_jmpbuf, 1);
    }
  }
  //
} // namespace NAMESPACE
