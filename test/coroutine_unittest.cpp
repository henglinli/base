// -*-coding:utf-8-unix;-*-
#include "gtest/gtest.h"
#include "coroutine.hh"
//
using namespace NAMESPACE;
//
struct Task {
  void Run(){
    std::cout << (++_n) << std::endl;
  }
  static int _n;
};
int Task::_n(0);
//
struct Work {
  void Run() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }
};
//
TEST(context, api) {
  Work w;
  Task t;
  Coroutine<128*1024> r[2];
  bool ok = r[0].Init(t);
  EXPECT_EQ(true, ok);
  ok = r[1].Init(w);
  EXPECT_EQ(true, ok);
  r[0].Switch(r[1]);
}

