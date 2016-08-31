// -*-coding:utf-8-unix;-*-
#include "gtest/gtest.h"
#include "thread.hh"
//
using namespace NAMESPACE;
//
template<typename Value>
class Task: public Thread::Routine<Task<Value> > {
 public:
  explicit Task(Value value)
      : _value(value) {
  }
  //
  void* Loop() {
    Thread::Yield();
    return &_value;
  }
  //
 private:
  Value _value;
};
//
const int kOk(8);
//
TEST(thread, task) {
  Thread thread;
  Task<int> task(kOk);
  int done(-1);
  done = thread.Run(task);
  ASSERT_EQ(0, done);
  int value(0);
  done = thread.Join(value);
  ASSERT_EQ(0, done);
  EXPECT_EQ(kOk, value);
}
//
TEST(thread, backgroud_task) {
  Thread thread;
  Task<int> task(1);
  int done(-1);
  done = thread.RunBackgroud(task);
  ASSERT_EQ(0, done);
  sleep(1);
  EXPECT_EQ(0, done);
}
//
