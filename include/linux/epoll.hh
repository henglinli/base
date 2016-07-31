// -*-coding:utf-8-unix;-*-
//
#pragma once
//
#include <errno.h>
#ifdef __linux__
#include <sys/epoll.h>
#endif
#include "netpoll.hh"
//
namespace NAMESPACE {
//
const size_t kDefaultEvents(128);
//
class Epoll
    : public NetPoll<Epoll> {
 private:
  //
  Epoll()
      : _fd(-1) {
    // nil
  }
  //
  int init() {
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
  int open(int fd, void* ptr) {
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
  int close(int fd) {
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
  int poll(bool block) {
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
  int _fd;
};
//
} // namespace NAMESPACE
