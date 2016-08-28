// -*- coding:utf-8-unix; -*-
// see https://github.com/esl/seta
#pragma once
#include <stdint.h>
#include "worker.hh"
//
namespace NAMESPACE {
//
template <typename Impl, uint32_t kMaxCPUs = Processor<int>::MaxCPUs()>
class Seta {
 public:
  typedef Seta<Impl, kMaxCPUs> Self;
  //
  class Closure {
   public:
   private:
    size_t _level;
    size_t _join_connter;
    bool _is_last_thread;
  };
  //
  struct Context {
    size_t _level;
    int _processor;
    bool _is_last_thread;
  };
  //
  class Worker {

  };
  //
  static bool Start(Self& seta) {
    seta.InitWorker(seta);
    return seta.StartWorkerThread();
  }
  //
  template<typename Task>
  bool Spawn(Task* task, Context* context);
  //
  template<typename Task>
  bool Signal(Task* task, Context* context);
  //
 protected:
  void InitWorker(Self& seta) {
    auto cpus = Processor<Impl>::Count();
    _worker_threads = cpus < kMaxCPUs ? cpus : kMaxCPUs;
    _last_victim[i] = (0x49f6428aUL + Processor<Task>::Timestap()) % _worker_threads;
    _worker[i].Init(seta);
  }
  //
  bool StartWorkerThread();
  //
 private:
  Worker _worker[kMaxCPUs];
  Thread<Worker, Status> _thread[kMaxCPUs];
  uint32_t _last_victim[kMaxCPUs];
  uint32_t _worker_threads;
  //
  DISALLOW_COPY_AND_ASSIGN(Seta);
};
//
} // namespace NAMESPACE
