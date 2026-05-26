#include "avl.h"
#include <iostream>
#include <cassert>

int main() {
    Node* root = nullptr; // start with an empty tree

    // --- insert tests ---

    // insert some values
    root = insert(root, 10);
    root = insert(root, 20);
    root = insert(root, 30); // this should need a rotation

    assert(avl_inv(root));
    assert(search_tree_inv(root));

    root = insert(root, 40);
    root = insert(root, 50);
    root = insert(root, 25); // more rotations can happen here

    assert(avl_inv(root));
    assert(search_tree_inv(root));

    root = insert(root, 5);
    root = insert(root, 3);
    root = insert(root, 7);

    assert(avl_inv(root));
    assert(search_tree_inv(root));

    // inserting an existing value should not change the tree
    root = insert(root, 25);

    assert(avl_inv(root));
    assert(search_tree_inv(root));

    std::cout << "Tree after insertions: ";
    inorder(root);
    std::cout << "\n";

    // search test for existing and non-existing values
    assert(search(root, 25));
    assert(search(root, 3));
    assert(!search(root, 99));
    assert(!search(root, 0));

    std::cout << "Search tests passed\n";

    // delete tests
    // delete some values
    root = deleteNode(root, 3);
    root = deleteNode(root, 5);

    assert(avl_inv(root));
    assert(search_tree_inv(root));

    root = deleteNode(root, 20);
    root = deleteNode(root, 30);

    assert(avl_inv(root));
    assert(search_tree_inv(root));

    // deleting a value which is not in the tree should not break anything
    root = deleteNode(root, 99);

    assert(avl_inv(root));
    assert(search_tree_inv(root));

    std::cout << "Tree after some deletions: ";
    inorder(root);
    std::cout << "\n";

    // delete the remaining values
    root = deleteNode(root, 7);
    root = deleteNode(root, 10);
    root = deleteNode(root, 25);
    root = deleteNode(root, 40);
    root = deleteNode(root, 50);

    assert(avl_inv(root));
    assert(search_tree_inv(root));

    // now the tree should be empty
    assert(root == nullptr);

    std::cout << "Tree is empty now\n";
    std::cout << "All tests passed\n";

    freeTree(root);
    return 0;
}