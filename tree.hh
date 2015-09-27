// -*-coding:utf-8-unix;-*-
#pragma once
#include <cstdint>
#include <cassert>
#define RB_COMPAT
#include "tree.h"

namespace base {
//
template<typename Value>
struct Node {
  rb_node(Value) _node;
};
//
template<typename Value>
class Tree {
 public:
  //
  typedef Tree<Value> This;
  //
  typedef rbt(Value) _Tree;
  //
  rb_gen(static, _, _Tree, Value, _node, Value::Compare);
  //
  Tree() {
    This::_new(&_tree);
  }
 protected:
 private:
  _Tree _tree;
};
//
}

