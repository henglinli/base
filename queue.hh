// -*-coding:utf-8-unix;-*-
#pragma once

#include <stddef.h> // for NULL
#include "atomic.hh"

namespace base {
namespace atomic {
template <typename Value>
class RefPointer {
 public:
  RefPointer()
      : ptr_(NULL)
      , count_(0) {
  }

  RefPointer(Value * ptr, size_t count)
      : ptr_(ptr)
      , count_(count) {
  }

  void Assign(Value * value) {
    RefPointer<Value> newval(value, 0);
    RefPointer<Value> oldval(ptr_, count_);
    while(not BoolComapreAndSwap(&data_, oldval.data_, newval.data_)) {
      ++(newval.count_);
      oldval.ptr_ = ptr_;
      oldval.count_ = count_;
    }
  }

  const Value * Get() {
    ++count_;
    return ptr_;
  }

  bool Comapre(const RefPointer<Value> & other) {
    return (ptr_ == other.ptr_ and count_ == other.count_);
  }

 private:
  template <typename>
  friend class SlistHead;
  template <typename>
  friend class SlistEntry;
  union {
    __int128 data_;
    struct {
      Value * ptr_;
      size_t count_;
    };
  };
}; // class RefPointer

template <typename Value>
class SlistEntry {
 public:
  Value * Next() {
    return next_.ptr_;
  }

  void InsertAfter(const Value * value) {
    value->next_ = this;
  }

 private:
  template<typename>
  friend class SlistHead;
  RefPointer<Value> next_;
}; // class SlistEntry

template <typename Value>
class SlistHead {
 public:
  Value * Next() {
    return next_.ptr_;
  }

  bool Insert(Value * value) {

  }

 private:
  RefPointer<Value> next_;
}; // class ListHead
} // namespace atomic
} // namespace base
