#include "rbt.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <set>

using Tree = rbt<int64_t>;
using Color = Color;

void makeUnbalanced(int depth, Tree &tree);
void makeBalanced(int depth, Tree &tree);

struct Cmd { char type; int key; };
std::vector<Cmd> parseLine(const std::string& line);
void writeResult(const std::string& name, const std::vector<double>& times,
                 const std::vector<double>& memory, std::ofstream& out);

std::vector<long> getFullInserts(long n);
std::vector<long> getFullDeletes(long n);
