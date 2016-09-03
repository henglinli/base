// -*-coding:utf-8-unix;-*-
#include "gtest/gtest.h"
#include "thread.hh"
//
using namespace NAMESPACE;
//
template<typename Value>
class Task: public Thread<Task<Value> > {
 public:
  explicit Task(Value value)
      : _value(value) {
  }
  //
  auto Loop() -> void* {
    Task<Value>::Yield();
    return &_value;
  }
  //
 private:
  Value _value;
};
//
const int kOk(8);
//
TEST(Thread, Run) {
  Task<int> task(kOk);
  // Run
  auto done = Task<int>::Run(task);
  EXPECT_EQ(0, done);
  int value(0);
  // Join
  done = task.Join(value);
  ASSERT_EQ(0, done);
  EXPECT_EQ(kOk, value);
}
//
TEST(Thread, Run_cpu) {
  Task<int> task(kOk);
  // Run
  auto done = Task<int>::Run(task, 0);
  EXPECT_EQ(0, done);
  int value(0);
  // Join
  done = task.Join(value);
  ASSERT_EQ(0, done);
  EXPECT_EQ(kOk, value);
  // Run cpu
  done = Task<int>::Run(task, 0);
  EXPECT_EQ(-1, done);
}
//
TEST(Thread, RunBackgroud) {
  Task<int> task(kOk);
  // Run
  auto done = Task<int>::RunBackgroud(task);
  EXPECT_EQ(0, done);
  int value(0);
  // Join
  sleep(1);
  done = task.Join(value);
  EXPECT_EQ(-1, done);
}
//
TEST(Thread, RunBackgroud_cpu) {
  Task<int> task(kOk);
  // Run
  auto done = Task<int>::RunBackgroud(task, 0);
  EXPECT_EQ(0, done);
  int value(0);
  // Join
  sleep(1);
  done = task.Join(value);
  EXPECT_EQ(-1, done);
}
//
