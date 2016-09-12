// -*-coding:utf-8-unix;-*-
#pragma once
#include <stdint.h>
#include "macros.hh"
#include "atomic.hh"
//
namespace NAMESPACE {
namespace mpmc {
// copy of https://github.com/CausalityLtd/ponyc/blob/master/src/libponyrt/sched/mpmcq.c
//
template<typename Value>
class StailQ {
 public:
  class Node {
   public:
    Node(): _next(nullptr) {}
    virtual ~Node() = default;
    //
   protected:
   private:
    Value* _next;
    //
    template<typename>
    friend class StailQ;
    //
    DISALLOW_COPY_AND_ASSIGN(Node);
  };
  //
  StailQ(): _node(), _head(static_cast<Value*>(&_node)), _tail() {
    _tail._aba = 0;
    _tail._ptr = static_cast<Value*>(&_node);
  }
  //
  void Push(Value* value) {
    auto prev = atomic::Exchange(&_head, value);
    atomic::Store(&(prev->_next), value);
  }
  //
  Value* Pop() {
    _Node cmp, xchg;
    Value* next(nullptr);
    //
    cmp._data = atomic::Load(&(_tail._data));
    //
    do {
      // Get the next node rather than the tail.
      // The tail is either a stub or has already been consumed.
      next = atomic::Load(&(cmp._ptr->_next));
      // Bailout if we have no next node.
      if (nullptr == next) {
        return nullptr;
      }
      // Make the next node the tail, incrementing the aba counter.
      // If this fails, cmp becomes the new tail and we retry the loop.
      xchg._aba = cmp._aba + 1;
      xchg._ptr = next;
    } while(not atomic::CAS(&(_tail._data), &(cmp._data), xchg._data));
    return next;
  }
  //
 protected:
  struct _Node {
    union {
      struct {
        uint64_t _aba;
        Value* _ptr;
      };
      uint128_t _data;
    };
  };
  //
 private:
  Node _node;
  Value* _head;
  _Node _tail;
  //
  DISALLOW_COPY_AND_ASSIGN(StailQ);
};
} // namespace mpmc
} // namespace NAMESPACE
