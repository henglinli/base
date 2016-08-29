// -*-coding:utf-8-unix;-*-
// refactry from
// https://github.com/scylladb/seastar/blob/master/core/thread.cc
// https://github.com/qemu/qemu/blob/master/util/coroutine-ucontext.c
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
  void __splitstack_getcontext(void* context[10]);
  void __splitstack_setcontext(void* context[10]);
  void *__splitstack_makecontext(size_t, void* context[10], size_t*);
  void *__splitstack_resetcontext(void* context[10], size_t*);
  void *__splitstack_find(void*, void*, size_t*, void**, void**, void**);
  void __splitstack_block_signals(int*, int*);
  void __splitstack_block_signals_context(void* context[10], int*, int*);
}
//
namespace NAMESPACE {
  /* usage
  struct Task_: pulbic Coroutine {
    void Run() {
      // dosomething here
    }
  };
  Task_ t;
  Coroutine::Init(t);
  Coroutine::SwitchIn(t);
  */
  //
  const size_t kStackSize(SIGSTKSZ);
  //
  class Context final: public List<Context>::Node {
  public:
    Context(): _link(nullptr), _env(), _stack(nullptr), _stack_context() {}
    ~Context() = default;
    //
  private:
    Context *_link;
    jmp_buf _env;
    void *_stack;
    void *_stack_context[10];
    //
    friend class Coroutine;
    //
  public:
    thread_local static List<Context> list;
    thread_local static List<Context> free_list;
    thread_local static Context context0;
    thread_local static Context *current;
    //
    DISALLOW_COPY_AND_ASSIGN(Context);
  };
  //
  thread_local List<Context> Context::list;
  thread_local List<Context> Context::free_list;
  thread_local Context Context::context0;
  thread_local Context* Context::current(nullptr);
  //
  namespace {
    struct Init {
      Init() {
        Context::current = &Context::context0;
      }
    };
    static Init init;
  }
  //
  class Coroutine {
  public:
    Coroutine(): _context() {}
    ~Coroutine() = default;
    //
    template<typename Runner>
    static bool Init(Runner& runner) {
      static_assert(__is_base_of(Coroutine, Runner), "Runner must inherit from Coroutine");
      return runner.init(runner);
    }
    //
    template<typename Runner>
    static void SwitchIn(Runner& runner) {
      runner.SwitchIn();
    }
    //
    [[gnu::no_split_stack]] void SwitchIn();
    //
    [[gnu::no_split_stack]] void SwitchOut();
    //
  protected:
    template<typename Runner>
    [[gnu::no_split_stack]] bool init(Runner& runner);
    //
    template<typename Runner>
    [[gnu::no_split_stack]] static void Run(Runner* runner, jmp_buf* env);
    //
    [[gnu::no_split_stack]] static void Switch(Context* from, Context* to);
    //
  private:
    Context _context;
    //
    DISALLOW_COPY_AND_ASSIGN(Coroutine);
  };
  //
  template<typename Runner>
  void Coroutine::Run(Runner* runner, jmp_buf* env) {
    auto self = &(runner->_context);
    auto done = _setjmp(self->_env);
    if (0 == done) {
      _longjmp(*env, 1);
    }
    while (true) {
      runner->Run();
      runner->SwitchOut();
    }
  }
  //
  template<typename Runner>
  bool Coroutine::init(Runner& runner) {
    ucontext_t context;
    auto done = getcontext(&context);
    if (0 not_eq done) {
      return false;
    }
    size_t size(0);
    _context._stack = __splitstack_makecontext(kStackSize, _context._stack_context, &size);
    context.uc_stack.ss_sp = _context._stack;
    context.uc_stack.ss_size = size;
    context.uc_stack.ss_flags = 0;
    context.uc_link = nullptr;
    //
    int block(0);
    __splitstack_block_signals_context(_context._stack_context, &block, nullptr);
    //
    jmp_buf env;
    auto func(reinterpret_cast<void(*)()>(Coroutine::Run<Runner>));
    makecontext(&context, func, 2, &runner, &env);
    //
    done = _setjmp(env);
    if (0 == done) {
      __splitstack_block_signals(&block, nullptr);
      __splitstack_setcontext(_context._stack_context);
      setcontext(&context);
    }
    //
    Context::list.Push(&_context);
    //
    return true;
  }
  //
  void Coroutine::SwitchIn() {
    auto from = Context::current;
    _context._link = Context::current;
    Context::current = &_context;
    Switch(from, &_context);
  }
  //
  void Coroutine::SwitchOut() {
    Context::current = _context._link;
    Switch(&_context, Context::current);
  }
  //
  void Coroutine::Switch(Context* from, Context* to) {
    auto done = _setjmp(from->_env);
    if (0 == done) {
      _longjmp(to->_env, 1);
    }
  }
} // namespace NAMESPACE
