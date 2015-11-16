// -*- coding:utf-8-unix; -*-
#include "gtest/gtest.h"
#include "scheduler.hh"
//
using namespace NAMESPACE;
//
//
struct Task
    : public mpmc::Node<Task> {
  int _value;
};
//
TEST(processor, api) {
  Scheduler<Task> scheduler;
  scheduler.Start(4);
}
