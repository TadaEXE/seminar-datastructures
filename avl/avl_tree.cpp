// backslashes in the ASCII diagrams below are intentional (tree branches),
// not line continuations — suppress the GCC warning
#pragma GCC diagnostic ignored "-Wcomment"

// AVL Tree — C++ version for my seminar
// mainly follows Chapter 9 of Nipkow's FDL book
// https://fdsa-book.net/functional_data_structures_algorithms.pdf
//
// In the book, AVL trees are BSTs where every node also stores
// the height of its subtree.  In Isabelle notation:
//   type_synonym 'a tree_ht = ('a × nat) tree
//
// One point from Section 9.2.2 that is easy to miss at first:
// balL and balR are reused for BOTH insert and delete, so I do not need
// a second set of balancing functions:
//   - balL(z): left side might be 2 taller  (after insert-left or delete-right)
//   - balR(z): right side might be 2 taller (after insert-right or delete-left)
//
// Corollary 9.3 gives the logarithmic height bound, so search/insert/delete
// are O(log n).

#include <iostream>
#include <algorithm>   // std::max
#include <cmath>       // std::abs
#include <climits>     // LLONG_MIN, LLONG_MAX
#include <utility>     // std::pair

// ------------------------------------------------------------
// Node: key + stored height + two child pointers.
// The book stores the height inside the pair (a, n).  Here I just keep
// it as the field ht.
// ------------------------------------------------------------
struct Node {
    int   key;
    int   ht;     // height of this subtree (0 means empty / nullptr)
    Node* left;
    Node* right;

    // new leaf always has height 1
    explicit Node(int k) : key(k), ht(1), left(nullptr), right(nullptr) {}
};

// ht(t) — height of t, 0 if null.
// Book (Chapter 9 intro):
//   ht ⟨⟩ = 0
//   ht ⟨_,(_,n),_⟩ = n
int ht(const Node* t) {
    return t ? t->ht : 0;
}

// Recalculate the height after changing child pointers.
// This is basically the height part of the book's "node" constructor:
//   node l a r = ⟨l, (a, max(ht l)(ht r) + 1), r⟩
void updateHeight(Node* z) {
    if (z)
        z->ht = 1 + std::max(ht(z->left), ht(z->right));
}

// node() is here mostly because the book uses this smart constructor a lot.
// My rotations below do the same job by rewiring existing nodes, so this helper
// is not used in the demo.  I kept it because it makes the connection to the
// functional version clearer.
Node* node(Node* l, int key, Node* r) {
    Node* z = new Node(key);
    z->left  = l;
    z->right = r;
    z->ht    = 1 + std::max(ht(l), ht(r));
    return z;
}

// ------------------------------------------------------------
// avl() checks the AVL height invariant from the start of Chapter 9.
//
// Book definition:
//   avl ⟨⟩ = True
//   avl ⟨l,(_,n),r⟩ = |ht l − ht r| ≤ 1  ∧  n = max(ht l)(ht r)+1  ∧  avl l  ∧  avl r
//
// Important: this is only the height/balance part.  It does not check whether
// the keys are ordered like a BST.  I check that separately below.
// ------------------------------------------------------------
bool avl(const Node* t) {
    if (!t) return true;
    int diff = ht(t->left) - ht(t->right);
    return std::abs(diff) <= 1
        && t->ht == 1 + std::max(ht(t->left), ht(t->right))
        && avl(t->left)
        && avl(t->right);
}

// ------------------------------------------------------------
// bst() checks the ordering property separately from avl().
// Every key in the left subtree must be < t->key,
// every key in the right subtree must be > t->key.
// I use long long bounds so INT_MIN and INT_MAX can still be valid keys.
// ------------------------------------------------------------
bool bst(const Node* t, long long lo = LLONG_MIN, long long hi = LLONG_MAX) {
    if (!t) return true;
    return t->key > lo
        && t->key < hi
        && bst(t->left,  lo,    t->key)
        && bst(t->right, t->key, hi);
}

