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
struct Task {
  auto Run() -> void {
    int google(0);
    std::cout << (&google) << " " <<  __PRETTY_FUNCTION__ << (++_n) << std::endl;
  }
  static int _n;
};
int Task::_n(0);
//
struct Work {
  auto Run() -> void {
    auto fiber = Fiber::Fork();
    EXPECT_NE(static_cast<Fiber*>(nullptr), fiber);
    std::cout << fiber << std::endl;
    fiber->Switch<Task>();
    Fiber::Join(fiber);
    EXPECT_EQ(static_cast<Fiber*>(nullptr), fiber);
    //
    int google(0);
    std::cout << (&google) << " " <<  __PRETTY_FUNCTION__ << std::endl;
  }
};
//
struct Go {
  auto Run() -> void {
    auto fiber = Fiber::Fork();
    EXPECT_NE(static_cast<Fiber*>(nullptr), fiber);
    std::cout << fiber << std::endl;
    fiber->Switch<Task>();
    Fiber::Join(fiber);
    EXPECT_EQ(static_cast<Fiber*>(nullptr), fiber);
    //
    fiber = Fiber::Fork();
    EXPECT_NE(static_cast<Fiber*>(nullptr), fiber);
    std::cout << fiber << std::endl;
    fiber->Switch<Work>();
    Fiber::Join(fiber);
    EXPECT_EQ(static_cast<Fiber*>(nullptr), fiber);
    //
    int google(0);
    std::cout << (&google) << " " <<  __PRETTY_FUNCTION__ << std::endl;
  }
};
//
TEST(Fiber, api) {
  auto fiber = Fiber::Fork();
  EXPECT_NE(static_cast<Fiber*>(nullptr), fiber);
  std::cout << fiber << std::endl;
  fiber->Switch<Go>();
  Fiber::Join(fiber);
  EXPECT_EQ(static_cast<Fiber*>(nullptr), fiber);
  //
  fiber = Fiber::Fork();
  EXPECT_NE(static_cast<Fiber*>(nullptr), fiber);
  std::cout << fiber << std::endl;
  fiber->Switch<Work>();
  Fiber::Join(fiber);
  EXPECT_EQ(static_cast<Fiber*>(nullptr), fiber);
}
//
