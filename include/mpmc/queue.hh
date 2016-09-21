// -*-coding:utf-8-unix;-*-
#pragma once
#include <stdint.h>
#include "macros.hh"
#include "atomic.hh"
//
namespace NAMESPACE {
namespace mpmc {
// http://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue
template<typename T, size_t kCapacity>
class BoundedQ {
  static_assert(kCapacity >= 2, "kCapacity should >= 2");
  static_assert(not (kCapacity bitand (kCapacity - 1)), "kCapacity should power of 2");
 public:
  BoundedQ(): _pad0(), _buffer(), _pad1(), _push(0), _pad2(), _pop(0), _pad3() {
    for (auto i(0LU); i < kCapacity; ++i) {
      atomic::Store(&(_buffer[i].sequence), i);
    }
    atomic::Store(&_push, 0LU);
    atomic::Store(&_pop, 0LU);
  }
  //
  auto Push(T* ptr) -> bool {
    Cell* cell(nullptr);
    auto pos = atomic::Load(&_push);
    while (true) {
      cell = &_buffer[pos bitand (kCapacity - 1)];
      auto seq = atomic::Load(&(cell->sequence));
      auto diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos);
      if(0 == diff) {
        // empty
        if (atomic::CAS(&_push, &pos, pos + 1)) {
          break;
        }
      } else if (0 > diff) {
        // full
        return false;
      } else {
        pos = atomic::Load(&_push);
      }
    }
    cell->ptr = ptr;
    atomic::Store(&(cell->sequence), pos + 1);
    return true;
  }
  //
  auto Pop() -> T* {
    Cell* cell(nullptr);
    auto pos = atomic::Load(&_pop);
    while (true) {
      cell = &_buffer[pos bitand (kCapacity - 1)];
      auto seq = atomic::Load(&(cell->sequence));
      auto diff = static_cast<intptr_t>(seq) - static_cast<intptr_t>(pos + 1);
      if (0 == diff) {
        if (atomic::CAS(&_pop, &pos, pos + 1)) {
          break;
        }
      } else if (0 > diff) {
        // empty
        return nullptr;
      } else {
        pos = atomic::Load(&_pop);
      }
    }
    auto ptr = cell->ptr;
    atomic::Store(&(cell->sequence), pos + kCapacity);
    return ptr;
  }
 protected:
  struct Cell {
    Cell(): ptr(nullptr), sequence(0) {}
    ~Cell() = default;
    T* ptr;
    size_t sequence;
  };
  //
  static const size_t kCacheLineSize = 64;
  typedef char CacheLinePad[kCacheLineSize];
  //
 private:
  CacheLinePad _pad0;
  Cell _buffer[kCapacity];
  CacheLinePad _pad1;
  size_t _push;
  CacheLinePad _pad2;
  size_t _pop;
  CacheLinePad _pad3;
  //
  DISALLOW_COPY_AND_ASSIGN(BoundedQ);
};
} // namespace mpmc
} // namespace NAMESPACE
