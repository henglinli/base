// -*- coding:utf-8-unix; -*-
#pragma once
#include <stdint.h>
#include "worker.hh"
//
namespace NAMESPACE {
//
const uint32_t kDefaultMaxCPU(512);
//
/*
class _Task
    : public mpmc::Node<_Task> {
 public:
  virtual bool DoWork() = 0;
};
*/
//
template<typename Task, uint32_t kMaxCPU = kDefaultMaxCPU>
class Scheduler {
 public:
  typedef Scheduler<Task, kMaxCPU> Self;
  //
  Scheduler();
  ~Scheduler();
  //
  static bool Start(Self& scheduler, size_t threads);
  //
  void Stop();
  //
  void Abort();
  //
  bool Add(Task* task);
  //
 protected:
  //
  template<Status kStatus>
  void StopOrAbort();
  //
  void InitWorker(Self& scheduler, size_t threads);
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
  Worker<Self, Task> _worker[kMaxCPU];
  Thread<Worker<Self, Task>, Status> _thread[kMaxCPU];
  uint32_t _last_victim[kMaxCPU];
  uint32_t _worker_threads;
}; // class Scheduler
//
template<typename Task, uint32_t kMaxCPU>
Scheduler<Task, kMaxCPU>::Scheduler()
    : _worker()
    , _thread()
    , _last_victim()
    , _worker_threads(0) {}
//
template<typename Task, uint32_t kMaxCPU>
Scheduler<Task, kMaxCPU>::~Scheduler() {
  Abort();
}
//
template<typename Task, uint32_t kMaxCPU>
bool Scheduler<Task, kMaxCPU>::Start(Self& scheduler, size_t threads) {
  // should init worker first
  scheduler.InitWorker(scheduler, threads);
  //
  return scheduler.StartWorkerThread();
}
//
  template<typename Task, uint32_t kMaxCPU>
  template<Status kStatus>
  void Scheduler<Task, kMaxCPU>::StopOrAbort() {
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
template<typename Task, uint32_t kMaxCPU>
void Scheduler<Task, kMaxCPU>::Stop() {
  StopOrAbort<kStop>();
}
  //
  template<typename Task, uint32_t kMaxCPU>
  void Scheduler<Task, kMaxCPU>::Abort() {
    StopOrAbort<kAbort>();
  }
//
template<typename Task, uint32_t kMaxCPU>
bool Scheduler<Task, kMaxCPU>::Add(Task* task) {
  if (nullptr not_eq task) {
    auto which = Processor<Task>::Current();
    return _worker[which].Add(task);
  }
  return false;
}
//
template<typename Task, uint32_t kMaxCPU>
void Scheduler<Task, kMaxCPU>::InitWorker(Self& scheduler, size_t threads) {
  _worker_threads = threads < kMaxCPU ? threads: kMaxCPU;
  for(size_t i(0); i < _worker_threads; ++i) {
    _worker[i].Init(scheduler);
  }
}
//
template<typename Task, uint32_t kMaxCPU>
bool Scheduler<Task, kMaxCPU>::StartWorkerThread() {
  int done(0);
  for(size_t i(0); i < _worker_threads; ++i) {
    _last_victim[i] = i;
    done = _thread[i].Run(_worker[i]);
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
template<typename Task, uint32_t kMaxCPU>
Task* Scheduler<Task, kMaxCPU>::Steal() {
  auto victim = Self::ChooseVictim();
  return _worker[victim].GetTask();
}
//
template<typename Task, uint32_t kMaxCPU>
uint32_t Scheduler<Task, kMaxCPU>::ChooseVictim() {
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
