#include "avl.h"
#include <iostream>
#include <cassert>

int main() {
    Node* root = nullptr;

    // test inserts
    root = insert(root, 10);
    assert(avl_inv(root));
    assert(search_tree_inv(root));

    root = insert(root, 20);
    assert(avl_inv(root));
    assert(search_tree_inv(root));

    root = insert(root, 30); // triggers RR rotation
    assert(avl_inv(root));
    assert(search_tree_inv(root));

    root = insert(root, 40);
    assert(avl_inv(root));
    assert(search_tree_inv(root));

    root = insert(root, 50);
    assert(avl_inv(root));
    assert(search_tree_inv(root));

    root = insert(root, 25);
    assert(avl_inv(root));
    assert(search_tree_inv(root));

    std::cout << "inorder: ";
    inorder(root);
    std::cout << "\n";

    std::cout << "search 25: " << (search(root, 25) ? "found" : "not found") << "\n";
    std::cout << "search 99: " << (search(root, 99) ? "found" : "not found") << "\n";

    // test deletes
    root = deleteNode(root, 30);
    assert(avl_inv(root));
    assert(search_tree_inv(root));
    std::cout << "after deleting 30: ";
    inorder(root);
    std::cout << "\n";

    root = deleteNode(root, 20);
    assert(avl_inv(root));
    assert(search_tree_inv(root));
    std::cout << "after deleting 20: ";
    inorder(root);
    std::cout << "\n";

    std::cout << "all invariants ok\n";

    freeTree(root);
    return 0;
}
