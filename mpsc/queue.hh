// -*-coding:utf-8-unix;-*-
#pragma once
#include "nullptr.hh"
#include "atomic/internal.hh"

namespace base {
namespace mpsc {
// www.1024cores.net version lockfree queue
template<typename Value>
struct Node {
  Value* volatile _next;
};
//
template<typename Value>
class Queue {
 public:
  Queue()
      : _head(&_stub)
      , _tail(&_stub) {
    // nil
  }
  //
  void Push(Value* value) {
    value->_next = nullptr;
    Value* prev = atomic::Exchange(&_head, value);
    prev->_next = value;
  }
  //
 protected:
 private:
  Value _stub;
  Value* _head;
  Value* _tail;
};
} // namespace mpsc
} // namespace mpsc

