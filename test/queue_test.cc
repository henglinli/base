// -*-coding:utf-8-unix;-*-
#include <iostream>
#include "queue.hh"

using namespace ::base::atomic;
class Message
    : public StailqEntry<Message> {
};

int main() {
  Stailq<Message> header;
  Message one;
  header.Push(&one);
  return 0;
}
