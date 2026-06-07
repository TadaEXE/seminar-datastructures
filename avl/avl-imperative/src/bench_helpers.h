#include "avl.h"
#include "avl_iterative.h"
#include "avl_compact.h"
#include "avl_vector.h"
#include <vector>
using namespace avl;

Node* buildUnbalancedBasic(int step, int depth);
uint64_t buildUnbalancedCompact(int step, int depth);
int buildUnbalancedVector(int step, int depth, std::vector<VectorNode> *vector);

Node* buildBalancedBasic(int minKey, int step, int depth);
uint64_t buildBalancedCompact(int minKey, int step, int depth);
int buildBalancedVector(int minKey, int step, int depth, std::vector<VectorNode> *vector);

Avl* makeUnbalanced(int depth, int version);
Avl* makeBalanced(int depth, int version);

std::vector<int> getFullInserts(int n);
std::vector<int> getFullDeletes(int n);
