#include "avl.h"
#include <vector>

class VectorNode {
    public:
        int key;
        int height;
        int left;
        int right;
};

namespace avl {

class AvlVec {
    public:
        AvlVec() {};
        AvlVec(std::vector<VectorNode>* nodes) { 
            this->nodes = *nodes; 
            this->size = nodes->size();
        };
        void ins(int x);
        void del(int x);
        bool find(int x);
        int balL1(int current);
        int balL2(int current);
        int balR1(int current);
        int balR2(int current);
        void compareSizes();

        bool checkInv();

    private:
        std::vector<VectorNode> nodes;
        int size = 0;
        int root = -1;
        bool balance(int current, int parent);
        void compress();
        void addNode(int x);
};

}