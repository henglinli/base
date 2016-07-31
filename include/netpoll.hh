// -*-coding:utf-8-unix;-*-
//
#pragma once
//
#include "macros.hh"
//
namespace NAMESPACE {
//
template<typename Impl>
class NetPoll {
 public:
  //
  int Init() {
    return static_cast<Impl*>(this)->init();
  }
  //
  int Open(int fd, void* ptr) {
    return static_cast<Impl*>(this)->open(fd, ptr);
  }
  //
  int Close(int fd) {
    return static_cast<Impl*>(this)->close(fd);
  }
  //
  int Poll(bool block);
};
//
} // namespace NAMESPACE
