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
        ~Node() {
          _next = nullptr;
        }
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
      //
      void PushSingle(Value* node) {
        *_last = node;
        _last = &(node->_next);
      }
      // insert tail
      void Push(Value* node) {
        __transaction_atomic {
          *_last = node;
          _last = &(node->_next);
        }
      }
      // remove head
      Value* Pop() {
        Value* first(_first);
        __transaction_atomic {
          if (nullptr != _first) {
            first = _first;
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
