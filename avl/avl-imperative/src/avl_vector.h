#include "avl.h"
#include <iostream>
#include <vector>
#include "stdint.h"

class VectorNode {
    public:
        long key;
        int height;
        size_t left;
        size_t right;

        // Count every Node allocation, then delegate to the global allocator.
        // Used for the allocation benchmark.
        inline static long long bytes_allocated = 0;
};

namespace avl {

class AvlVec: public Avl {
    public:
        AvlVec() {};
        void ins(long x);
        void del(long x);
        bool find(long x);
        void free();
        void compareSizes();

        bool checkInv();
        void printLimit();

        std::vector<VectorNode>* getNodes();
        void setSize() { 
            this->size = this->nodes.size(); 
            this->root = 0;
        }

        long min();
        long max();

    private:
        std::vector<VectorNode> nodes;
        size_t size = 0;
        size_t root = SIZE_MAX;
        bool balance(size_t current, size_t parent);
        void compress();
        void addNode(long x);

        size_t balL1(size_t current);
        size_t balL2(size_t current);
        size_t balR1(size_t current);
        size_t balR2(size_t current);
};

}