// -*-coding:utf-8-unix;-*-
#include "gtest/gtest.h"
#include "mpsc/queue.hh"

class Session
    : public base::mpsc::Node<Session> {
 public:
  int _value;
};
//
const size_t kSize(10);
//
TEST(mpsc, Push) {
  base::mpsc::Queue<Session> q;
  Session s[kSize];
  for (size_t i(0); i < sizeof(s)/sizeof(s[0]); ++i) {
    s[i]._value = i;
    q.Push(s+i);
  }
}
//
TEST(mpsc, Pop) {
  base::mpsc::Queue<Session> q;
  Session s[kSize];
  Session* p = q.Pop();
  EXPECT_EQ(nullptr, p);
  for (size_t i(0); i < sizeof(s)/sizeof(s[0]); ++i) {
    s[i]._value = i;
    q.Push(s+i);
  }
  //
  for (size_t i(0); i < sizeof(s)/sizeof(s[0]); ++i) {
    p = q.Pop();
    EXPECT_NE(nullptr, p);
  }
}
