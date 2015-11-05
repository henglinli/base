// -*-coding:utf-8-unix;-*-
#pragma once
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/timerfd.h>
#include <unistd.h> // for close

namespace base {
namespace internal {
const size_t kDefaultEventSize(128); // golang use 128 as default
}
//
class EventCallback {
  virtual void OnRead(int fd);
  virtual void OnWrite(int fd);
  virtual void OnError(int fd);
};
//
template<typename EventCallback,
         size_t kEventSize=internal::kDefaultEventSize>
class Multiplexer {
 public:
  //
  Multiplexer() = default;
  //
  virtual ~Multiplexer() {
    if(0 < epollfd_) {
      close(epollfd_);
    }
  }
  //
  bool Init() {
    epollfd_ = epoll_create1(EPOLL_CLOEXEC);
    if (0 < epollfd_) {
      return true;
    }
    epollfd_ = epoll_create(1024); // golang use 1024 as default
    if (0 < epollfd_) {
      return true;
    }
    return false;
  }
  //
  bool Add(int fd, uint32_t events) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.fd = fd;
    if (-1 < epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev)) {
      return true;
    }
    return false;
  }
  //
  bool AddRead(int fd) {
    return Add(fd, EPOLLIN);
  }
  //
  bool AddWrite(int fd) {
    return Add(fd, EPOLLOUT);
  }
  //
  bool AddReadWrite(int fd) {
    return Add(fd, EPOLLIN | EPOLLOUT);
  }
  //
  bool Del(int fd) {
    struct epoll_event ev;
    if (0 <= epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &ev)) {
      return true;
    }
    return false;
  }
  // wait for envent for milliseconds
  int Wait(int timeout) {
    auto nfds = epoll_wait(epollfd_, events_, kEventSize, timeout);
    if (0 >= nfds) {
      return 0;
    }
    auto done(0);
    for(auto nfd(0); nfd < nfds; ++nfd) {
      auto fd = events_[nfd].data.fd;
      auto events = events_[nfd].events;
      switch (events) {
        case EPOLLIN: {
          static_cast<EventCallback *>(this)->OnRead(fd);
          ++done;
          break;
        }
        case EPOLLOUT: {
          static_cast<EventCallback *>(this)->OnWrite(fd);
          ++done;
          break;
        }
        default: {
          static_cast<EventCallback *>(this)->OnError(fd);
          ++done;
          break;
        }
      }
    }
    return done;
  }
  //
 private:
  int epollfd_;
  struct epoll_event events_[kEventSize];
};  // class Multiplexer
//
class TimerCallback {
  virtual void OnTimeout();
};
//
template<typename TimerCallback>
class Timer
    : public Multiplexer<Timer<TimerCallback> > {
 public:
  //
  using Base = Multiplexer<Timer<TimerCallback> >;
  //
  Timer() = default;
  //
  virtual ~Timer() {
    if(0 < timerfd_) {
      Base::Del(timerfd_);
      close(timerfd_);
    }
  }
  //
  inline bool Init() {
    if (Base::Init()) {
      timerfd_ = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK|TFD_CLOEXEC);
      if(-1 < timerfd_) {
        if(Base::AddRead(timerfd_)) {
          return true;
        }
      }
    }
    return false;
  }
  //
  bool NextTimeout(size_t timeout) {
    struct itimerspec time;
    time.it_value.tv_sec=0;
    time.it_value.tv_nsec=static_cast<long>(timeout*1000000);
    if(-1 < timerfd_settime(timerfd_, 0, &time, NULL)) {
      return true;
    }
    return false;
  }
  // Multiplexer callback
  void OnRead(int fd) {
    struct itimerspec time;
    if(-1 < timerfd_gettime(fd, &time)) {
      static_cast<TimerCallback *>(this)->OnTimeout();
    }
  }
  //
  void OnWrite(int /*fd*/) {
  }
  //
  void OnError(int /*fd*/) {
  }
 private:
  int timerfd_;
}; // class Timer
//
class UserEventCallback {
  virtual void OnEvent(eventfd_t event);
};
//
template<typename UserEventCallback>
class UserEvent
    : public Multiplexer<UserEvent<UserEventCallback> > {
 public:
  //
  using Base = Multiplexer<UserEvent<UserEventCallback> >;
  //
  UserEvent() = default;
  virtual ~UserEvent() {
    if(0 < eventfd_) {
      Base::Del(eventfd_);
      close(eventfd_);
    }
  }
  //
  bool Init() {
    if (Base::Init()) {
      eventfd_ = eventfd(0, EFD_CLOEXEC|EFD_NONBLOCK);
      if(-1 < eventfd_) {
        if(Base::AddRead(eventfd_)) {
          return true;
        }
      }
    }
    return false;
  }
  //
  inline bool Notify(eventfd_t event) {
    if(-1 < eventfd_write(eventfd_, event)) {
      return true;
    }
    return false;
  }
  // Multiplexer callback
  void OnRead(int fd) {
    eventfd_t event;
    if(-1 < eventfd_read(fd, &event)) {
      static_cast<UserEventCallback *>(this)->OnEvent(event);
    }
  }
  //
  void OnWrite(int /*fd*/) {
  }
  //
  void OnError(int /*fd*/) {
  }
 private:
  int eventfd_;
};
//
} // namespace base
