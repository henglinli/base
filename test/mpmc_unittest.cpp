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
TEST(mpmc, PushSingle) {
  base::mpmc::Queue<Session> q;
  Session s[kSize];
  for (size_t i(0); i < sizeof(s)/sizeof(s[0]); ++i) {
    s[i]._value = i;
    q.PushSingle(s+i);
  }
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
