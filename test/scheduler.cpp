// -*- coding:utf-8-unix; -*-
#include "gtest/gtest.h"
#include "scheduler.hh"
//
using namespace NAMESPACE;
//
const size_t kTasks(32);
const size_t kMaxCPU(4);
//
struct Task: public gnutm::StailQ<Task>::Node {
  int n;
  int retry;
  bool DoWork() {
    printf("%d ", n);
    if (retry) {
      retry = 0;
      return false;
    }
    return false;
  }
};
//
TEST(Scheduler, api) {
  typedef Scheduler<Task, kMaxCPU> S;
  S scheduler;
  bool done = S::Start(scheduler, 4);
  EXPECT_EQ(true, done);
  Task t[kTasks];
  auto ok(false);
  for (size_t i(0); i < kTasks; ++i) {
    t[i].retry = 1;
    t[i].n = i;
    ok = scheduler.Add(t + i);
    EXPECT_EQ(true, ok);
  }
  sleep(4);
  scheduler.Stop();
}
//
