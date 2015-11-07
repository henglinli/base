// -*-coding:utf-8-unix;-*-
#include "gtest/gtest.h"
#include "processor.hh"
using namespace NAMESPACE;
//
TEST(processor, base) {
  uint32_t cpu = Processor::Current();
  std::cout << cpu << std::endl;
  EXPECT_GE(cpu, 0);
}
