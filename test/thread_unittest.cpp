// -*-coding:utf-8-unix;-*-
#include "gtest/gtest.h"
#include "thread.hh"
//
using namespace NAMESPACE;
//
template<typename Value>
class Task {
 public:
  explicit Task(Value value)
      : _value(value) {
  }
  //
  Value* Loop() {
    Thread<Task, int>::Yield();
    return &_value;
  }
  //
 private:
  Value _value;
};
//
TEST(thread, task) {
  Thread<Task<int>, int> thread;
  Task<int> task(1);
  int done(-1);
  done = thread.Run(&task);
  ASSERT_EQ(0, done);
  int value(0);
  done = thread.Join(&value);
  ASSERT_EQ(0, done);
  EXPECT_EQ(1, value);
}
//
TEST(thread, backgroud_task) {
  Thread<Task<int>, int> thread;
  Task<int> task(1);
  int done(-1);
  done = thread.RunBackgroud(&task);
  ASSERT_EQ(0, done);
  done = thread.Cancel();
  EXPECT_EQ(0, done);
}
//
