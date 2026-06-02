#include "avl.h"
#include <vector>

class VectorNode {
    public:
        VectorNode(int key) { 
            this->key = key;
            height = 1;
            left = -1;
            right = -1;
        };
        int key;
        int height;
        int left;
        int right;
};

namespace avl {

class AvlVec {
    public:
        AvlVec() {};
        AvlVec(std::vector<VectorNode>* nodes) { this->nodes = *nodes; };
        void ins(int x);
        void del(int x);
        bool find(int x);
        int balL1(int current);
        int balL2(int current);
        int balR1(int current);
        int balR2(int current);

        bool checkInv();

    private:
        std::vector<VectorNode> nodes;
        int size = 0;
        int root = -1;
        bool balance(int current, int parent);
        void compress();
};

}