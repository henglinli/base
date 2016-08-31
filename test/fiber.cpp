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
struct Work: public Fiber::Routine<Task> {
  void Run() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }
};
//
struct Go: public Fiber::Routine<Task> {
  void Run() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }
};
//
TEST(coroutine, api) {
  Task t;
  Work w;
  //
  Fiber f;
  auto ok = f.Init(t, 64*1024);
  EXPECT_EQ(true, ok);
  f.SwitchIn();
  //
  Error error;
  //Coroutine::Init(error);
  //
}
//
