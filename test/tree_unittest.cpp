// -*-coding:utf-8-unix;-*-
#include "gtest/gtest.h"
#include "tree.hh"

class Session
    : public base::Node<Session> {
 public:
  void Set(int fd) {
    fd_ = fd;
  }

  static int Compare(Session* one, Session* another) {
    if (one->fd_ < another->fd_) {
      return -1;
    } else if (one->fd_ > another->fd_) {
      return 1;
    } else {
      return 0;
    }
  }

 private:
  int fd_;
};

TEST(tree, basic) {
  base::Tree<Session> header;
  // header.Empty();
  Session sessions[10];
  for (size_t i(0); i < sizeof(sessions)/sizeof(sessions[0]) ; ++i) {
    sessions[i].Set(i);
    //header.Insert(sessions + i);
  }
  /*
  header.Empty();
  auto remove = header.Remove(sessions + 7);
  if (remove) {
    remove->Debug();
  }
  auto find = header.Find(sessions + 7);
  if (find) {
    find->Debug();
  }
  */
}

