// -*-coding:utf-8-unix;-*-
#include "gtest/gtest.h"
#include "mpmc/queue.hh"
#include "gnutm/queue.hh"
//
using namespace NAMESPACE;
//
class Session: public mpmc::Node<Session> {
 public:
  Session()
    : _value(0) {
    //
  }
  int _value;
};
//
const size_t kSize(128);
//
TEST(mpmc, Pop) {
  mpmc::Queue<Session> q;
  auto p = q.Pop();
  EXPECT_EQ(nullptr, p);
  p = q.Pop();
  EXPECT_EQ(nullptr, p);
}
//
TEST(mpmc, PushPop) {
  mpmc::Queue<Session> q;
  Session* p(nullptr);
  //
  Session s[kSize];
  for (size_t i(0); i < sizeof(s)/sizeof(s[0]); ++i) {
    s[i]._value = i;
    q.Push(s+i);
  }
  //
  p = q.Pop();
  EXPECT_NE(nullptr, p);
  EXPECT_EQ(0, p->_value);
  //
  p = q.Pop();
  EXPECT_NE(nullptr, p);
  EXPECT_EQ(1, p->_value);
}
//
