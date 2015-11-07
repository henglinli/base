// -*-coding:utf-8-unix;-*-
#include "gtest/gtest.h"
#include "mpmc/queue.hh"
//
using namespace NAMESPACE;
//
class Session
    : public mpmc::Node<Session> {
 public:
  int _value;
};
//
const size_t kSize(10);
//
TEST(mpmc, Pop) {
  mpmc::Queue<Session> q;
  Session* p = q.Pop();
  EXPECT_EQ(static_cast<Session*>(nullptr), p);
}
//
TEST(mpmc, PushPop) {
  mpmc::Queue<Session> q;
  Session* p(nullptr);
  //
  Session s[kSize];
  for (size_t i(0); i < sizeof(s)/sizeof(s[0]); ++i) {
    s[i]._value = i;
    q.PushSingle(s+i);
  }
  //
  Session s1[kSize];
  for (size_t i(0); i < sizeof(s1)/sizeof(s[0]); ++i) {
    s1[i]._value = i + kSize;
    q.Push(s1+i);
  }
  //
  p = q.Pop();
  EXPECT_NE(static_cast<Session*>(nullptr), p);
  EXPECT_EQ(0, p->_value);
  //
  p = q.Pop();
  EXPECT_NE(static_cast<Session*>(nullptr), p);
  EXPECT_EQ(1, p->_value);
}

