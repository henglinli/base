// -*-coding:utf-8-unix;-*-
#include "gtest/gtest.h"
#include "mpsc/queue.hh"

class Session
    : public base::mpsc::Node<Session> {
 public:
  int _value;
};
//
TEST(queue, base) {
  base::mpsc::Queue<Session> q;
  Session s[10];
  q.Push(s);
}

