// -*-coding:utf-8-unix;-*-
#pragma once

namespace base {
namespace atomic {
// cas
template <typename Integer>
bool BoolComapreAndSwap(Integer * ptr, Integer oldval, Integer newval) {
  return __sync_bool_compare_and_swap(ptr, oldval, newval);
}

template <typename Integer>
Integer ValueComapreAndSwap(Integer * ptr, Integer oldval, Integer newval) {
  return __sync_val_compare_and_swap(ptr, oldval, newval);
}

// add 
template <typename Integer>
Integer FetchAndAdd(Integer * ptr, Integer value) {
  return __sync_fetch_and_add(ptr, value);
}

template <typename Integer>
Integer AddAndFetch(Integer * ptr, Integer value) {
  return __sync_add_and_fetch(ptr, value);
}

// sub
template <typename Integer>
Integer FetchAndSub(Integer * ptr, Integer value) {
  return __sync_fetch_and_sub(ptr, value);
}

template <typename Integer>
Integer SubAndFetch(Integer * ptr, Integer value) {
  return __sync_sub_and_fetch(ptr, value);
}

// or
template <typename Integer>
Integer FetchAndOr(Integer * ptr, Integer value) {
  return __sync_fetch_and_or(ptr, value);
}

template <typename Integer>
Integer OrAndFetch(Integer * ptr, Integer value) {
  return __sync_or_and_fetch(ptr, value);
}

// and
template <typename Integer>
Integer FetchAndAnd(Integer * ptr, Integer value) {
  return __sync_fetch_and_and(ptr, value);
}

template <typename Integer>
Integer AndAndFetch(Integer * ptr, Integer value) {
  return __sync_and_and_fetch(ptr, value);
}

// xor
template <typename Integer>
Integer FetchAndXor(Integer * ptr, Integer value) {
  return __sync_fetch_and_xor(ptr, value);
}

template <typename Integer>
Integer XorAndFetch(Integer * ptr, Integer value) {
  return __sync_xor_and_fetch(ptr, value);
}

// nand
template <typename Integer>
Integer FetchAndNand(Integer * ptr, Integer value) {
  return __sync_fetch_and_nand(ptr, value);
}

template <typename Integer>
Integer NandAndFetch(Integer * ptr, Integer value) {
  return __sync_nand_and_fetch(ptr, value);
}

} // namespace atomic 
} // namespace base
