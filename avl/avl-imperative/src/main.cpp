#include "avl.h"
#include <iostream>
#include <cassert>

int main() {
    Node* root = nullptr;

    // --- insert tests ---

    // ascending insert: triggers RR rotations
    root = insert(root, 10); assert(avl_inv(root)); assert(search_tree_inv(root));
    root = insert(root, 20); assert(avl_inv(root)); assert(search_tree_inv(root));
    root = insert(root, 30); assert(avl_inv(root)); assert(search_tree_inv(root)); // RR
    root = insert(root, 40); assert(avl_inv(root)); assert(search_tree_inv(root));
    root = insert(root, 50); assert(avl_inv(root)); assert(search_tree_inv(root)); // RR
    root = insert(root, 25); assert(avl_inv(root)); assert(search_tree_inv(root)); // RL
    root = insert(root, 5);  assert(avl_inv(root)); assert(search_tree_inv(root));
    root = insert(root, 3);  assert(avl_inv(root)); assert(search_tree_inv(root)); // LL
    root = insert(root, 7);  assert(avl_inv(root)); assert(search_tree_inv(root)); // LR

    // duplicate insert should be a no-op
    root = insert(root, 25); assert(avl_inv(root)); assert(search_tree_inv(root));

    std::cout << "inorder: ";
    inorder(root);
    std::cout << "\n";

    // --- search tests ---
    assert(search(root, 25));
    assert(search(root, 3));
    assert(!search(root, 99));
    assert(!search(root, 0));
    std::cout << "search ok\n";

    // --- delete tests ---

    // delete a leaf
    root = deleteNode(root, 3);  assert(avl_inv(root)); assert(search_tree_inv(root));
    // delete a node with one child
    root = deleteNode(root, 5);  assert(avl_inv(root)); assert(search_tree_inv(root));
    // delete a node with two children
    root = deleteNode(root, 20); assert(avl_inv(root)); assert(search_tree_inv(root));
    // delete root
    root = deleteNode(root, 30); assert(avl_inv(root)); assert(search_tree_inv(root));
    // delete non-existent (should be a no-op)
    root = deleteNode(root, 99); assert(avl_inv(root)); assert(search_tree_inv(root));

    std::cout << "after some deletes: ";
    inorder(root);
    std::cout << "\n";

    // delete all remaining elements
    root = deleteNode(root, 7);  assert(avl_inv(root)); assert(search_tree_inv(root));
    root = deleteNode(root, 10); assert(avl_inv(root)); assert(search_tree_inv(root));
    root = deleteNode(root, 25); assert(avl_inv(root)); assert(search_tree_inv(root));
    root = deleteNode(root, 40); assert(avl_inv(root)); assert(search_tree_inv(root));
    root = deleteNode(root, 50); assert(avl_inv(root)); assert(search_tree_inv(root));

    // tree should be empty now
    assert(root == nullptr);
    std::cout << "tree empty ok\n";

    std::cout << "all invariants ok\n";

    freeTree(root);
    return 0;
}
