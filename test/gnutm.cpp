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
struct Producer: public Thread<Producer> {
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
  auto Loop() -> int* {
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
struct Consumer: public Thread<Consumer> {
  int _sum;
  int _result;
  Google* _g;
  explicit Consumer(size_t sum): _sum(sum), _result(-1), _g(nullptr) {}
  //
  auto Loop() -> int* {
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
  Consumer c1(1000*10), c2(1000*11);
  Producer p1(1000*12), p2(1000*13);
  //
  int done(-1);
  done = Consumer::Run(c1);
  EXPECT_EQ(0, done);
  done = Consumer::Run(c2);
  EXPECT_EQ(0, done);
  //
  done = Producer::Run(p1);
  EXPECT_EQ(0, done);
  done = Producer::Run(p2);
  //
  int result(0);
  done = p1.Join(result);
  EXPECT_EQ(0, done);
  printf("mp1 %d\n", result);
  done = p2.Join(result);
  EXPECT_EQ(0, done);
  printf("mp2 %d\n", result);
  //
  done = c1.Join(result);
  EXPECT_EQ(0, done);
  printf("mc1 %d\n", result);
  done = c2.Join(result);
  EXPECT_EQ(0, done);
  printf("mc2 %d\n", result);
  //
}
