// -*- coding:utf-8-unix; -*-
#include "gtest/gtest.h"
#include "scheduler.hh"
//
using namespace NAMESPACE;
//
const size_t kTasks(102400);
const size_t kMaxCPU(4);
//
struct Task: public gnutm::StailQ<Task>::Node {
  Task(): _n(0), _ok(false) {}
  ~Task() = default;
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
  bool done = S::Start(scheduler);
  EXPECT_EQ(true, done);
  Task t[kTasks];
  auto ok = scheduler.ToSelf(nullptr);
  EXPECT_EQ(false, ok);
  for (size_t i(0); i < kTasks; ++i) {
    t[i]._n = i;
    if(i%4) {
      ok = scheduler.ToOther(t + i);
    } else {
      ok = scheduler.ToSelf(t + i);
    }
    EXPECT_EQ(true, ok);
  }
  scheduler.Stop();
}
//
