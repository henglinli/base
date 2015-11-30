// -*- coding:utf-8-unix; -*-
#include "gtest/gtest.h"
#include "worker.hh"
//
using namespace NAMESPACE;
//
const size_t kTasks(1024);
//
struct Task
    : public mpmc::Node<Task> {
  int _value;
  //
  bool DoWork() {
    size_t sum(0);
    for (size_t i(0); sum < kTasks; ++i) {
      ++sum;
    }
    if (1 < sum) {
      return true;
    }
    return false;
  }
};
//
struct Scheduler {
  typedef int Value;
  //
  Task* Steal() {
    return nullptr;
  }
};
//
TEST(Worker, api) {
  Thread<Worker<Scheduler, Task>, Scheduler::Value> thread;
  Worker<Scheduler, Task> worker;
  Scheduler scheduler;
  worker.Init(scheduler);
  Task t[kTasks];
  for (size_t i(0); i < kTasks; ++i) {
    worker.Add(t + i);
  }
  int done = thread.Run(worker);
  EXPECT_EQ(0, done);
  worker.Stop();
  Scheduler::Value value(-1);
  done = thread.Join(&value);
  EXPECT_EQ(0, done);
  EXPECT_EQ(1, value);
}
//
