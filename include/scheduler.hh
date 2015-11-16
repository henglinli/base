// -*- coding:utf-8-unix; -*-
#include <stdint.h>
#include "processor.hh"
#include "thread.hh"
//
namespace NAMESPACE {
//
const size_t kDefaultMaxCPU(32);
//
template<typename Task, size_t kMaxCPU = kDefaultMaxCPU>
class Scheduler {
 public:
  bool Start(size_t threads) {
    DoNothing(threads);
    return false;
  }
  //
  void Stop() {

  }
  //
  void Add(Task* task) {
    DoNothing(task);
  }
  //
 protected:
  Task* Steal() {
    return nullptr;
  }
 private:
}; // class Scheduler
//
} // namespace NAMESPACE
