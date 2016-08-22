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
class Worker {
 public:
  Worker();
  ~Worker();
  //
  void Init(Scheduler& scheduler) {
    atomic::Store(&_status, kInit);
    _scheduler = &scheduler;
    printf("%s\n", __PRETTY_FUNCTION__);
  }
  //
  Task* GetTask() {
    return _processor.Pop();
  }
  //
  Status* Loop();
  //
  bool Add(Task* task) {
    auto status = atomic::Load(&_status);
    if (kInit == status or kReady == status) {
      _processor.Push(task);
      return true;
    }
    return false;
  }
  //
  void Stop() {
    Signal<kStop>();
  }
  //
  void Abort() {
    Signal<kAbort>();
  }
  //
 protected:
  template<Status kStatus>
  void Signal() {
    atomic::Store(&_status, kStatus);
  }
  //
 private:
  Status _status;
  Scheduler* _scheduler;
  Task* _task;
  Processor<Task> _processor;
};
//
template<typename Scheduler, typename Task>
Worker<Scheduler, Task>::Worker()
    : _status(kUndefined)
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
Status* Worker<Scheduler, Task>::Loop() {
  Status status(kInit);
  auto ok = atomic::CAS(&_status, &status, kReady);
  if (not ok) {
    return &_status;
  }
  printf("CPU %d\n", Processor<Task>::Current());
  //
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
        Thread<Worker, Status>::Yield();
        continue;
      }
    }
    //
    auto redo = _task->DoWork();
    if (redo) {
      _processor.Push(_task);
    }
  } // while
  return &_status;
}
//
} // namespace NAMESPACE
