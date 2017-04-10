#ifndef DABTREE_TREE_H_
#define DABTREE_TREE_H_

#include <vector>
#include "data.h"

const size_t MAX_NODE_SIZE = 64;

struct RegTree {
  size_t id;
  vector<size_t> split_id;
  vector<Value> split_value;
};

typedef std::vector<RegTree> VecTree;

class TreeWrapper {
public:
  VecTree tree;

  float Predict(SamplePtr sample);
};

#endif  // DABTREE_TREE_H_
