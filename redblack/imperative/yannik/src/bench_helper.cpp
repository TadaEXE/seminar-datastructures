#include "bench_helper.h"



Tree* buildBalancedHelper(int minKey, int step, int depth) {
    if (depth <= 0) return nullptr;
    int rootKey = minKey + (int)(1LL << (depth - 1)) * step;
    auto *root = new Tree{ rootKey, Black };
    root->left  = buildBalancedHelper(minKey,   step, depth - 1);
    root->right = buildBalancedHelper(rootKey,  step, depth - 1);
    return root;
}

Tree* buildUnbalancedHelper(int& nextKey, int step, int depth, Color color) {
    if (depth <= 0) return nullptr;
    Color leftColor = color == Black ? Red : Black;
    int childDepth = color == Black ? depth - 1 : depth;
    Tree* left = buildUnbalancedHelper(nextKey, step, childDepth, leftColor);
    auto *root = new Tree{nextKey, color};
    nextKey += step;
    root->left  = left;
    //The right child is always black since it is the shorter path
    root->right = buildUnbalancedHelper(nextKey, step, childDepth, Black);
    return root;
}

void makeBalanced(int depth, Tree &tree) {
    auto root = buildBalancedHelper(0, 64, depth);
    tree.mark  = root->mark;
    tree.color = root->color;
    tree.left  = root->left;
    tree.right = root->right;
    delete root;
}

void makeUnbalanced(int depth, Tree &tree) {
    int nextKey = 0;
    auto root = buildUnbalancedHelper(nextKey, 64, depth, Black);
    tree.mark  = root->mark;
    tree.color = root->color;
    tree.left  = root->left;
    tree.right = root->right;
    delete root;
}

// write results in the same format as OCaml's plots.py data_2 dict
// second value is memory-related: C++ uses estimated bytes per operation.
void writeResult(const std::string& name, const std::vector<double>& times,
                 const std::vector<double>& memory, std::ofstream& out) {
    out << "\"" << name << "\":[";
    for (int i = 0; i < (int)times.size(); i++) {
        out << std::fixed << std::setprecision(9)
            << "(" << times[i] << ", " << memory[i] << ")";
        if (i + 1 < (int)times.size()) out << ", ";
    }
    out << "],\n";
    out.flush();
}

std::vector<Cmd> parseLine(const std::string& line) {
    std::vector<Cmd> cmds;
    int i = 0;
    while (i < (int)line.size()) {
        char type = line[i++];
        if (type != 'i' && type != 'd' && type != 'f') break;
        bool neg = (i < (int)line.size() && line[i] == '-');
        if (neg) i++;
        int key = 0;
        while (i < (int)line.size() && std::isdigit(line[i]))
            key = key * 10 + (line[i++] - '0');
        cmds.push_back({type, neg ? -key : key});
    }
    return cmds;
}


std::vector<long> getFullDeletes(long n) {
    std::srand(0);
    std::vector<long> inserts;
    std::vector<long> insertsTmp;
    std::vector<long> deletes;
    for (size_t i = 0; i < n; i++) {
        long random_value = std::rand() % (n * 20);
        inserts.push_back(random_value);
        insertsTmp.push_back(random_value);
    }
    for (size_t i = 0; i < insertsTmp.size(); i++) {
        long random_value = std::rand() % insertsTmp.size();
        deletes.push_back(insertsTmp[random_value]);
        insertsTmp.erase(insertsTmp.begin() + random_value);
        if (insertsTmp.empty()) break;
    }
    return deletes;
}

std::vector<long> getFullInserts(long n) {
    std::srand(0);
    std::vector<long> inserts;
    for (size_t i = 0; i < n; i++) {
        int random_value = std::rand() % (n * 20);
        inserts.push_back(random_value);
    }
    return inserts;
}


