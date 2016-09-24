// -*-coding:utf-8-unix;-*-
#pragma once
//
#include <linux/futex.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include "atomic.hh"
//
namespace NAMESPACE {
namespace Linux {
struct Futex {
  //
  static auto Wait(int* addr, int val) -> int {
    return syscall(SYS_futex, addr, FUTEX_WAIT bitor FUTEX_PRIVATE_FLAG,
                   val, nullptr, nullptr, 0);
  }
  //
  static auto Wake(int* addr, int val) -> int {
    return syscall(SYS_futex, addr, FUTEX_WAKE bitor FUTEX_PRIVATE_FLAG,
                   val, nullptr, nullptr, 0);
  }
  //
};
} // namespace Linux
} // namespace NAMESPACE
