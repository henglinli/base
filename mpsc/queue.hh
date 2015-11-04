// -*-coding:utf-8-unix;-*-
#pragma once
#include "macros.hh"
#include "atomic.hh"

namespace base {
namespace mpsc {
// www.1024cores.net version lockfree queue
template<typename Value>
class Node {
 public:
  Node()
      : _next(nullptr) {
  }
  //
 protected:
 private:
  template<typename>
  friend class Queue;
  Value* volatile _next;
};
//
template<typename Value>
class Queue {
 public:
  Queue()
      : _head(static_cast<Value*>(&_stub))
      , _tail(static_cast<Value*>(&_stub)) {
    // nil
  }
  //
  void Push(Value* value) {
    value->_next = nullptr;
    Value* prev = atomic::Exchange(&_head, value);
    prev->_next = value;
  }
  //
  Value* Pop() {
    Value* tail = _tail;
    Value* next = tail->_next;
    //
    if (&_stub == tail) {
      if (nullptr == next) {
        return nullptr;
      }
      //
      _tail = next;
      tail = next;
      next = next->_next;
    }
    //
    if (nullptr not_eq next) {
      _tail = next;
      return tail;
    }
    //
    Value* head = _head;
    if (tail not_eq head) {
      return nullptr;
    }
    //
    Push(static_cast<Value*>(&_stub));
    next = tail->_next;
    //
    if(nullptr not_eq next) {
      _tail = next;
      return tail;
    }
    return nullptr;
  }
  //
 protected:
 private:
  Node<Value> _stub;
  Value* _head;
  Value* _tail;
};
} // namespace mpsc
} // namespace mpsc
