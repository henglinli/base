// -*-coding:utf-8-unix;-*-
#include "gtest/gtest.h"
#include "coroutine.hh"
//
using namespace NAMESPACE;
//
struct Task
  : public Coroutine<Task> {
  void Run(){
    std::cout << __PRETTY_FUNCTION__ << (++_n) << std::endl;
  }
  static int _n;
};
int Task::_n(0);
//
struct Work
  : public Coroutine<Work> {
  void Run() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }
};
//
TEST(coroutine, api) {
  Work w;
  Task t;
  //
  auto ok = Work::Init(w);
  EXPECT_EQ(true, ok);
  ok = Task::Init(t);
  EXPECT_EQ(true, ok);
}
//
