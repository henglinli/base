// -*- coding:utf-8-unix; -*-
#include "gtest/gtest.h"
#include "scheduler.hh"
#include "fiber.hh"
//
using namespace NAMESPACE;
//
const size_t kTasks(200*1024);
const size_t kMaxCPU(4);
//
struct Task {
  Task(): _n(0) {}
  ~Task() = default;
  //
  int _n;
  static auto Switch(Task* task) -> void {
    task->DoWork();
  }
  //
  auto DoWork() -> void {
    printf("%d ", _n);
  }
  //
  DISALLOW_COPY_AND_ASSIGN(Task);
};
//
TEST(Scheduler, api) {
  typedef Scheduler<Task, kMaxCPU> S;
  S scheduler;
  printf("sizeof(S)=%lu\n", sizeof(scheduler));
  auto ok = S::Start(scheduler);
  EXPECT_TRUE(ok);
  Task t[kTasks+1];
  ok = scheduler.ToSelf(nullptr);
  EXPECT_FALSE(ok);
  for (auto i(kTasks); i > 0; --i) {
    t[i]._n = i;
    if(i%4) {
      ok = scheduler.ToOther(t + i);
    } else {
      ok = scheduler.ToSelf(t + i);
    }
    EXPECT_EQ(true, ok);
  }
  scheduler.Stop();
  //
}
//
struct Go {
  static size_t n;
  static auto Run(void* arg) -> void {
    auto p = static_cast<size_t*>(arg);
    auto x = atomic::AddFetch(p, 1LU);
    printf("%lu ", x);
  }
};
size_t Go::n(0);
//
TEST(Scheduler, fiber) {
  typedef Scheduler<Fiber<>, kMaxCPU> S;
  S scheduler;
  printf("sizeof(S)=%lu\n", sizeof(scheduler));
  auto ok = S::Start(scheduler);
  EXPECT_TRUE(ok);
  Fiber<>* fiber(nullptr);
  for (auto i(kTasks); i > 0; --i) {
    fiber = Fiber<>::Fork(Go::Run, &Go::n);
    EXPECT_TRUE(fiber);
    ok = scheduler.ToOther(fiber);
    EXPECT_TRUE(ok);
  }
  //
  scheduler.Stop();
}
//
