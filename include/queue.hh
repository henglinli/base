// -*-coding:utf-8-unix;-*-
#pragma once
//
#include "macros.hh"
//
namespace NAMESPACE {
  //
  template<typename T>
  class TailQ {
  public:
    class Node {
    public:
      Node() : _next(nullptr), _prev(&_next) {}
      //
      virtual ~Node() {
      }
      //
      auto Next() const -> Node* {
        return _next;
      }
      //
      auto Prev() const -> T* {
        return *(reinterpret_cast<TailQ<T>*>(_prev)->_last);
      }
      //
    private:
      T *_next; // next element
      T **_prev; // address of previous next element
      //
      template<typename>
      friend class TailQ;
      //
      DISALLOW_COPY_AND_ASSIGN(Node);
    };
    //
    TailQ() : _first(nullptr), _last(&_first) {}
    //
    auto Init() -> void {
      _first = nullptr;
      _last = &_first;
    }
    //
    auto Push(T* other) -> void {
      other->_next = nullptr;
      other->_prev = _last;
      *_last = other;
      _last = &(other->_next);
    }
    //
    auto Remove(T* other) ->void {
      if (nullptr != other->_next) {
        other->_next->_prev = other->_prev;
      } else {
        _last = other->_prev;
      }
      *(other->_prev) = other->_next;
    }

    auto Pop() -> T* {
      if (nullptr == _first) {
        return nullptr;
      }
      auto first = _first;
      Remove(first);
      return first;
    }
    //
    auto Empty() const -> bool {
      return nullptr == _first;
    }
    //
    auto First() const -> T* {
      return _first;
    }
    //
    auto Last() const -> T* {
      return *(reinterpret_cast<TailQ<T>*>(_last)->_last);
    }
    //
  private:
    T* _first; // first element
    T** _last; // address of last next element
    //
    DISALLOW_COPY_AND_ASSIGN(TailQ);
  };
} // namespace NAMESPACE
