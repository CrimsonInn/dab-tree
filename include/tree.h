#ifndef DABTREE_TREE_H_
#define DABTREE_TREE_H_

#include <vector>
#include <memory>
#include "data.h"

const size_t MAX_NODE_SIZE = 64;

struct RegTree {

  size_t id;
  float weight;
  std::vector<size_t> split_fea;
  std::vector<Value> split_value;

};
typedef std::shared_ptr<RegTree> RegTreePtr;

typedef std::vector<RegTree> Ensemble;
typedef std::shared_ptr<Ensemble> EnsemblePtr;

float TreePredict(RegTreePtr tree, SamplePtr sample);
float EnsemblePredict(EnsemblePtr Ens, SamplePtr sample);

#endif  // DABTREE_TREE_H_
