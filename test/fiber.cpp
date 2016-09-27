// -*-coding:utf-8-unix;-*-
#include "gtest/gtest.h"
#include "fiber.hh"
//
typedef NAMESPACE::Fiber<> Fiber;
//
struct Empty {
  static void Run(void*) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }
};
//
struct Task {
  static auto Run(void*) -> void {
    int google(0);
    std::cout << (&google) << " " <<  __PRETTY_FUNCTION__ << (++_n) << std::endl;
  }
  static int _n;
};
int Task::_n(0);
//
struct Work {
  static auto Run(void*) -> void {
    auto fiber = Fiber::Fork(Task::Run, nullptr);
    EXPECT_NE(fiber, nullptr);
    std::cout << fiber << std::endl;
    Fiber::Join();
    //
    int google(0);
    std::cout << (&google) << " " <<  __PRETTY_FUNCTION__ << std::endl;
  }
};
//
struct Go {
  static auto Run(void*) -> void {
    auto fiber = Fiber::Fork(Task::Run, nullptr);
    EXPECT_NE(fiber, nullptr);
    std::cout << fiber << std::endl;
    //
    fiber = Fiber::Fork(Work::Run, nullptr);
    EXPECT_NE(fiber, nullptr);
    std::cout << fiber << std::endl;
    Fiber::Join();
    //
    int google(0);
    std::cout << (&google) << " " <<  __PRETTY_FUNCTION__ << std::endl;
  }
};
//
TEST(Fiber, simple) {
  auto fiber = Fiber::Fork(Empty::Run, nullptr);
  EXPECT_NE(fiber, nullptr);
  fiber = Fiber::Fork(Task::Run, nullptr);
  EXPECT_NE(fiber, nullptr);
  Fiber::Switch(fiber);
  Fiber::Join();
}
//
TEST(Fiber, api) {
  auto fiber = Fiber::Fork(Empty::Run, nullptr);
  EXPECT_NE(fiber, nullptr);
  std::cout << fiber << std::endl;
  //
  fiber = Fiber::Fork(Go::Run, nullptr);
  EXPECT_NE(fiber, nullptr);
  std::cout << fiber << std::endl;
  //
  fiber = Fiber::Fork(Work::Run, nullptr);
  EXPECT_NE(fiber, nullptr);
  std::cout << fiber << std::endl;
  Fiber::Join();
}
//
