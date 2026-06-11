#pragma once

#include <concepts>
#include <cstdint>
#include <utility>
#include <vector>
#include <cassert>
#include <iostream>

namespace rbt {

template <typename T>
concept NodeData = std::default_initializable<T> && requires(T a, T b) {
  { a == b } -> std::convertible_to<bool>;
  { a < b } -> std::convertible_to<bool>;
};

template <NodeData D> struct Node {
  enum class Color : uint8_t {
    Red,
    Black,
  };

  Node *parent = nullptr;
  Node *left = nullptr;
  Node *right = nullptr;
  D data{};
  Color color = Color::Red;
};

template <NodeData D> class Tree {
public:
  Node<D> _nil{};
  Node<D> *_root = nullptr;

  Tree() {
    _nil.color = Node<D>::Color::Black;
    _nil.parent = &_nil;
    _nil.left = &_nil;
    _nil.right = &_nil;

    _root = &_nil;
  }

  ~Tree() { destroy(_root); }

  Tree(const Tree &) = delete;
  Tree &operator=(const Tree &) = delete;

public:
  Node<D> *root() { return _root; }
  Node<D> *nil() { return &_nil; }

public:
  void free() {
    freeRec(_root);
  }

  bool insert(D data) {
    auto *node = new Node<D>{
        .parent = nullptr,
        .left = &_nil,
        .right = &_nil,
        .data = std::move(data),
        .color = Node<D>::Color::Red,
    };

    Node<D> *parent = nullptr;
    Node<D> *cur = _root;

    while (cur != &_nil) {
      parent = cur;

      if (node->data < cur->data) {
        cur = cur->left;
      } else if (cur->data < node->data) {
        cur = cur->right;
      } else {
        delete node;
        return false;
      }
    }

    node->parent = parent;

    if (!parent) {
      _root = node;
    } else if (node->data < parent->data) {
      parent->left = node;
    } else {
      parent->right = node;
    }

    insert_fix(*node);

    return true;
  }

  bool remove(const D &data) {
    auto *z = find_node(data);
    if (z == &_nil) {
      return false;
    }

    auto *y = z;
    auto y_original_color = y->color;

    Node<D> *x = nullptr;
    if (z->left == &_nil) {
      x = z->right;
      transplant(z, z->right);

    } else if (z->right == &_nil) {
      x = z->left;
      transplant(z, z->left);

    } else {
      y = minimum(z->right);
      y_original_color = y->color;

      x = y->right;

      if (y->parent == z) {
        x->parent = y;

      } else {
        transplant(y, y->right);

        y->right = z->right;
        y->right->parent = y;
      }

      transplant(z, y);

      y->left = z->left;
      y->left->parent = y;

      y->color = z->color;
    }
    if (data == 921407) std::cout << "A2!!!\n";

    delete z;

    if (y_original_color == Node<D>::Color::Black) {
      delete_fix(*x);
    }

    return true;
  }

  Node<D> *find(const D &data) {
    auto *node = find_node(data);

    if (node == &_nil) {
      return nullptr;
    }

    return node;
  }

  const Node<D> *find(const D &data) const {
    auto *node = find_node(data);

    if (node == &_nil) {
      return nullptr;
    }

    return node;
  }

  void inorder(Node<D> *node, std::vector<D> *out) {
    if (node == &_nil)
      return;
    inorder(node->left, out);
    if (node->data == 12470) std::cout << "Start: " << node << ", " << _root << ", " << _root->left << ", " << out->data() << ", " << out->size() << "\n";
    out->push_back(node->data);
    if (node->data == 12470) std::cout << "End:   " << node << ", " << _root << ", " << _root->left << ", " << out->data() << ", " << out->size() << "\n";
    inorder(node->right, out);
  }

  size_t black_height(Node<D> *node) {
    if (node == &_nil)
      return 1;

    auto left = black_height(node->left);

    auto right = black_height(node->right);

    assert(left == right);

    if (node->color == Node<D>::Color::Black) {
      return left + 1;
    }

    return left;
  }

private:
  void freeRec(Node<D>* root) {
    if (root == &_nil) return;
    freeRec(root->left);
    freeRec(root->right);
    delete root;
  }

  void rotate_left(Node<D> &root) {
    auto *pivot = root.right;

    root.right = pivot->left;

    if (pivot->left != &_nil) {
      pivot->left->parent = &root;
    }

    pivot->parent = root.parent;

    if (!root.parent) {
      _root = pivot;

    } else if (&root == root.parent->left) {
      root.parent->left = pivot;

    } else {
      root.parent->right = pivot;
    }

    pivot->left = &root;
    root.parent = pivot;
  }

  void rotate_right(Node<D> &root) {
    auto *pivot = root.left;

    root.left = pivot->right;

    if (pivot->right != &_nil) {
      pivot->right->parent = &root;
    }

    pivot->parent = root.parent;

    if (!root.parent) {
      _root = pivot;

    } else if (&root == root.parent->right) {
      root.parent->right = pivot;

    } else {
      root.parent->left = pivot;
    }

    pivot->right = &root;
    root.parent = pivot;
  }

  void insert_fix(Node<D> &node) {
    auto *n = &node;

    while (n->parent && n->parent->color == Node<D>::Color::Red) {
      auto *grand = n->parent->parent;

      if (n->parent == grand->left) {
        auto *unc = grand->right;

        if (unc->color == Node<D>::Color::Red) {
          n->parent->color = Node<D>::Color::Black;
          unc->color = Node<D>::Color::Black;
          grand->color = Node<D>::Color::Red;

          n = grand;

        } else {
          if (n == n->parent->right) {
            n = n->parent;
            rotate_left(*n);
          }

          n->parent->color = Node<D>::Color::Black;
          grand->color = Node<D>::Color::Red;

          rotate_right(*grand);
        }

      } else {
        auto *unc = grand->left;

        if (unc->color == Node<D>::Color::Red) {
          n->parent->color = Node<D>::Color::Black;
          unc->color = Node<D>::Color::Black;
          grand->color = Node<D>::Color::Red;

          n = grand;

        } else {
          if (n == n->parent->left) {
            n = n->parent;
            rotate_right(*n);
          }

          n->parent->color = Node<D>::Color::Black;
          grand->color = Node<D>::Color::Red;

          rotate_left(*grand);
        }
      }
    }

    _root->color = Node<D>::Color::Black;
  }

  void delete_fix(Node<D> &node) {
    auto *x = &node;

    while (x != _root && x->color == Node<D>::Color::Black) {
      if (x == x->parent->left) {
        auto *w = x->parent->right;

        if (w->color == Node<D>::Color::Red) {
          w->color = Node<D>::Color::Black;
          x->parent->color = Node<D>::Color::Red;

          rotate_left(*x->parent);

          w = x->parent->right;
        }

        if (w->left->color == Node<D>::Color::Black &&
            w->right->color == Node<D>::Color::Black) {
          w->color = Node<D>::Color::Red;
          x = x->parent;

        } else {
          if (w->right->color == Node<D>::Color::Black) {
            w->left->color = Node<D>::Color::Black;
            w->color = Node<D>::Color::Red;

            rotate_right(*w);

            w = x->parent->right;
          }

          w->color = x->parent->color;
          x->parent->color = Node<D>::Color::Black;
          w->right->color = Node<D>::Color::Black;

          rotate_left(*x->parent);

          x = _root;
        }

      } else {
        auto *w = x->parent->left;

        if (w->color == Node<D>::Color::Red) {
          w->color = Node<D>::Color::Black;
          x->parent->color = Node<D>::Color::Red;

          rotate_right(*x->parent);

          w = x->parent->left;
        }

        if (w->right->color == Node<D>::Color::Black &&
            w->left->color == Node<D>::Color::Black) {
          w->color = Node<D>::Color::Red;
          x = x->parent;

        } else {
          if (w->left->color == Node<D>::Color::Black) {
            w->right->color = Node<D>::Color::Black;
            w->color = Node<D>::Color::Red;

            rotate_left(*w);

            w = x->parent->left;
          }

          w->color = x->parent->color;
          x->parent->color = Node<D>::Color::Black;
          w->left->color = Node<D>::Color::Black;

          rotate_right(*x->parent);

          x = _root;
        }
      }
    }

    x->color = Node<D>::Color::Black;
  }

  void transplant(Node<D> *dst, Node<D> *src) {
    if (!dst->parent) {
      _root = src;

    } else if (dst == dst->parent->left) {
      dst->parent->left = src;

    } else {
      dst->parent->right = src;
    }

    src->parent = dst->parent;
  }

  Node<D> *minimum(Node<D> *node) const {
    while (node->left != &_nil) {
      node = node->left;
    }

    return node;
  }

  Node<D> *find_node(const D &data) const {
    auto *cur = _root;
    while (cur != &_nil) {
      if (data < cur->data) {
        cur = cur->left;

      } else if (cur->data < data) {
        cur = cur->right;

      } else {
        return cur;
      }
    }

    return const_cast<Node<D> *>(&_nil);
  }

  void destroy(Node<D> *node) {
    if (node == &_nil) {
      return;
    }

    destroy(node->left);
    destroy(node->right);

    delete node;
  }
};

} // namespace rbt
