// -*- coding:utf-8-unix; -*-
#include "gtest/gtest.h"
#include "scheduler.hh"
//
using namespace NAMESPACE;
//
const size_t kTasks(1024);
const size_t kMaxCPU(4);
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
TEST(Scheduler, api) {
  typedef Scheduler<Task, kMaxCPU> S;
  S scheduler;
  bool done = S::Start(scheduler, 4);
  EXPECT_EQ(true, done);
  Task t[kTasks];
  for (size_t i(0); i < kTasks; ++i) {
    scheduler.Add(t + i);
  }
  scheduler.Stop();
}
//
