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
  //
  bool DoWork() {
    size_t sum(0);
    for (size_t i(0); sum < kTasks; ++i) {
      ++sum;
    }
    if (1 < sum) {
      return false;
    }
    return true;
  }
};
//
struct Scheduler {
  //
  Task* Steal() {
    return nullptr;
  }
};
//
TEST(Worker, api) {
  Thread<Worker<Scheduler, Task>, Status> thread;
  Worker<Scheduler, Task> worker;
  Scheduler scheduler;
  worker.Init(scheduler);
  Status s = worker.Stat();
  EXPECT_EQ(kReady, s);
  Task t[kTasks];
  for (size_t i(0); i < kTasks; ++i) {
    worker.Add(t + i);
  }
  int done = thread.Run(worker);
  EXPECT_EQ(0, done);
  sleep(1);
  worker.Stop();
  s = worker.Stat();
  EXPECT_EQ(kStop, s);
  Status status(kUnkown);
  done = thread.Join(&status);
  EXPECT_EQ(0, done);
  EXPECT_EQ(kStop, status);
}
//
