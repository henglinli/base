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
      ~Node() {
        Unlink();
      }
      //
      Node* Next() const {
        return _next;
      }
      //
      void Unlink() {
        if (nullptr != _next) {
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
    void Push(Node* other) {
      if (nullptr != (other->_next = _first)) {
        _first->_prev = &(other->_next);
      }
      _first = other;
      other->_prev = &_first;
    }
    //
    Node* First() const {
      return _first;
    }
    //
    bool Empty() const {
      return nullptr == _first;
    }
    //
  private:
    Node *_first;
    //
    DISALLOW_COPY_AND_ASSIGN(List);
  };
} // namespace NAMESPACE
