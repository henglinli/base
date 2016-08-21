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
  //
  static bool Start(Self& scheduler, size_t threads);
  //
  void Stop();
  //
  void Add(Task* task);
  //
 protected:
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
  uint32_t ChooseVictim(uint32_t which);
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
bool Scheduler<Task, kMaxCPU>::Start(Self& scheduler, size_t threads) {
  // should init worker first
  scheduler.InitWorker(scheduler, threads);
  //
  return scheduler.StartWorkerThread();
}
//
template<typename Task, uint32_t kMaxCPU>
void Scheduler<Task, kMaxCPU>::Stop() {
  for(size_t i(0); i < _worker_threads; ++i) {
    _worker[i].Stop();
  }
  //
  Status status(kUndefined);
  int done(0);
  for(size_t i(0); i < _worker_threads; ++i) {
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
    auto which = Processor<Task>::Current();
    printf("%d ", which);
    _worker[which].Add(task);
  }
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
  printf("%s\n", __PRETTY_FUNCTION__);
  return true;
}
//
template<typename Task, uint32_t kMaxCPU>
Task* Scheduler<Task, kMaxCPU>::Steal() {
  auto which = Processor<Task>::Current();
  auto victim = Self::ChooseVictim(which);
  return _worker[victim].Steal();
}
//
template<typename Task, uint32_t kMaxCPU>
uint32_t Scheduler<Task, kMaxCPU>::ChooseVictim(uint32_t which) {
  auto victim = _last_victim[which];
  victim = (++victim)%(_worker_threads);
  _last_victim[which] = victim;
  return victim;
}
//
} // namespace NAMESPACE
