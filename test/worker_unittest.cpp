// -*- coding:utf-8-unix; -*-
#include "gtest/gtest.h"
#include "worker.hh"
//
using namespace NAMESPACE;
//
struct Task
    : public mpmc::Node<Task> {
  int _value;
};
//
TEST(processor, api) {
  Worker<Task, int> worker;
}
