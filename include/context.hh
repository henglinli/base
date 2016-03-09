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
//
template<typename Task>
class Context {
 public:
 protected:
 private:
  template<typename, size_t>
  friend class Routine;
  jmp_buf _jmpbuf;
};
//
} // namespace NAMESPACE
