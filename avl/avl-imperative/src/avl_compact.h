#include "avl.h"
#include <cstdint>
#include <vector>

namespace avl {
/*
    Balance is encoded by bit-stuffing in the two least significant bits
    Left-heavy: 10
    Right-heavy: 01
    Balance: 00
*/

#define MASK_MAX (UINT64_MAX - 3)
#define BALANCE(raw) ((uint64_t) (raw) & 3)
#define SET_BALANCE(raw, bal) (((uint64_t) (raw) & MASK_MAX) | (bal))
#define CLEAN(raw) ((CompactNode*) ((raw) & MASK_MAX))

class CompactNode {
    public:
        int key;
        uint64_t left;
        uint64_t right; 
        CompactNode(int key) {
            this->key = key;
            this->left = 0;
            this->right = 0;
        }
};

class AvlComp: public Avl {
    public:
        AvlComp() { root = 0; };
        AvlComp(uint64_t root, int height) { 
            this->root = root; 
            this->height = height;
        };
        void ins(int x);
        void del(int x);
        bool find(int x);
        CompactNode* getRoot();
        bool checkInv();
        void printTree();
        void free();

        int min();
        int max();

    private:
        uint64_t root;
        int height = 0;

        uint64_t balL1(uint64_t current);
        uint64_t balL2(uint64_t current);
        uint64_t balR1(uint64_t current);
        uint64_t balR2(uint64_t current);
        void freeRec(uint64_t current);
        int checkAndHeight(uint64_t current);
};

}