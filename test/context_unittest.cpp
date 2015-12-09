// -*-coding:utf-8-unix;-*-
#include "gtest/gtest.h"
#include "context.hh"
#include "routine.hh"
//
using namespace NAMESPACE;
//
struct Task
    : public Context<Task> {
  void Run(){
    std::cout << (++_n) << std::endl;
  }
  static int _n;
};
int Task::_n(0);
//
TEST(context, api) {
  Task t;
  Routine<Task, 128*1024> r;
  bool ok = r.Init(t);
  EXPECT_EQ(true, ok);
  Task t1;
  r.SwitchIn(t1);
  Task t2;
  r.SwitchOut(t2);
}

