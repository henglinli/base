// -*- coding:utf-8-unix; -*-
#pragma once
#include <stdint.h>
#include "worker.hh"
//
namespace NAMESPACE {
//
const uint32_t kDefaultMaxCPU(32);
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
  //
  typedef Scheduler<Task, kMaxCPU> Self;
  //
  Scheduler();
  //
  static bool Start(Self& scheduler, size_t threads);
  //
  void Stop();
  //
  void Add(Task* task);
  //
 protected:
  //
  void InitWorker(Self& scheduler);
  //
  bool StartWorkerThread(size_t threads);
  //
  template<typename, typename>
  friend class Worker;
  //
  Task* Steal();
  //
  uint32_t ChooseVictim(uint32_t which);
  //
 private:
  //
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
    , _worker_threads(0) {
  // nil
}
//
template<typename Task, uint32_t kMaxCPU>
bool Scheduler<Task, kMaxCPU>::Start(Self& scheduler, size_t threads) {
  // should init worker first
  scheduler.InitWorker(scheduler);
  //
  bool done(false);
  done = scheduler.StartWorkerThread(threads);
  if (not done) {
    return false;
  }
  //
  return done;
}
//
template<typename Task, uint32_t kMaxCPU>
void Scheduler<Task, kMaxCPU>::Stop() {
  Status status;
  int done(0);
  size_t i(0);
  for(i = 0; i < _worker_threads; ++i) {
    _worker[i].Stop();
  }
  //
  for(i = 0; i < _worker_threads; ++i) {
    done = _thread[i].Join(&status);
    if (0 not_eq done) {
      _thread[i].Cancel();
    }
  }
}
//
template<typename Task, uint32_t kMaxCPU>
void Scheduler<Task, kMaxCPU>::Add(Task* task) {
  if (nullptr not_eq task) {
    uint32_t which = Processor<Task>::Current();
    _worker[which].Add(task);
  }
}
//
template<typename Task, uint32_t kMaxCPU>
void Scheduler<Task, kMaxCPU>::InitWorker(Self& scheduler) {
  for(size_t i(0); i < _worker_threads; ++i) {
    _worker[i].Init(scheduler);
  }
}
//
template<typename Task, uint32_t kMaxCPU>
bool Scheduler<Task, kMaxCPU>::StartWorkerThread(size_t threads) {
  _worker_threads = threads < kMaxCPU ? threads : kMaxCPU;
  //
  int done(0);
  for(size_t i(0); i < threads; ++i) {
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
  uint32_t which = Processor<Task>::Current();
  uint32_t victim = Self::ChooseVictim(which);
  Task* task = _worker[victim].Steal();
  return task;
}
//
template<typename Task, uint32_t kMaxCPU>
uint32_t Scheduler<Task, kMaxCPU>::ChooseVictim(uint32_t which) {
  uint32_t victim = _last_victim[which];
  if (++victim == _worker_threads) {
    victim = 0;
  }
  for (; victim < _worker_threads; ++victim) {
    if (which == victim) {
      continue;
    }
    _last_victim[which] = victim;
    break;
  }
  return victim;
}
//
} // namespace NAMESPACE
