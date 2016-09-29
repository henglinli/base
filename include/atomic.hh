// -*-coding:utf-8-unix;-*-
#pragma once
#include "macros.hh"
//
namespace NAMESPACE {
namespace atomic {
// Load
template<typename Int>
inline auto Load(Int* ptr) -> Int {
  static_assert(sizeof(Int) <= sizeof(int128_t), "most 128 bit Integer type only");
  return __atomic_load_n(ptr, __ATOMIC_ACQUIRE);
};
// Store
template<typename Int>
inline auto Store(Int* ptr, Int value) -> void {
  static_assert(sizeof(Int) <= sizeof(int128_t), "most 128 bit Integer type only");
  __atomic_store_n(ptr, value, __ATOMIC_RELEASE);
};
// Exchage
template<typename Int>
inline auto Exchange(Int* ptr, Int value) -> Int {
  static_assert(sizeof(Int) <= sizeof(int128_t), "most 128 bit Integer type only");
  return __atomic_exchange_n(ptr, value, __ATOMIC_RELEASE);
}
// CAS
template<typename Int>
inline auto CAS(Int* ptr, Int* expected, Int value) -> bool {
  static_assert(sizeof(Int) <= sizeof(int128_t), "most 128 bit Integer type only");
  return __atomic_compare_exchange_n(ptr, expected, value, false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE);
}
// AddFetch
template<typename Int>
inline auto AddFetch(Int* ptr, Int value) -> Int {
  static_assert(sizeof(Int) <= sizeof(int128_t), "most 128 bit Integer type only");
  return __atomic_add_fetch(ptr, value, __ATOMIC_RELEASE);
}
//
union DoubleWord {
 public:
  auto Store(uintptr_t low, uintptr_t high) -> void {
    DoubleWord tmp;
    tmp._low = low;
    tmp._high = high;
    atomic::Store(&_data, tmp._data);
  }
  //
  auto Load(uintptr_t* low, uintptr_t* high) -> void {
    DoubleWord tmp;
    tmp._data = atomic::Load(&_data);
    *low = tmp._low;
    *high = tmp._high;
  }
  //
  auto Exchange(uintptr_t low, uintptr_t high,
                uintptr_t* prev_low, uintptr_t* prev_high) -> void {
    DoubleWord prev, tmp;
    tmp._low = low;
    tmp._high = high;
    prev._data = atomic::Exchange(&_data, tmp._data);
    *prev_low = prev._low;
    *prev_high = prev._high;
  }
  //
  auto CAS(uintptr_t* expected_low, uintptr_t* expected_high,
           uintptr_t low, uintptr_t high) -> bool {
    DoubleWord expected, tmp;
    tmp._low = low;
    tmp._high = high;
    auto ok = atomic::CAS(&_data, &expected._data, tmp._data);
    if (not ok) {
      *expected_low = expected._low;
      *expected_high = expected._high;
      return false;
    }
    return true;
  }
  //
  auto FetchAdd(uintptr_t low, uintptr_t high,
                uintptr_t* prev_low, uintptr_t* prev_high) -> void {
    Load(prev_low, prev_high);
    while (true) {
      if (CAS(prev_low, prev_high, *prev_low + low, *prev_high + high)) {
        break;
      }
    }
  }
  //
 private:
  struct {
    uintptr_t volatile _low;
    uintptr_t volatile _high;
  };
  uint128_t _data;
};
//
template<typename Object>
class DRCBase {
 public:
  using Self = DRCBase<Object>;
  //
  auto Acquire() -> void {
    Acquire(0, 1);
  }
  //
  auto Release() -> void {
    Release(0 , 1);
  }
  //
 protected:
  template<typename>
  friend class StrongRef;
  //
  auto Acquire(uintptr_t strong, uintptr_t basic) -> void {
    uintptr_t low, high;
    _state.FetchAdd(strong, basic, &low, &high);
  }
  //
  auto Release(uintptr_t strong, uintptr_t basic) -> void {
    uintptr_t low, high;
    _state.FetchAdd(-static_cast<intptr_t>(strong),
                    -static_cast<intptr_t>(basic),
                    &low, &high);
    if (low - strong == 0 and high - basic == 0) {
      Object::Release(this);
    }
  }
  //
 private:
  DoubleWord _state;
  //
  DISALLOW_COPY_AND_ASSIGN(DRCBase);
};
//
template<typename Object>
class StrongRef {
 public:
  typedef DRCBase<Object>* BasicPtr;
  //
  explicit StrongRef(BasicPtr ptr) {
    ptr->Acquire(1, 0);
    _state.Store(reinterpret_cast<uintptr_t>(ptr), 0);
  }
  //
  auto Acquire() -> BasicPtr {
    uintptr_t low, high;
    _state.FetchAdd(0, 1, &low, &high);
    return reinterpret_cast<BasicPtr>(low);
  }
  //
  auto Store(BasicPtr ptr) -> void {
    if (ptr) {
      ptr->Acquire(1, 0);
    }
    uintptr_t prev_ptr, prev_counter;
    _state.Exchange(reinterpret_cast<uintptr_t>(ptr), 0,
                    &prev_ptr, &prev_counter);
    if (prev_ptr) {
      reinterpret_cast<BasicPtr>(prev_ptr)->Release(1, -static_cast<intptr_t>(prev_counter));
    }
  }
  //
  auto Exchange(BasicPtr ptr) -> BasicPtr {
    if (ptr) {
      ptr->Acquire(1, 0);
    }
    uintptr_t prev_ptr, prev_counter;
    _state.Exchange(reinterpret_cast<uintptr_t>(ptr), 0,
                    &prev_ptr, &prev_counter);
    if (prev_ptr) {
      reinterpret_cast<BasicPtr>(prev_ptr)->Release(1, -static_cast<intptr_t>(prev_counter));
    }
    return reinterpret_cast<BasicPtr>(prev_ptr);
  }
  //
  auto CAS(BasicPtr* cmp, BasicPtr xchg) -> bool {
    if (xchg) {
      xchg->Acquire(1, 0);
    }
    uintptr_t low, high;
    _state.Load(&low, &high);
    while (true) {
      if (*cmp not_eq reinterpret_cast<BasicPtr>(low)) {
        if (*cmp) {
          *cmp = Acquire();
          if (xchg) {
            xchg->Release(1, 0);
          }
          return false;
        }
      }
      //
      if (_state.CAS(&low, &high, reinterpret_cast<uintptr_t>(xchg), 0)) {
        if (low) {
          reinterpret_cast<BasicPtr>(low)->Release(1, -static_cast<intptr_t>(high));
        }
        return true;
      }
    }
  }
  //
 private:
  DoubleWord _state;
  //
  DISALLOW_COPY_AND_ASSIGN(StrongRef);
};
//
} // namespace atomic
//
} // namespace NAMESPACE
