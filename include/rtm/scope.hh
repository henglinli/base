// -*- coding:utf-8-unix; -*-
#pragma once
#include <immintrin.h> // for _mm_pause
#include "rtm/rtm.hh"
#include "atomic.hh"
//
namespace NAMESPACE {
//
class SpinLock {
 public:
  SpinLock()
      : _state(kFree) {
  }
  //
  void Lock() {
    while(kFree not_eq ValueComapreAndSwap(&_state, kBusy, kFree)) {
      do {
        _mm_pause();
      } while(kBusy == _state);
    }
  }
  //
  void Unlock() {
    Exchange(&_state, kFree);
  }
  //
  bool IsLocked() const {
    return kBusy == _state;
  }
  //
 private:
  enum State {
    kFree = 0,
    kBusy = 1
  };
  volatile State _state;
}; // class SpinLock
//
template<size_t kMaxRetries>
class Scope {
 public:
  Scope(SpinLock& fallback_lock)
      : _fallback_lock(fallback_lock) {
    unsigned status(0);
    for(int i(0); i < kMaxRetries; ++i) {
      status = _xbegin();
      if(_XBEGIN_STARTED == status) {
        if(not _fallback_lock.IsLocked()) {
          return; // successfully started transaction
          // started transaction but someone executes the transaction section
          // non-speculatively (acquired the fall-back lock)
          _xabort(kAbort);
        }
      }
      // handle _xabort from above
      if((status bitand _XABORT_EXPLICIT)
         and (kAbort == _XABORT_CODE(status))
         and not(status bitand _XABORT_NESTED)) {
        while(_fallback_lock.IsLocked()) {
          _mm_pause(); // wait until lock is free
        }
      } else if(not(status bitand _XABORT_RETRY)) {
        break;
      }
    }
    //
    _fallback_lock.Lock();
  }
  //
  ~Scope() {
    if(_fallback_lock.IsLocked()) {
      _fallback_lock.Unlock();
    } else {
      _xend();
    }
  }
 private:
  enum {
    kAbort = 0xff
  };
  SpinLock& _fallback_lock;
}; // class Scope
//
} // namespace NAMESPACE
