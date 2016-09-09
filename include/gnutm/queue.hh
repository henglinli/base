// -*-coding:utf-8-unix;-*-
#pragma once
#include "macros.hh"
//
namespace NAMESPACE {
namespace gnutm {
//
template<typename Value>
class StailQ {
 public:
  class Node {
   public:
    Node(): _next(nullptr){}
    virtual ~Node() = default;
    //
   private:
    Value* _next;
    //
    template<typename>
    friend class StailQ;
    //
    DISALLOW_COPY_AND_ASSIGN(Node);
  };
  //
  StailQ(): _first(nullptr), _last(&_first) {}
  // insert tail
  auto Push(Value* node) -> void {
    __transaction_atomic {
      node->_next = nullptr;
      *_last = node;
      _last = &(node->_next);
    }
  }
  // remove head
  auto Pop() -> Value* {
    __transaction_atomic {
     auto first(_first);
      if (nullptr != _first) {
        if (nullptr == (_first = _first->_next)) {
          _last = &_first;
        }
      }
      return first;
    }
  }
  //
 private:
  Value* _first;
  Value** _last;
  //
  DISALLOW_COPY_AND_ASSIGN(StailQ);
};
} // namespace gnutm
} // namespace NAMESPACE
