// -*- coding:utf-8-unix; -*-
#pragma once
#include <stdint.h>
#include "mpmc/queue.hh"
#include "processor.hh"
#include "thread.hh"
//
namespace NAMESPACE {
//
const uint32_t kDefaultMaxCPU(32);
//
class _Task
    : public mpmc::Node<_Task> {
 public:
  virtual bool DoWork() = 0;
};
//
template<typename Task, uint32_t kMaxCPU = kDefaultMaxCPU>
class Scheduler {
 public:
  //
  typedef Scheduler<Task, kMaxCPU> Self;
  typedef int Value;
  //
  static bool Start(Self& scheduler, size_t threads) {
    bool done(false);
    done = scheduler.StartWorkerThread(threads);
    if (not done) {
      return false;
    }
    //
    scheduler.InitWorker(&scheduler);
    return done;
  }
  //
  void Stop() {
    Value tmp;
    int done(0);
    size_t i(0);
    for(i = 0; i < _worker_threads; ++i) {
      _worker[i].Signal(kStop);
    }
    for(i = 0; i < _worker_threads; ++i) {
      done = _thread[i].Join(&tmp);
      if (0 not_eq done) {
        _thread[i].Cancel();
      }
    }
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
  enum Status {
    kInit = 0,
    kStop = 1,
    kAbort = 2
  };
  //
  void InitWorker(Self* scheduler) {
    for(size_t i(0); i < _worker_threads; ++i) {
      _worker[i].Init(scheduler);
    }
  }
  //
  bool StartWorkerThread(size_t threads) {
    _worker_threads = threads < kMaxCPU ? threads : kMaxCPU;
    //
    int done(0);
    for(size_t i(0); i < threads; ++i) {
      _last_victim[i] = i;
      done = _thread[i].Run(_worker + i);
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
  friend class Worker;
  Task* Steal() {
    uint32_t which = Processor<Task>::Current();
    uint32_t victim = Self::ChooseVictim(which);
    Task* task = _worker[victim].Steal();
    return task;
  }
  //
  uint32_t ChooseVictim(uint32_t which) {
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
 private:
  //
  class Worker {
   public:
    //
    inline void Init(Self* scheduler) {
      _status = kInit;
      _scheduler = scheduler;
    }
    //
    inline Task* Steal() {
      return _processor.Pop();
    }
    //
    Value* Loop() {
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
   protected:
    //
   private:
    Self* _scheduler;
    Processor<Task> _processor;
    Value _status;
  };
  //
  Worker _worker[kMaxCPU];
  Thread<Worker, Value> _thread[kMaxCPU];
  uint32_t _last_victim[kMaxCPU];
  uint32_t _worker_threads;
}; // class Scheduler
//
} // namespace NAMESPACE
