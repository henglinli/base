// -*-coding:utf-8-unix;-*-
#include "gtest/gtest.h"
#include "thread.hh"
//
using namespace NAMESPACE;
//
class Task {
 public:
  explicit Task(int value)
      : _value(value) {
  }
  //
  int* Loop() {
    return &_value;
  }
  //
 private:
  int _value;
};
//
TEST(thread, task) {
  Thread<Task> thread;
  Task task(1);
  int done(-1);
  done = thread.Run(&task);
  ASSERT_EQ(0, done);
  int value = thread.Join();
  EXPECT_EQ(1, value);
}
