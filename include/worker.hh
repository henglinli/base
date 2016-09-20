// -*- coding:utf-8-unix; -*-
#pragma once
#include <stdint.h>
#include "status.hh"
#include "processor.hh"
#include "thread.hh"
#include "atomic.hh"
#include "mpmc/queue.hh"
//
namespace NAMESPACE {
//
template<typename Scheduler, typename Task>
class Worker: public Thread<Worker<Scheduler, Task> > {
 public:
  //
  typedef Worker<Scheduler, Task> Self;
  typedef Thread<Self> Base;
  //
  Worker();
  //
  ~Worker();
  //
  static auto Start(Self& worker, Scheduler& scheduler) -> int {
    worker._scheduler = &scheduler;
    auto done = Self::Run(worker);
    atomic::Store(&(worker._status), kReady);
    return done;
  }
  //
  inline auto GetTask() -> Task* {
    return _processor.Pop();
  }
  //
  auto Loop() -> Status*;
  //
  auto Add(Task* task) -> bool {
    auto status = atomic::Load(&_status);
    if (kInit == status or kReady == status) {
      _processor.Push(task);
      return true;
    }
    return false;
  }
  //
  inline auto Stop() -> void {
    Signal<kStop>();
  }
  //
  inline auto Abort() -> void {
    Signal<kAbort>();
  }
  //
 protected:
  template<Status kStatus>
  auto Signal() -> void {
    static_assert(kStatus == kStop or kStatus == kAbort, "should kStop or kAbort");
    atomic::Store(&_status, kStatus);
  }
  //
 private:
  Status _status;
  Scheduler* _scheduler;
  Task* _task;
  mpmc::BoundedQ<Task, 1<<16 > _processor;
  bool _spin;
  //
  DISALLOW_COPY_AND_ASSIGN(Worker);
};
//
template<typename Scheduler, typename Task>
Worker<Scheduler, Task>::Worker()
    : _status(kInit)
    , _scheduler(nullptr)
    , _task(nullptr)
    , _processor()
    , _spin(true) {}
//
template<typename Scheduler, typename Task>
Worker<Scheduler, Task>::~Worker() {
  Abort();
}
//
template<typename Scheduler, typename Task>
auto Worker<Scheduler, Task>::Loop() -> Status* {
  const uint32_t spin_count(30);
  //
  while(true) {
    if(kAbort == atomic::Load(&_status)) {
      break;
    }
    //
    _task = _processor.Pop();
    if (nullptr == _task) {
      // self is empty
      _task = _scheduler->Steal();
      if (nullptr == _task) {
        // other is empty
        if (kStop == atomic::Load(&_status)) {
          break;
        }
        // try spin an then yield;
        if (_spin) {
          Processor::Relax(spin_count);
          _spin = false;
        } else {
          _spin = true;
          Base::Yield();
        }
        continue;
      }
    }
    //
    if (not _task->DoWork()) {
      if (kStop != atomic::Load(&_status)) {
        _processor.Push(_task);
      }
    }
  } // while
  return &_status;
}
//
} // namespace NAMESPACE
