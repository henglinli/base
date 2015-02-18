// -*-coding:utf-8-unix;-*-
#pragma once

#include <stddef.h> // for NULL

class SplayEntryInterface {
  virtual int Compare(SplayEntryInterface * other) = 0;
  virtual ~SplayEntryInterface();
};

namespace base {
template <typename Node>
  class SplayEntry {
 public:
  template<typename>
  friend class SplayTree;

  SplayEntry()
      : left_(NULL)
      , right_(NULL) {
  }
  
  Node * Left() {
    return left_;
  }

  Node * Right() {
    return right_;
  }

 private:
  Node * left_;
  Node * right_;
}; // class SplayEntry

template <typename Node>
class SplayTree {
 public:
  enum {
    kNegInf = -1,
    kZero,
    kInf
  };

  bool Empty() {
    return (NULL == root_);
  }

  Node * Find(Node * elm) {
    if (NULL != root_) {
      Splay(elm);
      if (kZero == elm->Compare(root_)) {
        return root_;
      }
    }
    return NULL;
  }

  Node * Next(Node * elm) {
    Splay(elm);
    if (NULL != elm->right_) {
      elm = elm->right_;
      while (NULL != elm->left_) {
        elm = elm->left_;
      }
    } else {
      elm = NULL;
    }
    return elm;
  }

  Node * Min() {
    if (NULL != root_) {
      MinMax(kNegInf);
      return root_;
    }
    return NULL;
  }

  Node * Max() {
    if (NULL != root_) {
      MinMax(kInf);
      return root_;
    }
    return NULL;
  }

  Node * Insert(Node * elm) {
    if (NULL != root_) {
      Splay(elm);
      int comp = elm->Compare(root_);
      if (kZero > comp) {
        elm->left_ = root_->left_;
        elm->right_ = root_;
        root_->left_ = NULL;
      } else if (kZero < comp) {
        elm->right_ = root_->right_;
        elm->left_ = root_;
        root_->right_ = NULL;
      } else {
        return root_;
      }
    } else {
      elm->left_ = elm->right_ = NULL;
    }
    root_ = elm;
    return NULL;
  }

  Node * Remove(Node * elm) {
    Node * tmp(NULL);
    if (NULL != root_) {
      Splay(elm);
      if (kZero == elm->Compare(root_)) {
        if (NULL != root_->left_) {
          tmp = root_->right_;
          root_ = root_->left_;
          Splay(elm);
          root_->right_ = tmp;
        } else {
          root_ = root_->right_;
        }
        return elm;
      }
    }
    return NULL;
  }

 protected:
  void RotateRight(Node * tmp) {
    root_->left_ = tmp->right_;
    tmp->right_ = root_;
    root_ = tmp;
  }

  void RotateLeft(Node * tmp) {
    root_->right_ = tmp->left_;
    tmp->left_ = root_;
    root_ = tmp;
  }

  void LinkLeft(Node * tmp) {
    tmp->left_ = root_;
    tmp = root_;
    root_ = root_->left_;
  }

  void LinkRight(Node * tmp) {
    tmp->right_ = root_;
    tmp = root_;
    root_ = root_->right_;
  }

  void Assemble(Node * node, Node * left, Node * right) {
    left->right_ = root_->left_;
    right->left_ = root_->right_;
    root_->left_ = node->right_;
    root_->right_ = node->left_;
  }

  void Splay(Node * elm) {
    SplayEntry<Node> node;
    Node * left(static_cast<Node *>(&node)),
        * right(static_cast<Node *>(&node)),
        * tmp(static_cast<Node *>(&node));
    int comp(kZero);
    while ((comp = elm->Compare(root_))) {
      if (kZero > comp) {
        tmp = root_->left_;
        if (NULL == tmp) {
          break;
        }
        if (kZero > elm->Compare(tmp)) {
          RotateRight(tmp);
          if (NULL == root_->left_) {
            break;
          }
        }
        LinkLeft(right);
      } else if (kZero < comp) {
        tmp = root_->right_;
        if (NULL == tmp) {
          break;
        }
        if (kZero < elm->Compare(tmp)) {
          RotateLeft(tmp);
          if (NULL == root_->right_) {
            break;
          }
        }
        LinkRight(left);
      }
    }
    Assemble(static_cast<Node *>(&node), left, right);
  }

  void MinMax(int comp) {
    SplayEntry<Node> node;
     Node * left(static_cast<Node *>(&node)),
        * right(static_cast<Node *>(&node)),
        * tmp(static_cast<Node *>(&node));
     while (true) {
      if (kZero > comp) {
        tmp = root_->left_;
        if (NULL == tmp) {
          break;
        }
        if (kZero > comp) {
          RotateRight(tmp);
          if (NULL == root_->left_) {
            break;
          }
        }
        LinkLeft(right);
      } else if (kZero < comp) {
        tmp = root_->right_;
        if (NULL == tmp) {
          break;
        }
        if (kZero < tmp) {
          RotateLeft(tmp);
          if (NULL == root_->right_) {
            break;
          }
        }
        LinkRight(left);
      }
    }
    Assemble(&node, left, right);
  }

 private:
  Node * root_;
}; // class SplayTree
} // namespace base
