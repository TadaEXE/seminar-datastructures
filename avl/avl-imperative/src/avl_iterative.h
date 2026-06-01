#include "avl.h"
#include <vector>

namespace avl {

class VectorNode {
    int height;
    int left;
    int right;
};

class AvlRec {
    public:
        AvlRec() { root = nullptr; };
        AvlRec(Node* root) { this->root = root; };
        void ins(int x);
        void del(int x);
        bool find(int x);
        Node* getRoot();

    private:
        Node* root;
        bool balance(Node* current, Node* parent);
};

class AvlVec {
    public:
        AvlVec() {};
        AvlVec(std::vector<VectorNode>* nodes) { this->nodes = *nodes; };
        void ins(int x);
        void del(int x);
        bool find(int x);

    private:
        std::vector<VectorNode> nodes;
        int size = 0;
        Node* root = nullptr;
        bool balance(VectorNode* current, VectorNode* parent);
        void compress();
};

}
