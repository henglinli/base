// -*- coding:utf-8-unix; -*-
#pragma once
#include <stdint.h>
#include "mpmc/queue.hh"
#include "processor.hh"
#include "thread.hh"
//
namespace NAMESPACE {
//
const size_t kDefaultMaxCPU(32);
//
class _Task
    : public mpmc::Node<_Task> {
 public:
  virtual bool DoWork() = 0;
};
//
template<typename Task, size_t kMaxCPU = kDefaultMaxCPU>
class Scheduler {
 public:
  //
  typedef Scheduler<Task, kMaxCPU> Self;
  typedef int Value;
  //
  bool Start(size_t threads) {
    for(size_t i(0); i < threads; ++i) {
      _worker[i].Init(this);
    }
    return false;
  }
  //
  void Stop() {

  }
  //
  void Add(Task* task) {
    if (nullptr not_eq task) {
      uint32_t which = Processor<Task>::Current();
      _worker[which].Add(task);
    }
  }
  //
 protected:
  //
  friend class Worker;
  Task* Steal() {
    return nullptr;
  }
  //
 private:
  //
  class Worker {
   public:
    //
    inline void Init(Self* scheduler) {
      _scheduler = scheduler;
    }
    //
    Value* Loop() {
      while(true) {
        Task* task = _processor.Pop();
        if (nullptr == task) {
          task = _scheduler->Steal();
        }
      } // while
    }
    //
    inline void Add(Task* task) {
      _processor.Push(task);
    }
    //
   protected:
    //
   private:
    Self* _scheduler;
    Processor<Task> _processor;
  };
  //
  Worker _worker[kMaxCPU];
  Thread<Task, Value> _threads[kMaxCPU];
}; // class Scheduler
//
} // namespace NAMESPACE
