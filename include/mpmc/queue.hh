// -*-coding:utf-8-unix;-*-
#pragma once
#include <stdint.h>
#include "macros.hh"
#include "atomic.hh"

namespace base {
namespace mpmc {
// copy of https://github.com/CausalityLtd/ponyc/blob/master/src/libponyrt/sched/mpmcq.h
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
      : _head(static_cast<Value*>(&_node)) {
    _tail._node = static_cast<Value*>(&_node);
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
    Value* prev = atomic::Exchange(&_head, value);
    prev->_next = value;
  }
  //
  Value* Pop() {
    _Node cmp, xchg;
    Value* next;
    //
    cmp._aba = _tail._aba;
    cmp._node = _tail._node;
    //
    do {
      // Get the next node rather than the tail.
      // The tail is either a stub or has already been consumed.
      next = cmp._node->_next;
      // Bailout if we have no next node.
      if (nullptr == next) {
        return nullptr;
      }
      // Make the next node the tail, incrementing the aba counter.
      // If this fails, cmp becomes the new tail and we retry the loop.
      xchg._aba = cmp._aba + 1;
      xchg._node = next;
    } while(not atomic::BoolComapreAndSwap(&(_tail._data), cmp._data, xchg._data));
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
            Value* _node;
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
