// -*-coding:utf-8-unix;-*-
#pragma once
//
namespace base {
// cas
template <typename Integer>
inline bool BoolComapreAndSwap(Integer * ptr,
                               Integer oldval, Integer newval) {
  return __sync_bool_compare_and_swap(ptr, oldval, newval);
}

template <typename Integer>
inline Integer ValueComapreAndSwap(Integer * ptr,
                                   Integer oldval, Integer newval) {
  return __sync_val_compare_and_swap(ptr, oldval, newval);
}

// add 
template <typename Integer>
inline Integer FetchAndAdd(Integer * ptr, Integer value) {
  return __sync_fetch_and_add(ptr, value);
}

template <typename Integer>
inline Integer AddAndFetch(Integer * ptr, Integer value) {
  return __sync_add_and_fetch(ptr, value);
}

// sub
template <typename Integer>
inline Integer FetchAndSub(Integer * ptr, Integer value) {
  return __sync_fetch_and_sub(ptr, value);
}

template <typename Integer>
inline Integer SubAndFetch(Integer * ptr, Integer value) {
  return __sync_sub_and_fetch(ptr, value);
}

// or
template <typename Integer>
inline Integer FetchAndOr(Integer * ptr, Integer value) {
  return __sync_fetch_and_or(ptr, value);
}

template <typename Integer>
inline Integer OrAndFetch(Integer * ptr, Integer value) {
  return __sync_or_and_fetch(ptr, value);
}

// and
template <typename Integer>
inline Integer FetchAndAnd(Integer * ptr, Integer value) {
  return __sync_fetch_and_and(ptr, value);
}

template <typename Integer>
inline Integer AndAndFetch(Integer * ptr, Integer value) {
  return __sync_and_and_fetch(ptr, value);
}

// xor
template <typename Integer>
inline Integer FetchAndXor(Integer * ptr, Integer value) {
  return __sync_fetch_and_xor(ptr, value);
}

template <typename Integer>
inline Integer XorAndFetch(Integer * ptr, Integer value) {
  return __sync_xor_and_fetch(ptr, value);
}

// nand
template <typename Integer>
inline Integer FetchAndNand(Integer * ptr, Integer value) {
  return __sync_fetch_and_nand(ptr, value);
}

template <typename Integer>
inline Integer NandAndFetch(Integer * ptr, Integer value) {
  return __sync_nand_and_fetch(ptr, value);
}

// exchange
template <typename Integer>
inline Integer Exchange(Integer* ptr, Integer value) {
  return __sync_lock_test_and_set(ptr, value);
}
//
} // namespace base

