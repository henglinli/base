// -*- coding:utf-8-unix; -*-
#include "gtest/gtest.h"
#include "scheduler.hh"
//
using namespace NAMESPACE;
//
const size_t kTasks(10240);
const size_t kMaxCPU(4);
//
struct Task: public gnutm::StailQ<Task>::Node {
  Task(): _n(0), _ok(false) {}
  //
  int _n;
  bool _ok;
  bool DoWork() {
    if (not _ok) {
      printf("%d|%d ", _ok, _n);
      _ok = true;
      return false;
    }
    printf("%dr%d ", _ok, _n);
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
  auto ok = scheduler.Add(nullptr);
  EXPECT_EQ(false, ok);
  for (size_t i(0); i < kTasks; ++i) {
    t[i]._n = i;
    ok = scheduler.Add(t + i);
    EXPECT_EQ(true, ok);
  }
  scheduler.Stop();
}
//
