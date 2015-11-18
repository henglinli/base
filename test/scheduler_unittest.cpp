// -*- coding:utf-8-unix; -*-
#include "gtest/gtest.h"
#include "scheduler.hh"
//
using namespace NAMESPACE;
//
const size_t kMaxCPU(4);
//
struct Task
    : public mpmc::Node<Task> {
  int _value;
  //
  bool DoWork() {
    return false;
  }
};
//
TEST(Scheduler, api) {
  Scheduler<Task, kMaxCPU> scheduler;
  scheduler.Start(4);
  std::cout << sizeof(scheduler) << std::endl;
}
//
