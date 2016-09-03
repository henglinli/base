// -*- coding:utf-8-unix; -*-
#include "gtest/gtest.h"
#include "processor.hh"
//
using namespace NAMESPACE;
//
struct Task
  : public gnutm::StailQ<Task>::Node {
  int _value;
};
//
TEST(processor, api) {
  uint32_t cpu = Processor<Task>::Current();
  EXPECT_GE(cpu, 0U);
  uint32_t stap = Processor<Task>::Timestap();
  EXPECT_GE(stap, 1U);
  //
  stap = Processor<Task>::Rdtscp(&cpu);
  EXPECT_GE(cpu, 0U);
  EXPECT_GE(stap, 1U);
  uint32_t count = Processor<Task>::Count();
  printf("cpus %u\n", count);
  EXPECT_GE(count, 1u);
}
