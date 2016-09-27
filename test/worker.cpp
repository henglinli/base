// -*- coding:utf-8-unix; -*-
#include "gtest/gtest.h"
#include "worker.hh"
//
using namespace NAMESPACE;
//
const size_t kTasks(1024);
//
struct Task {
  int n;
  Task(): n(0) {}
  //
  static auto Switch(Task* task) -> void {
    task->DoWork();
  }
  //
  auto DoWork() -> void {
    size_t sum(0);
    for (size_t i(0); sum < kTasks; ++i) {
      ++sum;
    }
    printf("%d ", n);
  }
  //
  DISALLOW_COPY_AND_ASSIGN(Task);
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
  Worker<Scheduler, Task> worker;
  Scheduler scheduler;
  Task t[kTasks];
  auto done = Worker<Scheduler, Task>::Start(worker, scheduler);
  auto ok(false);
  for (size_t i(0); i < kTasks; ++i) {
    t[i].n = i;
    ok = worker.Add(t + i);
    EXPECT_EQ(true, ok);
  }
  //
  EXPECT_EQ(0, done);
  worker.Stop();
  auto status(kUndefined);
  done = worker.Join(status);
  EXPECT_EQ(0, done);
  EXPECT_EQ(kStop, status);
}
//
