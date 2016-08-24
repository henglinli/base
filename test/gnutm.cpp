// -*- coding:utf-8-unix; -*-
#include "gtest/gtest.h"
#include "gnutm/queue.hh"
#include "thread.hh"
//
using namespace NAMESPACE;
//
struct Google: public gnutm::StailQ<Google>::Node {};
//
gnutm::StailQ<Google> q;
//
struct Producer {
  int _sum;
  int _result;
  Google* _g;
  explicit Producer(size_t sum): _sum(sum), _result(-1), _g(nullptr) {}
  //
  ~Producer() {
    if (_g) {
      delete[] _g;
    }
  }
  //
  int* Loop() {
    _g = new Google[_sum];
    printf("Producer%p\n", &_result);
    for (_result = 0; _result < _sum; ++_result) {
      q.Push(_g + _result);
    }
    return &_result;
  }
  //
  DISALLOW_COPY_AND_ASSIGN(Producer);
};
//
struct Consumer {
  int _sum;
  int _result;
  Google* _g;
  explicit Consumer(size_t sum): _sum(sum), _result(-1), _g(nullptr) {}
  //
  int* Loop() {
    printf("Producer%p\n", &_result);
    for (_result = 0; _result < _sum;) {
      _g = q.Pop();
      if(_g) {
        ++_result;
      }
    }
    return &_result;
  }
  //
  DISALLOW_COPY_AND_ASSIGN(Consumer);
};
//
TEST(gnutm, stailq) {
  Thread<Consumer, int> mc1, mc2;
  Thread<Producer, int> mp1, mp2;
  Consumer c1(1000*10), c2(1000*11);
  Producer p1(1000*12), p2(1000*13);
  //
  int done(-1);
  done = mc1.Run(c1);
  EXPECT_EQ(0, done);
  done = mc2.Run(c2);
  EXPECT_EQ(0, done);
  //
  done = mp1.Run(p1);
  EXPECT_EQ(0, done);
  done = mp2.Run(p2);
  //
  int result(0);
  done = mp1.Join(result);
  EXPECT_EQ(0, done);
  printf("mp1 %d\n", result);
  done = mp2.Join(result);
  EXPECT_EQ(0, done);
  printf("mp2 %d\n", result);
  //
  done = mc1.Join(result);
  EXPECT_EQ(0, done);
  printf("mc1 %d\n", result);
  done = mc2.Join(result);
  EXPECT_EQ(0, done);
  printf("mc2 %d\n", result);
  //
}
