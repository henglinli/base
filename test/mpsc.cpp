// -*-coding:utf-8-unix;-*-
#include "gtest/gtest.h"
#include "mpsc/queue.hh"
//
using namespace NAMESPACE;
class Session
    : public mpsc::Node<Session> {
 public:
  Session()
    : _value(0) {
    //
  }
  int _value;
};
//
const size_t kSize(10);
//
TEST(mpsc, Push) {
  mpsc::Queue<Session> q;
  Session s[kSize];
  for (size_t i(0); i < sizeof(s)/sizeof(s[0]); ++i) {
    s[i]._value = i;
    q.Push(s+i);
  }
}
//
TEST(mpsc, Pop) {
  mpsc::Queue<Session> q;
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
//
