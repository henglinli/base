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
template<typename Task, uint32_t kMaxCPUs = Processor::MaxCPUs()>
class Scheduler {
 public:
  typedef Scheduler<Task, kMaxCPUs> Self;
  //
  Scheduler() : _worker(), _last_victim(), _worker_threads(0) {}
  //
  virtual ~Scheduler() {
    Abort();
  }
  //
  static auto Start(Self& scheduler) -> bool {
    return scheduler.StartWorker(scheduler);
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
  auto ToSelf(Task* task) -> bool;
  //
  auto ToOther(Task* task) -> bool;
  //
 protected:
  //
  template<typename, typename>
  friend class Worker;
  //
  template<Status kStatus>
  auto Signal() -> void;
  //
  auto StartWorker(Self& scheduler) -> bool;
  //
  auto Steal() -> Task*;
  //
  auto ChooseVictim() -> uint32_t;
  //
 private:
  Worker<Self, Task> _worker[kMaxCPUs];
  uint32_t _last_victim[kMaxCPUs];
  uint32_t _worker_threads;
}; // class Scheduler
//
template<typename Task, uint32_t kMaxCPUs>
template<Status kStatus>
auto Scheduler<Task, kMaxCPUs>::Signal() -> void {
  static_assert(kStatus == kStop or kStatus == kAbort, "should kStop or kAbort");
  Status status(kUndefined);
  for(size_t i(0); i < _worker_threads; ++i) {
    if (kStop != kStatus) {
      _worker[i].Abort();
    } else {
      _worker[i].Stop();
    }
    _worker[i].Join(status);
  }
}
//
template<typename Task, uint32_t kMaxCPUs>
auto Scheduler<Task, kMaxCPUs>::ToSelf(Task* task) -> bool {
  if (nullptr not_eq task) {
    auto which = Processor::Current();
    return _worker[which].Add(task);
  }
  return false;
}
//
template<typename Task, uint32_t kMaxCPUs>
auto Scheduler<Task, kMaxCPUs>::ToOther(Task* task) -> bool {
  if (nullptr not_eq task) {
    auto which = Processor::Current();
    return _worker[which].Add(task);
  }
  return false;
}
//
template<typename Task, uint32_t kMaxCPUs>
auto Scheduler<Task, kMaxCPUs>::StartWorker(Self& scheduler) -> bool {
  auto cpus = Processor::Count();
  _worker_threads = cpus < kMaxCPUs ? cpus : kMaxCPUs;
  int done(0);
  //
  for(size_t i(0); i < _worker_threads; ++i) {
    _last_victim[i] = (0x49f6428aUL + Processor::Timestap()) % _worker_threads;
    done = Worker<Self, Task>::Start(_worker[i], scheduler);
    if (0 not_eq done) {
      for(size_t j(0); j < i; ++j) {
        _worker[j].Abort();
      }
      return false;
    }
  }
  return true;
}
//
template<typename Task, uint32_t kMaxCPUs>
auto Scheduler<Task, kMaxCPUs>::Steal() -> Task* {
  auto victim = Self::ChooseVictim();
  return _worker[victim].GetTask();
}
//
template<typename Task, uint32_t kMaxCPUs>
auto Scheduler<Task, kMaxCPUs>::ChooseVictim() -> uint32_t {
  auto self = Processor::Current();
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
