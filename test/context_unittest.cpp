// -*-coding:utf-8-unix;-*-
#include "gtest/gtest.h"
#include "context.hh"
//
using namespace NAMESPACE;
//
struct Task
    : public Context<Task, 128*1024> {
  void Run(){
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }
};
//
TEST(context, api) {
  Task t;
  bool ok = Task::Init(t);
  EXPECT_EQ(true, ok);
  Task t1;
  Task::SwitchIn(t1);
  Task t2;
  Task::SwitchOut(t2);
}

