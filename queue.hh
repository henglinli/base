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

  explicit RefPointer(__int128 data)
      : data_(data) {
  }

  void Assign(Value * value) {
    RefPointer<Value> newval(value, 0);
    RefPointer<Value> oldval(data_);
    while(not BoolComapreAndSwap(&data_, oldval.data_, newval.data_)) {
      oldval.data_ = data_;
    }
  }

  const Value * Retrieve () {
    RefPointer<Value> oldval(data_);
    RefPointer<Value> newval(oldval.ptr_, oldval.count_ + 1);
    while(not BoolComapreAndSwap(&data_, oldval.data_, newval.data_)) {
      oldval.data_ = data_;
      newval.count_ = oldval.count_ + 1;
    }
    return newval.ptr_;
  }

  bool Comapre(const RefPointer<Value> & other) {
    return (ptr_ == other.ptr_ and count_ == other.count_);
  }

 private:
  template <typename>
  friend class Stailq;
  template <typename>
  friend class StailqEntry;
  union {
    __int128 data_;
    struct {
      Value * ptr_;
      size_t count_;
    };
  };
}; // class RefPointer

template <typename Value>
class StailqEntry {
 public:
  void InsertAfter(const Value * value) {
    value->next_ = this;
  }

 private:
  template<typename>
  friend class Stailq;
  RefPointer<Value> next_;
}; // class SlistEntry

template <typename Value>
class Stailq {
 public:
  Stailq()
      : head_()
      , tail_(&(head_.ptr_), 0) {
  }
  
  bool Push(Value * value) {
    return false;
  }

  const Value * Pop() {
    return NULL;
  }
  
 private:
  RefPointer<Value> head_;
  RefPointer<Value *> tail_;
}; // class ListHead
} // namespace atomic
} // namespace base