// ============================================================
// balL and balR — based on Figures 9.1 and 9.2
//
// These are the main part of the AVL implementation.  Each function has
// a single-rotation case and a double-rotation case.  The same functions
// are also reused during deletion, as in Section 9.2.2.
// ============================================================

// ------------------------------------------------------------
// balL: used when the left side may be too tall.
//
// From Figure 9.1:
//   balL AB c C =
//     if ht AB = ht C + 2
//     then let ⟨A,(a,_),B⟩ = AB
//          in if ht B ≤ ht A        -- LL case: single right rotation
//             then node A a (node B c C)
//             else let ⟨B1,(b,_),B2⟩ = B  -- LR case: double rotation
//                  in node (node A a B1) b (node B2 c C)
//     else node AB c C
//
// In the code below, AB = z->left, c = z->key, C = z->right.
// The book builds new nodes; here I just change pointers.
//
// LL (single right rotation):
//
//       z(c)              AB(a)
//      /    \            /     \
//    AB(a)   C   →      A     z(c)
//    /  \                     /  \
//   A    B                   B    C
//
// LR (double rotation):
//
//       z(c)                  B(b)
//      /    \               /      \
//    AB(a)   C   →       AB(a)    z(c)
//    /  \                /  \     /  \
//   A   B(b)            A   B1  B2    C
//       / \
//      B1  B2
// ------------------------------------------------------------
Node* balL(Node* z) {
    Node* AB = z->left;
    Node* C  = z->right;

    if (ht(AB) != ht(C) + 2) {
        // no rotation needed, but the stored height may need updating
        updateHeight(z);
        return z;
    }

    // left side is 2 taller, so AB must exist
    Node* A = AB->left;
    Node* B = AB->right;

    if (ht(B) <= ht(A)) {
        // LL case — single right rotation
        // same shape as: node A a (node B c C)
        // AB becomes the new root; z moves down to the right
        AB->right = z;
        z->left   = B;
        // z->right = C  (stays the same)
        updateHeight(z);   // z is now lower, update it first
        updateHeight(AB);
        return AB;
    } else {
        // LR case — double rotation
        // B can't be null here because ht(B) > ht(A) >= 0
        // same shape as: node (node A a B1) b (node B2 c C)
        Node* B1 = B->left;
        Node* B2 = B->right;

        AB->right = B1;   // AB becomes ⟨A, a, B1⟩
        z->left   = B2;   // z  becomes ⟨B2, c, C⟩
        B->left   = AB;   // B is the new root
        B->right  = z;

        updateHeight(AB); // update children before parent
        updateHeight(z);
        updateHeight(B);
        return B;
    }
}

// ------------------------------------------------------------
// balR: the mirror version of balL, for when the right side may be too tall.
//
// From Figure 9.2:
//   balR A a BC =
//     if ht BC = ht A + 2
//     then let ⟨B,(c,_),C⟩ = BC
//          in if ht B ≤ ht C        -- RR case: single left rotation
//             then node (node A a B) c C
//             else let ⟨B1,(b,_),B2⟩ = B  -- RL case: double rotation
//                  in node (node A a B1) b (node B2 c C)
//     else node A a BC
//
// RR (single left rotation):
//
//    z(a)                  BC(c)
//   /    \               /      \
//  A    BC(c)   →      z(a)      C
//       /  \           /  \
//      B    C         A    B
//
// RL (double rotation):
//
//    z(a)                    B(b)
//   /    \                 /      \
//  A    BC(c)   →       z(a)    BC(c)
//       /  \            /  \    /  \
//     B(b)   C         A   B1  B2   C
//     / \
//    B1  B2
// ------------------------------------------------------------
Node* balR(Node* z) {
    Node* A  = z->left;
    Node* BC = z->right;

    if (ht(BC) != ht(A) + 2) {
        updateHeight(z);
        return z;
    }

    Node* B = BC->left;
    Node* C = BC->right;

    if (ht(B) <= ht(C)) {
        // RR case — single left rotation
        // same shape as: node (node A a B) c C
        BC->left = z;
        z->right = B;
        // z->left = A  (unchanged)
        updateHeight(z);
        updateHeight(BC);
        return BC;
    } else {
        // RL case — double rotation
        // B can't be null because ht(B) > ht(C) >= 0
        // same shape as: node (node A a B1) b (node B2 c C)
        Node* B1 = B->left;
        Node* B2 = B->right;

        z->right = B1;    // z  becomes ⟨A, a, B1⟩
        BC->left = B2;    // BC becomes ⟨B2, c, C⟩
        B->left  = z;     // B is the new root
        B->right = BC;

        updateHeight(z);
        updateHeight(BC);
        updateHeight(B);
        return B;
    }
}

