// -*-coding:utf-8-unix;-*-
#include "gtest/gtest.h"
#include "fiber.hh"
//
using namespace NAMESPACE;
//
struct Error {
  void Run(){}
};
//
struct Task: public Fiber::Routine<Task> {
  void Run(){
    int google(0);
    std::cout << (&google) << std::endl;
    std::cout << __PRETTY_FUNCTION__ << (++_n) << std::endl;
  }
  static int _n;
};
int Task::_n(0);
//
struct Work: public Fiber::Routine<Work> {
  void Run() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }
};
//
struct Go: public Fiber::Routine<Go> {
  void Run() {
    Fiber f;
    Task t;
    auto ok = f.Init(t);
    EXPECT_TRUE(ok);
    //f.SwitchIn();
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }
};
//
TEST(Fiber, api) {
  Task t;
  Work w;
  Go g;
  //
  Fiber f;
  auto ok = f.MakeStack(64*1024);
  EXPECT_TRUE(ok);
  f.RunTask(t);
  f.RunTask(w);
  f.RunTask(g);
  //
  Error error;
  //f.RunTask(error);
}
//
#if 0
TEST(Fiber, new_api) {
  Go g;
  //
  Fiber f;
  auto ok = f.Init(g);
  EXPECT_TRUE(ok);
  f.SwitchIn();
  f.SwitchIn();
}
#endif
//
