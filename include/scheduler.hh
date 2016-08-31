// -*- coding:utf-8-unix; -*-
#pragma once
#include <stdint.h>
#include "worker.hh"
//
namespace NAMESPACE {
//
/*
  class _Task
  : public mpmc::Node<_Task> {
  public:
  virtual bool DoWork() = 0;
  };
*/
//
template<typename Task, uint32_t kMaxCPUs = Processor<Task>::MaxCPUs()>
class Scheduler {
 public:
  typedef Scheduler<Task, kMaxCPUs> Self;
  //
  Scheduler();
  ~Scheduler();
  //
  static bool Start(Self& scheduler);
  //
  void Stop();
  //
  void Abort();
  //
  bool ToSelf(Task* task);
  //
  bool ToOther(Task* task);
  //
 protected:
  //
  template<Status kStatus>
  void Signal();
  //
  void InitWorker(Self& scheduler);
  //
  bool StartWorkerThread();
  //
  template<typename, typename>
  friend class Worker;
  //
  Task* Steal();
  //
  uint32_t ChooseVictim();
  //
 private:
  Worker<Self, Task> _worker[kMaxCPUs];
  Thread _thread[kMaxCPUs];
  uint32_t _last_victim[kMaxCPUs];
  uint32_t _worker_threads;
}; // class Scheduler
//
template<typename Task, uint32_t kMaxCPUs>
Scheduler<Task, kMaxCPUs>::Scheduler()
    : _worker()
    , _thread()
    , _last_victim()
    , _worker_threads(0) {}
//
template<typename Task, uint32_t kMaxCPUs>
Scheduler<Task, kMaxCPUs>::~Scheduler() {
  Abort();
}
//
template<typename Task, uint32_t kMaxCPUs>
bool Scheduler<Task, kMaxCPUs>::Start(Self& scheduler) {
  // should init worker first
  scheduler.InitWorker(scheduler);
  //
  return scheduler.StartWorkerThread();
}
//
template<typename Task, uint32_t kMaxCPUs>
template<Status kStatus>
void Scheduler<Task, kMaxCPUs>::Signal() {
  static_assert(kStatus == kStop or kStatus == kAbort, "should kStop or kAbort");
  for(size_t i(0); i < _worker_threads; ++i) {
    if (kStop != kStatus) {
      _worker[i].Abort();
    } else {
      _worker[i].Stop();
    }
  }
  //
  Status status(kUndefined);
  int done(0);
  for(size_t i(0); i < _worker_threads; ++i) {
    done = _thread[i].Join(status);
    if (0 not_eq done) {
      _thread[i].Cancel();
    }
  }
}
//
template<typename Task, uint32_t kMaxCPUs>
void Scheduler<Task, kMaxCPUs>::Stop() {
  Signal<kStop>();
}
//
template<typename Task, uint32_t kMaxCPUs>
void Scheduler<Task, kMaxCPUs>::Abort() {
  Signal<kAbort>();
}
//
template<typename Task, uint32_t kMaxCPUs>
bool Scheduler<Task, kMaxCPUs>::ToSelf(Task* task) {
  if (nullptr not_eq task) {
    auto which = Processor<Task>::Current();
    return _worker[which].Add(task);
  }
  return false;
}
//
template<typename Task, uint32_t kMaxCPUs>
bool Scheduler<Task, kMaxCPUs>::ToOther(Task* task) {
  if (nullptr not_eq task) {
    auto which = Processor<Task>::Current();
    return _worker[which].Add(task);
  }
  return false;
}
//
template<typename Task, uint32_t kMaxCPUs>
void Scheduler<Task, kMaxCPUs>::InitWorker(Self& scheduler) {
  auto cpus = Processor<Task>::Count();
  _worker_threads = cpus < kMaxCPUs ? cpus : kMaxCPUs;
  for(size_t i(0); i < _worker_threads; ++i) {
    _last_victim[i] = (0x49f6428aUL + Processor<Task>::Timestap()) % _worker_threads;
    _worker[i].Init(scheduler);
  }
}
//
template<typename Task, uint32_t kMaxCPUs>
bool Scheduler<Task, kMaxCPUs>::StartWorkerThread() {
  int done(0);
  for(size_t i(0); i < _worker_threads; ++i) {
    _last_victim[i] = i;
    done = _thread[i].Run(_worker[i], i);
    // create worker thread failed should canncel all
    if (0 not_eq done) {
      for(size_t j(0); j < i; ++j) {
        _thread[j].Cancel();
      }
      return false;
    }
  }
  return true;
}
//
template<typename Task, uint32_t kMaxCPUs>
Task* Scheduler<Task, kMaxCPUs>::Steal() {
  auto victim = Self::ChooseVictim();
  return _worker[victim].GetTask();
}
//
template<typename Task, uint32_t kMaxCPUs>
uint32_t Scheduler<Task, kMaxCPUs>::ChooseVictim() {
  auto self = Processor<Task>::Current();
  auto victim = _last_victim[self];
  while(true) {
    // chose next victim
    if (victim == (_worker_threads - 1)) {
      victim = 0;
    } else {
      ++victim;
    }
    // victim not found then self is victim
    if (victim == _last_victim[self]) {
      break;
    }
    // self should not be victim
    if (victim == self) {
      continue;
    }
    // save last victim
    _last_victim[self] = victim;
    return victim;
  }
  return self;
}
//
} // namespace NAMESPACE
