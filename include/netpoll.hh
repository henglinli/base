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
  NetPoll() = default;
  ~NetPoll() = default;
  //
  auto Init() -> int {
    return static_cast<Impl*>(this)->init();
  }
  //
  auto Open(int fd, void *ptr) -> int {
    return static_cast<Impl*>(this)->open(fd, ptr);
  }
  //
  auto Close(int fd) -> int {
    return static_cast<Impl*>(this)->close(fd);
  }
  //
  auto Poll(bool block) -> int {
    return static_cast<Impl*>(this)->poll(block);
  }
  //
 private:
  friend class NetPoller;
  //
  DISALLOW_COPY_AND_ASSIGN(NetPoll);
};
//
class NetPoller final {
public:
  NetPoller() = default;
  ~NetPoller() = default;
  //
  template<typename Impl>
  static auto Init(NetPoll<Impl>& poll) -> int {
    return poll.Init();
  }
  //
  template<typename Impl>
  static auto Open(NetPoll<Impl>& poll, int fd, void *ptr) -> int {
    return poll.Open(fd, ptr);
  }
  //
  template<typename Impl>
  static auto Close(NetPoll<Impl>& poll, int fd) -> int {
    return poll.Close(fd);
  }
  //
  template<typename Impl>
  static auto Poll(NetPoll<Impl>& poll, bool block) -> int {
    return poll.Poll(block);
  }
  //
private:
  DISALLOW_COPY_AND_ASSIGN(NetPoller);
};
} // namespace NAMESPACE
