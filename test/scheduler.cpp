// -*- coding:utf-8-unix; -*-
#include "gtest/gtest.h"
#include "scheduler.hh"
//
using namespace NAMESPACE;
//
const size_t kTasks(512);
const size_t kMaxCPU(4);
//
struct Task: public gnutm::StailQ<Task>::Node {
  bool DoWork() {
    static size_t count(0);
    printf("%d ", ++sum);
    if (++count < kTasks) {
      return true;
    } else {
      return false;
    }
  }
  static int sum;
};
int Task::sum(0);
//
TEST(Scheduler, api) {
  typedef Scheduler<Task, kMaxCPU> S;
  S scheduler;
  bool done = S::Start(scheduler, 4);
  EXPECT_EQ(true, done);
  Task t[kTasks];
  auto ok(false);
  for (size_t i(0); i < kTasks; ++i) {
    ok = scheduler.Add(t + i);
    EXPECT_EQ(true, ok);
  }
  sleep(16);
  scheduler.Stop();
}
//
