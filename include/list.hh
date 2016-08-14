// -*-coding:utf-8-unix;-*-

#pragma once

#include "macros.hh"

namespace NAMESPACE {
  //
  template<typename T>
  class List {
  public:
    class Node {
    public:
      Node() : _prev(nullptr), _next(nullptr) {}
      //
      Node(Node *prev, Node *next)
        : _prev(prev) , _next(next) {}
      //
      ~Node() {
        Unlink();
      }
      //
      void InsertBefore(Node *other) {
        _next = other;
        _prev = other->_prev;
        other->_prev->_next = this;
        other->_prev = this;
      }
      //
      void InsertAfter(Node *other) {
        _next = other->_next;
        _prev = other;
        other->_next->_prev = this;
        other->_next = this;
      }
      //
      void Unlink() {
        if (nullptr != _prev) {
          _prev->_next = _next;
        }
        if (nullptr != _next) {
          _next->_prev = _prev;
        }
        _next = nullptr;
        _prev = nullptr;
      }
      //
      Node *Previous() const {
        return _prev;
      }
      //
      Node *Next() const {
        return _next;
      }
      //
    private:
      Node *_prev;
      Node *_next;
      //
      DISALLOW_COPY_AND_ASSIGN(Node);
    };
    //
    List() : _root(&_root, &_root) {}
    //
    void Append(T *other) {
      other->InsertBefore(&_root);
    }
    //
    T *Head() const {
      return _root.Next();
    }
    //
    T *Tail() const {
      return _root.Previous();
    }
    //
    const T *End() const {
      return &_root;
    }
    //
    bool Empty() const {
      return _root.Next() == _root.Previous();
    }
    //
  private:
    Node _root;
    DISALLOW_COPY_AND_ASSIGN(List);
  };
} // namespace NAMESPACE
