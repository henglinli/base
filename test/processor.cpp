// -*- coding:utf-8-unix; -*-
#include "gtest/gtest.h"
#include "processor.hh"
//
using namespace NAMESPACE;
//
TEST(processor, api) {
  uint32_t cpu = Processor::Current();
  EXPECT_GE(cpu, 0U);
  uint32_t stap = Processor::Timestap();
  EXPECT_GE(stap, 1U);
  //
  stap = Processor::Rdtscp(&cpu);
  EXPECT_GE(cpu, 0U);
  EXPECT_GE(stap, 1U);
  uint32_t count = Processor::Count();
  printf("cpus %u\n", count);
  EXPECT_GE(count, 1u);
}
