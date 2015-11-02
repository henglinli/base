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
Align(
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
     protected:
     private:
      Node<Value> _node;
      Value* volatile _head;
      Align(
          struct {
            union {
              struct {
                uint64_t _aba;
                Value* _node;
              };
              uint128_t _data;
            };
          }, 16) _tail;
    }, 64);
} // namespace mpsc
} // namespace mpsc
