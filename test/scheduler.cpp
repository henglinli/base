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
  Task(): n(0), ok(false) {}
  //
  int n;
  bool ok;
  bool DoWork() {
    if (not ok) {
      printf("%d|%d ", ok, n);
      ok = true;
      return true;
    }
    printf("%dr%d ", ok, n);
    return true;
  }
  //
  DISALLOW_COPY_AND_ASSIGN(Task);
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
    t[i].ok = false;
    t[i].n = i;
    ok = scheduler.Add(t + i);
    EXPECT_EQ(true, ok);
  }
  sleep(4);
  scheduler.Stop();
}
//
