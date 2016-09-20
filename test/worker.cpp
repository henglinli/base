// -*- coding:utf-8-unix; -*-
#include "gtest/gtest.h"
#include "worker.hh"
//
using namespace NAMESPACE;
//
const size_t kTasks(1024);
//
struct Task {
  bool _ok;
  int n;
  Task(): _ok(false), n(0) {}
  //
  auto DoWork() -> bool {
    size_t sum(0);
    for (size_t i(0); sum < kTasks; ++i) {
      ++sum;
    }
    if (not _ok) {
      printf("%d| ", _ok);
      _ok = true;
      return false;
    }
    printf("%dr%d ", _ok, n);
    return true;
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
  auto ok(false);
  for (size_t i(0); i < kTasks; ++i) {
    t[i].n = i;
    ok = worker.Add(t + i);
    EXPECT_EQ(true, ok);
  }
  auto done = Worker<Scheduler, Task>::Start(worker, scheduler);
  EXPECT_EQ(0, done);
  //sleep(1);
  worker.Stop();
  auto status(kUndefined);
  done = worker.Join(status);
  EXPECT_EQ(0, done);
  EXPECT_EQ(kStop, status);
}
//
