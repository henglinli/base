// -*- coding:utf-8-unix; -*-
#pragma once
#include <stdint.h>
#include "status.hh"
#include "processor.hh"
#include "thread.hh"
//
namespace NAMESPACE {
//
template<typename Scheduler, typename Task>
class Worker {
 public:
  //
  Worker();
  //
  inline void Init(Scheduler& scheduler) {
    _status = kReady;
    _scheduler = &scheduler;
  }
  //
  inline Task* Steal() {
    return _processor.Pop();
  }
  //
  Status* Loop();
  //
  inline void Add(Task* task) {
    if (kInit == _status) {
      _processor.Push(task);
    }
  }
  //
  inline void Stop() {
    _status = kStop;
  }
  //
 protected:
  //
 private:
  Status _status;
  Scheduler* _scheduler;
  Processor<Task> _processor;
};
//
template<typename Scheduler, typename Task>
Worker<Scheduler, Task>::Worker()
    : _status(kUnkown)
    , _scheduler(nullptr)
    , _processor() {
  //
}
//
template<typename Scheduler, typename Task>
Status* Worker<Scheduler, Task>::Loop() {
  if (kReady not_eq _status) {
    _status = kInit;
    return &_status;
  }
  //
  while(true) {
    if(kAbort == _status) {
      break;
    }
    //
    Task* task = _processor.Pop();
    if (nullptr == task) {
      task = _scheduler->Steal();
      if (nullptr == task) {
        if (kStop == _status) {
          break;
        }
        Thread<Worker, Status>::Yield();
        continue;
      }
      //
      bool redo = task->DoWork();
      if (redo) {
        _processor.Push(task);
      }
    }
  } // while
  return &_status;
}
//
} // namespace NAMESPACE
