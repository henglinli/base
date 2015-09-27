// -*-coding:utf-8-unix;-*-
#pragma once
#define RB_COMPAT
#include "tree.h"

namespace base {
//
template<typename Value>
class RBNode {
 public:
 protected:
 private:
  rb_node(Value) _node;
};
//
template<typename Value>
class RBTree {
 public:
  typedef RBTree<Value> This;
  typedef rbt(Value) Tree;
  rb_gen(static, _, Tree, Value, _node, Value::Compare);
 protected:
 private:
};
//
}

