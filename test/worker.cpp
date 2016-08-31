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
  bool _ok;
  Task(): _ok(false) {}
  //
  bool DoWork() {
    size_t sum(0);
    for (size_t i(0); sum < kTasks; ++i) {
      ++sum;
    }
    if (not _ok) {
      printf("%d| ", _ok);
      _ok = true;
      return false;
    }
    printf("%dr ", _ok);
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
  Thread thread;
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
  sleep(2);
  worker.Stop();
  auto status(kUndefined);
  done = thread.Join(status);
  EXPECT_EQ(0, done);
  EXPECT_EQ(kStop, status);
}
//
