// -*- coding:utf-8-unix; -*-
#pragma once
#include <stdint.h>
#include "status.hh"
#include "processor.hh"
#include "thread.hh"
#include "atomic.hh"
//
namespace NAMESPACE {
//
template<typename Scheduler, typename Task>
class Worker: public Thread<Worker<Scheduler, Task> > {
 public:
  //
  typedef Worker<Scheduler, Task> Self;
  //
  Worker();
  //
  ~Worker();
  //
  static auto Start(Self& worker, Scheduler& scheduler) -> int {
    worker._scheduler = &scheduler;
    return Self::Run(worker);
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
  Processor<Task> _processor;
  //
  DISALLOW_COPY_AND_ASSIGN(Worker);
};
//
template<typename Scheduler, typename Task>
Worker<Scheduler, Task>::Worker()
    : _status(kInit)
    , _scheduler(nullptr)
    , _task(nullptr)
    , _processor() {}
//
template<typename Scheduler, typename Task>
Worker<Scheduler, Task>::~Worker() {
  Abort();
}
//
template<typename Scheduler, typename Task>
auto Worker<Scheduler, Task>::Loop() -> Status* {
  Status status(kInit);
  bool ok(false);
  while(true) {
    ok = atomic::CAS(&_status, &status, kReady);
    if (ok) {
      break;
    }
  }
  //
  const uint32_t spin_count(30);
  bool spin(false);
  while(true) {
    status = atomic::Load(&_status);
    if(kAbort == status) {
      break;
    }
    //
    _task = _processor.Pop();
    if (nullptr == _task) {
      // self is empty
      _task = _scheduler->Steal();
      if (nullptr == _task) {
        // other is empty
        status = atomic::Load(&_status);
        if (kStop == status) {
          break;
        }
        // try spin an then yield;
        if (spin) {
          Processor<Task>::Relax(spin_count);
        } else {
          spin = true;
          Worker<Scheduler, Task>::Yield();
        }
        continue;
      }
    }
    //
    ok = _task->DoWork();
    if (not ok) {
      _processor.Push(_task);
    }
  } // while
  return &_status;
}
//
} // namespace NAMESPACE
