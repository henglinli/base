// -*-coding:utf-8-unix;-*-
#include "gtest/gtest.h"
#include "coroutine.hh"
//
using namespace NAMESPACE;
//
struct Error {
  void Run(){}
};
//
struct Task: public Coroutine {
  void Run(){
    std::cout << __PRETTY_FUNCTION__ << (++_n) << std::endl;
  }
  static int _n;
};
int Task::_n(0);
//
struct Work: public Coroutine {
  void Run() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }
};
//
struct Go: public Coroutine {
  void Run() {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    Work w;
    auto ok = Work::Init(w);
    EXPECT_EQ(true, ok);
    w.SwitchIn();
  }
};
//
TEST(coroutine, api) {
  Work w;
  Task t;
  //
  auto ok = Coroutine::Init(w);
  EXPECT_EQ(true, ok);
  ok = Coroutine::Init(t);
  EXPECT_EQ(true, ok);
  //
  Coroutine::SwitchIn(t);
  Coroutine::SwitchIn(w);
  t.SwitchIn();
  w.SwitchIn();
  //
  Go g;
  ok = Go::Init(g);
  EXPECT_EQ(true, ok);
  g.SwitchIn();
  g.SwitchIn();
  //
  Error error;
  //Coroutine::Init(error);
  //
}
//
