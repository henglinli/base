// -*-coding:utf-8-unix;-*-
#include "gtest/gtest.h"
#include "mpmc/queue.hh"

class Session
    : public base::mpmc::Node<Session> {
 public:
  int _value;
};
//
const size_t kSize(10);
//
TEST(mpmc, PushPop) {
  base::mpmc::Queue<Session> q;
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
  Session* p = q.Pop();
  EXPECT_NE(static_cast<Session*>(nullptr), p);
  EXPECT_EQ(0, p->_value);
  //
  p = q.Pop();
  EXPECT_NE(static_cast<Session*>(nullptr), p);
  EXPECT_EQ(1, p->_value);
}
//
#if 0
TEST(mpsc, Pop) {
  base::mpsc::Queue<Session> q;
  Session s[kSize];
  Session* p = q.Pop();
  EXPECT_EQ(static_cast<Session*>(nullptr), p);
  for (size_t i(0); i < sizeof(s)/sizeof(s[0]); ++i) {
    s[i]._value = i;
    q.Push(s+i);
  }
  //
  for (size_t i(0); i < sizeof(s)/sizeof(s[0]); ++i) {
    p = q.Pop();
    EXPECT_NE(static_cast<Session*>(nullptr), p);
  }
}
#endif
