// -*-coding:utf-8-unix;-*-
#pragma once
//
#include <errno.h>
#include <unistd.h>
#include <sys/epoll.h>
#include "netpoll.hh"
//
namespace NAMESPACE {
//
namespace Linux {
//
const size_t kDefaultEvents(128);
//
class Epoll final: public NetPoll<Epoll> {
public:
  Epoll(): _fd(-1) {}
  //
  ~Epoll() {
    if (-1 not_eq _fd) {
      close(_fd);
    }
  }
  //
protected:
  auto init() -> int {
    if (0 < _fd) {
      return _fd;
    }
    _fd = epoll_create1(EPOLL_CLOEXEC);
    if (0 < _fd) {
      return _fd;
    }
    return -errno;
  }
  //
  auto open(int fd, void *ptr) -> int {
    if (-1 == _fd) {
      return -1;
    }
    struct epoll_event ev;
    ev.events = EPOLLIN bitor EPOLLOUT bitor EPOLLRDHUP bitor EPOLLET;
    ev.data.ptr = ptr;
    int r = epoll_ctl(_fd, EPOLL_CTL_ADD, fd, &ev);
    if (0 <= r) {
      return r;
    }
    return -errno;
  }
  //
  auto close(int fd) -> int {
    if (-1 == _fd) {
      return -1;
    }
    struct epoll_event ev;
    int r = epoll_ctl(_fd, EPOLL_CTL_DEL, fd, &ev);
    if (0 <= r) {
      return r;
    }
    return -errno;
  }
  //
  auto poll(bool block) -> int {
    if (-1 == _fd) {
      return -1;
    }
    struct epoll_event events[kDefaultEvents], *ev;
    int n, i, timeout, mode;
    //
    timeout = -1;
    if (not block) {
      timeout = 0;
    }
 retry:
    n = epoll_wait(_fd, events, sizeof(events)/sizeof(events[0]), timeout);
    if (0 > n) {
      if (EINTR not_eq errno) {
        return -errno;
      }
      goto retry;
    }
    //
    for (i = 0; i < n; ++i) {
      ev = &events[i];
      if (0 == ev->events) {
        continue;
      }
      mode = 0;
      if (ev->events bitand (EPOLLIN bitor EPOLLRDHUP bitor EPOLLHUP bitor EPOLLERR)) {
        mode += 'r';
      }
      if (ev->events bitand (EPOLLOUT bitor EPOLLHUP bitor EPOLLERR)) {
        mode += 'w';
      }
      if (mode) {

      }
    }
    //
    return 0;
  }
  //
private:
  int _fd;
  //
  friend class NetPoll<Epoll>;
  //
  DISALLOW_COPY_AND_ASSIGN(Epoll);
};
//
} // namespace Linux
//
} // namespace NAMESPACE
