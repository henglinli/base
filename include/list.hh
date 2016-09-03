// -*-coding:utf-8-unix;-*-
#pragma once
//
#include "macros.hh"
//
namespace NAMESPACE {
  //
  template<typename T>
  class List {
  public:
    class Node {
    public:
      Node() : _next(nullptr), _prev(&_next) {}
      //
      virtual ~Node() {
        Unlink();
      }
      //
      auto Next() const -> Node* {
        return _next;
      }
      //
      auto Unlink() -> void {
        if (nullptr not_eq _next) {
          _next->_prev = _prev;
        }
        *_prev = _next;
      }
      //
    private:
      Node *_next;
      Node **_prev;
      //
      template<typename>
      friend class List;
      //
      DISALLOW_COPY_AND_ASSIGN(Node);
    };
    //
    List() : _first(nullptr) {}
    //
    auto Push(Node* other) -> void {
      if (nullptr not_eq (other->_next = _first)) {
        _first->_prev = &(other->_next);
      }
      _first = other;
      other->_prev = &_first;
    }
    //
    auto First() const -> Node* {
      return _first;
    }
    //
    auto Empty() const -> bool {
      return nullptr == _first;
    }
    //
  private:
    Node *_first;
    //
    DISALLOW_COPY_AND_ASSIGN(List);
  };
} // namespace NAMESPACE
