// -*- coding:utf-8-unix; -*-
#include "gtest/gtest.h"
#include "processor.hh"
//
using namespace NAMESPACE;
//
struct Task
    : public mpmc::Node<Task> {
  int _value;
};
//
TEST(processor, api) {
  uint32_t cpu = Processor<Task>::Current();
  EXPECT_GE(cpu, 0);
  uint32_t stap = Processor<Task>::Timestap();
  EXPECT_GE(stap, 1);
  //
  stap = Processor<Task>::Rdscp(&cpu);
  EXPECT_GE(cpu, 0);
  EXPECT_GE(stap, 1);
}
