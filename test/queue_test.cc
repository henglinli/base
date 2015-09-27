// -*-coding:utf-8-unix;-*-
#include <iostream>
#include "mpsc/queue.hh"

class Session
    : public base::mpsc::Node<Session> {
 public:
  int _value;
};
//
int main() {
  base::mpsc::Queue<Session> q;
  Session s[10];
  q.Push(s);
  return 0;
}

