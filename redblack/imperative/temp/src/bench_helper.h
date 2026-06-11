#include "rbt.hpp"
#include <vector>

using Tree = rbt::Tree<int32_t>;
using Node = rbt::Node<int32_t>;

Tree* makeUnbalanced(int depth);
Tree* makeBalanced(int depth);

std::vector<int> getFullInserts(int n);
std::vector<int> getFullDeletes(int n);
