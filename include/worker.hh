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
  Worker();
  //
  void Init(Scheduler& scheduler) {
    _status = kReady;
    _scheduler = &scheduler;
    printf("%s\n", __PRETTY_FUNCTION__);
  }
  //
  Task* Steal() {
    return _processor.Pop();
  }
  //
  Status* Loop();
  //
  void Add(Task* task) {
    if (kInit == _status) {
      _processor.Push(task);
    }
  }
  //
  void Stop() {
    _status = kStop;
  }
  //
 protected:
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
Status* Worker<Scheduler, Task>::Loop() {
  if (kReady not_eq _status) {
    _status = kInit;
    return &_status;
  }
  printf("CPU %d\n", Processor<Task>::Current());
  //
  while(true) {
    if(kAbort == _status) {
      break;
    }
    //
    _task = _processor.Pop();
    if (nullptr == _task) {
      _task = _scheduler->Steal();
      if (nullptr == _task) {
        if (kStop == _status) {
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
