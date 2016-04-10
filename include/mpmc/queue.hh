// -*-coding:utf-8-unix;-*-
#pragma once
#include <stdint.h>
#include "macros.hh"
#include "atomic.hh"

namespace NAMESPACE {
namespace mpmc {
// copy of https://github.com/CausalityLtd/ponyc/blob/master/src/libponyrt/sched/mpmcq.c
//
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
    : _node()
    , _head(static_cast<Value*>(&_node))
    , _tail() {
    _tail._aba = 0;
    _tail._ptr = static_cast<Value*>(&_node);
  }
  //
  void PushSingle(Value* value) {
    // If we have a single producer, don't use an atomic instruction.
    Value* prev = _head;
    _head = value;
    prev->_next = value;
  }
  //
  void Push(Value* value) {
    Value* prev = NAMESPACE::Exchange(&_head, value);
    prev->_next = value;
  }
  //
  Value* Pop() {
    _Node cmp, xchg;
    Value* next;
    //
    cmp._aba = _tail._aba;
    cmp._ptr = _tail._ptr;
    //
    do {
      // Get the next node rather than the tail.
      // The tail is either a stub or has already been consumed.
      next = cmp._ptr->_next;
      // Bailout if we have no next node.
      if (nullptr == next) {
        return nullptr;
      }
      // Make the next node the tail, incrementing the aba counter.
      // If this fails, cmp becomes the new tail and we retry the loop.
      xchg._aba = cmp._aba + 1;
      xchg._ptr = next;
    } while(not NAMESPACE::BoolComapreAndSwap(&(_tail._data), cmp._data, xchg._data));
    return next;
  }
  //
 protected:
 private:
  Align(
      struct _Node {
        union {
          struct {
            uint64_t _aba;
            Value* _ptr;
          };
          uint128_t _data;
        };
      }, 16);
  //
  Node<Value> _node;
  Value* _head;
  _Node _tail;
};
} // namespace mpmc
} // namespace base
