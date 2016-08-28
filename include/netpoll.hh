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
  int Init() {
    return static_cast<Impl*>(this)->init();
  }
  //
  int Open(int fd, void *ptr) {
    return static_cast<Impl*>(this)->open(fd, ptr);
  }
  //
  int Close(int fd) {
    return static_cast<Impl*>(this)->close(fd);
  }
  //
  int Poll(bool block) {
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
      static int Init(NetPoll<Impl>& poll) {
    return poll.Init();
  }
  //
  template<typename Impl>
      static int Open(NetPoll<Impl>& poll, int fd, void *ptr) {
    return poll.Open(fd, ptr);
  }
  //
  template<typename Impl>
      static int Close(NetPoll<Impl>& poll, int fd) {
    return poll.Close(fd);
  }
  //
  template<typename Impl>
      static int Poll(NetPoll<Impl>& poll, bool block) {
    return poll.Poll(block);
  }
  //
private:
  DISALLOW_COPY_AND_ASSIGN(NetPoller);
};
} // namespace NAMESPACE
