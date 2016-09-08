// -*-coding:utf-8-unix;-*-
#include "gtest/gtest.h"
#include "queue.hh"
//
using namespace NAMESPACE;
//
struct Context final: public TailQ<Context>::Node {
  static thread_local TailQ<Context> list;
};
//
thread_local TailQ<Context> Context::list;
//
TEST(TailQ, api) {
  Context c[4];
  auto empty = Context::list.Empty();
  EXPECT_EQ(true, empty);
  Context::list.Push(&c[0]);
  auto first = Context::list.First();
  EXPECT_EQ(&c[0], first);
  Context::list.Push(&c[1]);
  first = Context::list.First();
  auto next = first->Next();
  EXPECT_EQ(next, &c[1]);
  empty = Context::list.Empty();
  EXPECT_EQ(false, empty);
}