// ------------------------------------------------------------
// isin: normal BST search (the book calls it 'isin').
// O(log n) because height is bounded by 1.44 * log2(n) (Corollary 9.3).
// ------------------------------------------------------------
bool isin(int x, const Node* t) {
    if (!t)          return false;
    if (x == t->key) return true;
    if (x  < t->key) return isin(x, t->left);
    else             return isin(x, t->right);
}

// ------------------------------------------------------------
// insert — Section 9.2.1
//
// Book:
//   insert x ⟨⟩ = ⟨⟨⟩,(x,1),⟨⟩⟩
//   insert x ⟨l,(a,n),r⟩ =
//     case cmp x a of
//       LT → balL (insert x l) a r
//       EQ → ⟨l,(a,n),r⟩
//       GT → balR l a (insert x r)
//
// After inserting on the left, call balL.  After inserting on the right,
// call balR.  Theorem 9.4 says avl is preserved and the height changes
// by at most 1.
// ------------------------------------------------------------
Node* insert(int x, Node* t) {
    if (!t)
        return new Node(x);  // leaf, height = 1

    if (x < t->key) {
        t->left  = insert(x, t->left);
        return balL(t);
    } else if (x > t->key) {
        t->right = insert(x, t->right);
        return balR(t);
    }
    return t;  // already in tree, ignore duplicate
}

// ------------------------------------------------------------
// split_max — helper for delete (Figure 9.3).
//
// Book:
//   split_max ⟨l,(a,_),r⟩ =
//     if r = ⟨⟩ then (l, a)
//     else let (r',a') = split_max r
//          in (balL l a r', a')
//
// This removes the largest key from a subtree and returns the remaining
// tree plus that key.  Since the removal happens on the right side,
// balL may be needed on the way back up.
// ------------------------------------------------------------
std::pair<Node*, int> split_max(Node* t) {
    // t is never null here — caller guarantees it
    if (!t->right) {
        // t is the max, detach it
        int   maxKey    = t->key;
        Node* remaining = t->left;
        delete t;
        return {remaining, maxKey};
    }
    auto [r_prime, maxKey] = split_max(t->right);
    t->right = r_prime;        // right child may have shrunk
    return {balL(t), maxKey};  // so left might be relatively taller now
}

// ------------------------------------------------------------
// deleteNode — Section 9.2.2, Figure 9.3.
//
// Book:
//   delete _ ⟨⟩ = ⟨⟩
//   delete x ⟨l,(a,_),r⟩ =
//     case cmp x a of
//       LT → balR (delete x l) a r
//       EQ → if l = ⟨⟩ then r
//             else let (l',a') = split_max l
//                  in balR l' a' r
//       GT → balL l a (delete x r)
//
// This is where Section 9.2.2 is useful: the same balL/balR functions
// from insertion are used again.  If the left child shrinks, call balR;
// if the right child shrinks, call balL.  Theorem 9.5 says avl is preserved
// and the height can drop by at most 1.
// ------------------------------------------------------------
Node* deleteNode(int x, Node* t) {
    if (!t) return nullptr;

    if (x < t->key) {
        t->left  = deleteNode(x, t->left);
        return balR(t);  // left shrank, right might be relatively taller
    } else if (x > t->key) {
        t->right = deleteNode(x, t->right);
        return balL(t);  // right shrank, left might be relatively taller
    }

    // found the node to delete
    if (!t->left) {
        Node* r = t->right;
        delete t;
        return r;  // no left child, just return right (or null)
    }

    // replace with in-order predecessor (max of left subtree)
    auto [newLeft, predKey] = split_max(t->left);
    t->key  = predKey;
    t->left = newLeft;
    return balR(t);  // left subtree lost a node → balR
}

