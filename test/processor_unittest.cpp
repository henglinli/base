// -*- coding:utf-8-unix; -*-
#include "gtest/gtest.h"
#include "processor.hh"
//
using namespace NAMESPACE;
//
TEST(processor, api) {
  uint32_t cpu = Processor::Current();
  EXPECT_GE(cpu, 0);
  uint32_t stap = Processor::Timestap();
  EXPECT_GE(stap, 1);
  //
  stap = Processor::Rdscp(&cpu);
  EXPECT_GE(cpu, 0);
  EXPECT_GE(stap, 1);
}
