// -*- coding:utf-8-unix; -*-
#pragma once
#include <stdint.h>
#include "processor.hh"
#include "thread.hh"
//
namespace NAMESPACE {
//
template<typename Scheduler, typename Task>
class Worker {
 public:
  //
  typedef typename Scheduler::Value Value;
  //
  enum Status {
    kInit = 0,
    kStop = 1,
    kAbort = 2
  };
  //
  inline void Init(Scheduler& scheduler) {
    _status = kInit;
    _scheduler = &scheduler;
  }
  //
  inline Task* Steal() {
    return _processor.Pop();
  }
  //
  Value* Loop(); 
  //
  inline void Add(Task* task) {
    if (kInit == _status) {
      _processor.Push(task);
    }
  }
  //
  inline void Signal(int status) {
    _status = status;
  }
  //
  inline void Stop() {
    _status = kStop;
  }
  //
  inline void Abort() {
    _status = kAbort;
  }
  //
 protected:
  //
 private:
  Scheduler* _scheduler;
  Processor<Task> _processor;
  Value _status;
};
//
template <typename Scheduler, typename Task>
typename Worker<Scheduler, Task>::Value* Worker<Scheduler, Task>::Loop() {
  while(true) {
    if(kAbort == _status) {
      return &_status;
    }
    Task* task = _processor.Pop();
    if (nullptr == task) {
      task = _scheduler->Steal();
      if (nullptr == task) {
        if (kStop == _status) {
          return &_status;
        }
        Thread<Worker, Value>::Yield();
        continue;
      }
      //
      bool redo = task->DoWork();
      if (redo) {
        _processor.Push(task);
      }
    }
  } // while
  return nullptr;
}
//
} // namespace NAMESPACE