// in-order walk (should print sorted if BST property holds)
void inorder(const Node* t) {
    if (!t) return;
    inorder(t->left);
    std::cout << t->key << " ";
    inorder(t->right);
}

// free the whole tree
void freeTree(Node* t) {
    if (!t) return;
    freeTree(t->left);
    freeTree(t->right);
    delete t;
}

// print sideways (right subtree on top) for a rough visual check
void printTree(const Node* t, int indent = 0) {
    if (!t) return;
    printTree(t->right, indent + 4);
    std::cout << std::string(indent, ' ') << t->key << "(h=" << t->ht << ")\n";
    printTree(t->left, indent + 4);
}

// small helper for the demo below
static void demo(const char* label, std::initializer_list<int> keys) {
    std::cout << "\n=== " << label << " ===\n";
    Node* root = nullptr;
    for (int k : keys) {
        root = insert(k, root);
        std::cout << "  insert(" << k << ")"
                  << "  ht=" << ht(root)
                  << "  avl=" << (avl(root) ? "ok" : "VIOLATED")
                  << "  bst=" << (bst(root) ? "ok" : "VIOLATED") << "\n";
    }
    std::cout << "  in-order: ";
    inorder(root);
    std::cout << "\n  tree:\n";
    printTree(root, 4);
    freeTree(root);
}

// ------------------------------------------------------------
// main: small demo for the four rotations, then deletion.
//
// Minimal sequences that force each case:
//   LL: 30,20,10  →  single right rotation
//   RR: 10,20,30  →  single left rotation
//   LR: 30,10,20  →  double rotation (left-right)
//   RL: 10,30,20  →  double rotation (right-left)
// ------------------------------------------------------------
int main() {
    demo("LL — single right rotation  (balL, ht(B)<=ht(A))", {30, 20, 10});
    demo("RR — single left rotation   (balR, ht(B)<=ht(C))", {10, 20, 30});
    demo("LR — double rotation        (balL, ht(B)> ht(A))", {30, 10, 20});
    demo("RL — double rotation        (balR, ht(B)> ht(C))", {10, 30, 20});

    // deletion test
    std::cout << "\n=== Deletion (split_max) ===\n";
    Node* root = nullptr;
    for (int k : {10, 20, 30, 40, 50, 25})
        root = insert(k, root);

    std::cout << "  tree before deletions:\n";
    printTree(root, 4);

    // deleting 30 has two children, so split_max finds 25 (max of left subtree)
    root = deleteNode(30, root);
    std::cout << "  after deleteNode(30): ";
    inorder(root);
    std::cout << "  avl=" << (avl(root) ? "ok" : "VIOLATED")
              << "  bst=" << (bst(root) ? "ok" : "VIOLATED") << "\n";

    root = deleteNode(20, root);
    std::cout << "  after deleteNode(20): ";
    inorder(root);
    std::cout << "  avl=" << (avl(root) ? "ok" : "VIOLATED")
              << "  bst=" << (bst(root) ? "ok" : "VIOLATED") << "\n";

    std::cout << "\n  isin(25): " << (isin(25, root) ? "found" : "not found") << "\n";
    std::cout << "  isin(99): " << (isin(99, root) ? "found" : "not found") << "\n";

    freeTree(root);
    return 0;
}
