// -*-coding:utf-8-unix;-*-
#include "gtest/gtest.h"
#include "netpoll.hh"
#include "linux/epoll.hh"
//
using namespace NAMESPACE;
//
TEST(netpoll, api) {
  Epoll epoll;
  int fd = NetPoller::Init(epoll);
  EXPECT_LT(0, fd);
  fd = 1;
  int done = NetPoller::Open(epoll, fd, &epoll);
  EXPECT_EQ(0, done);
  done = NetPoller::Poll(epoll, false);
  EXPECT_EQ(0, done);
  done = NetPoller::Close(epoll, fd);
  EXPECT_EQ(0, done);
}
//
