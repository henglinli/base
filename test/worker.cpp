// -*- coding:utf-8-unix; -*-
#include "gtest/gtest.h"
#include "worker.hh"
#include "gnutm/queue.hh"
//
using namespace NAMESPACE;
//
const size_t kTasks(1024);
//
struct Task: public gnutm::StailQ<Task>::Node {
  //
  Task(){}
  ~Task(){}
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
  Task t[kTasks];
  auto ok(false);
  for (size_t i(0); i < kTasks; ++i) {
    ok = worker.Add(t + i);
    EXPECT_EQ(true, ok);
  }
  auto done = thread.Run(worker);
  EXPECT_EQ(0, done);
  sleep(1);
  worker.Stop();
  auto status(kUndefined);
  done = thread.Join(status);
  EXPECT_EQ(0, done);
  EXPECT_EQ(kStop, status);
}
//
