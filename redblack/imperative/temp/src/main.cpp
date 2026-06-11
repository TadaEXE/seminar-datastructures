#include <cassert>
#include <cstdint>
#include <random>
#include <set>
#include <vector>
#include <iostream>

#include "rbt.hpp"

using Tree = rbt::Tree<int32_t>;
using Node = rbt::Node<int32_t>;

void validate_red_rule(Node *node, Node *nil) {
  if (node == nil)
    return;

  if (node->color == Node::Color::Red) {
    assert(node->left->color == Node::Color::Black);

    assert(node->right->color == Node::Color::Black);
  }

  validate_red_rule(node->left, nil);

  validate_red_rule(node->right, nil);
}

void validate(Tree &tree) {
  auto root = tree.root();
  auto nil = tree.nil();

  if (root == nil)
    return;

  assert(root->color == Node::Color::Black);

  validate_red_rule(root, nil);

  tree.black_height(root);

  std::vector<int> values;

  tree.inorder(root, values);

  for (size_t i = 1; i < values.size(); ++i) {
    assert(values[i - 1] < values[i]);
  }
}

int main() {
  Tree tree;

  assert(tree.insert(10));
  assert(tree.insert(20));
  assert(tree.insert(30));
  assert(tree.insert(5));
  assert(tree.insert(1));
  validate(tree);
  std::cout << "Inserts done";

  assert(tree.find(20));
  assert(!tree.find(999));
  std::cout << "finds done";

  assert(!tree.insert(20));
  tree.remove(20);
  tree.remove(5);
  validate(tree);
  std::cout << "removes done";

  std::mt19937 rng(42069);

  std::uniform_int_distribution d(0, 10000);

  std::set<int32_t> ref;

  for (int i = 0; i < 100000; ++i) {
    int x = d(rng);

    if (rng() % 2) {
      tree.insert(x);
      ref.insert(x);

    } else {
      tree.remove(x);
      ref.erase(x);
    }

    validate(tree);

    for (auto v : ref) {
      assert(tree.find(v));
    }
  }
  validate(tree);
  std::cout << "random usage done";
}
