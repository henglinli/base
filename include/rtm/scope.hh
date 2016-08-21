// -*- coding:utf-8-unix; -*-
#pragma once
#include <x86intrin.h> // for __pause and rtm ...
#include "macros.hh"
#include "atomic.hh"
//
namespace NAMESPACE {
  //
  namespace rtm {
    class SpinLock {
    public:
      // Lock initialized with 0 initially
      SpinLock(): _state(kIdle) {}
      //
      void Lock() {
        while(kIdle not_eq __atomic_exchange_n(&_state, kBusy, __ATOMIC_ACQUIRE|__ATOMIC_HLE_ACQUIRE)) {
          State lock;
          // Wait for lock to become free again before retrying
          do {
            __pause(); // Abort speculation
            __atomic_load(&_state, &lock, __ATOMIC_CONSUME);
          } while(kBusy == lock);
        }
      }
      //
      void Unlock() {
        __atomic_store_n(&_state, kIdle, __ATOMIC_RELEASE|__ATOMIC_HLE_RELEASE);
      }
      //
      bool IsLocked() const {
        return kBusy == _state;
      }
      //
    private:
      enum State {
        kIdle = 0,
        kBusy = 1
      };
      volatile State _state;
      //
      DISALLOW_COPY_AND_ASSIGN(SpinLock);
    }; // class SpinLock
    //
    template<size_t kMaxRetries>
    class Scope {
    public:
      Scope(): _fallback_lock() {
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
              __pause(); // wait until lock is free
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
      //
    private:
      enum {
        kAbort = 0xff
      };
      SpinLock _fallback_lock;
    }; // class Scope
    //
  } // namespace rtm
} // namespace NAMESPACE
