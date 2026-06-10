#pragma once
#include <utility>
#include <climits>
#include <new>

struct Node {
    int key;    // the value of the node
    int height;
    Node* left;
    Node* right;

    Node(int k) : key(k), height(1), left(nullptr), right(nullptr) {}
    Node() : key(0), height(0), left(nullptr), right(nullptr) {}
    inline static long long bytes_allocated = 0;

    // Count every Node allocation, then delegate to the global allocator.
    // We don't count deallocations since we only care about the total memory used at the end of the benchmark.
    static void* operator new(std::size_t size) {
        bytes_allocated += static_cast<long long>(size);
        return ::operator new(size);
    }
    // Free memory normally. We do not decrease bytes_allocated because
    // we measure total allocations, not currently live memory.
    static void operator delete(void* ptr) noexcept {
        ::operator delete(ptr);
    }
};

class Avl {
    public:
        virtual void ins(int x) = 0;
        virtual void del(int x) = 0;
        virtual bool find(int x) = 0;
        virtual bool checkInv() = 0;
        virtual void free() = 0;
        virtual int min() = 0;
        virtual int max() = 0;
};

class AvlRec : public Avl {
    public:
        AvlRec(Node* root) { this->root = root; }
        AvlRec() { this->root = nullptr; }
        void ins(int x);
        void del(int x);
        bool find(int x);
        bool checkInv();
        void free();
        int min();
        int max();

    private:
        Node* root;

        Node* insert(Node* n, int key);
        Node* deleteNode(Node* n, int key);
        Node* balL(Node* z);
        Node* balR(Node* z);
        bool search(Node* n, int x);

        std::pair<Node*, int> split_max(Node* t);
};

void freeTree(Node* n);
void inorder();
int height(Node* n);
void updateHeight(Node* n);

// invariant checks
bool avl_inv(Node* n);
bool search_tree_inv(Node* n, long long min = LLONG_MIN, long long max = LLONG_MAX);
