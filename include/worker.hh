// -*- coding:utf-8-unix; -*-
#include <stdint.h>
#include "processor.hh"
//
namespace NAMESPACE {
template<typename Task, typename Value>
class Worker {
 public:
  Value* Loop() {

  }
  //
  void Add(Task* task) {
    DoNothing(task);
  }
  //
 protected:  
  void Steal(Worker* worker) {
    Task* task = worker->Pop();
    DoNothing(task);
  }
  //
 private:
  Processor<Task> _processor;
}; // class Worker
} // namespace NAMESPACE
