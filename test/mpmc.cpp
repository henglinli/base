// -*-coding:utf-8-unix;-*-
#include "gtest/gtest.h"
#include "thread.hh"
#include "mpmc/queue.hh"
//
using namespace NAMESPACE;
//
class Session {
 public:
  Session(): _value(0) {}
  //
  int _value;
};
//
const size_t kSize(128);
//
TEST(mpmc, Pop) {
  mpmc::BoundedQ<Session, 1<<10 > q;
  auto p = q.Pop();
  EXPECT_EQ(nullptr, p);
  p = q.Pop();
  EXPECT_EQ(nullptr, p);
}
//
Session s[kSize];
//
TEST(mpmc, PushPop) {
  mpmc::BoundedQ<Session, 1<<10> q;
  Session* p(nullptr);
  //
  for (size_t i(0); i < sizeof(s)/sizeof(s[0]); ++i) {
    s[i]._value = i;
    q.Push(s+i);
  }
  //
  p = q.Pop();
  EXPECT_NE(nullptr, p);
  EXPECT_EQ(0, p->_value);
  //
  p = q.Pop();
  EXPECT_NE(nullptr, p);
  EXPECT_EQ(1, p->_value);
}
//
struct Google {};
//
mpmc::BoundedQ<Google, 1<<11> q;
//
struct Producer: public Thread<Producer> {
  int _sum;
  int _result;
  int _full;
  Google* _g;
  explicit Producer(size_t sum): _sum(sum), _result(0), _full(0), _g(nullptr) {
    _g = new Google[_sum];
  }
  //
  ~Producer() {
    if (_g) {
      delete[] _g;
    }
  }
  //
  auto Loop() -> int* {
    printf("Producer %p\n", &_result);
    auto ok(false);
    for (_result = 0; _result < _sum;) {
      ok = q.Push(_g + _result);
      if (not ok) {
        ++_full;
      } else {
        ++_result;
      }
    }
    printf("full=%d\n", _full);
    return &_result;
  }
  //
  DISALLOW_COPY_AND_ASSIGN(Producer);
};
//
struct Consumer: public Thread<Consumer> {
  int _sum;
  int _result;
  int _empty;
  Google* _g;
  explicit Consumer(size_t sum): _sum(sum), _result(0), _empty(0), _g(nullptr) {}
  //
  auto Loop() -> int* {
    printf("Producer %p\n", &_result);
    for (_result = 0; _result < _sum;) {
      _g = q.Pop();
      if(_g) {
        ++_result;
      } else {
        ++_empty;
      }
    }
    printf("empty=%d\n", _empty);
    return &_result;
  }
  //
  DISALLOW_COPY_AND_ASSIGN(Consumer);
};
//
TEST(mpmc, full) {
  Consumer c1(1000*10), c2(1000*13);
  Producer p1(1000*11), p2(1000*12);
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
