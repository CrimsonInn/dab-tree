#ifndef DABTREE_TREE_H_
#define DABTREE_TREE_H_

#include <vector>
#include <memory>
#include "data.h"

const size_t MAX_NODE_SIZE = 4;

/* Regression Tree
 * 1-based indexing
 * If node is leaf, split_value is the predicted value;
 * otherwise split_value is the split value at this node.
*/
struct RegTree {

  size_t id;
  float weight;
  std::vector<FeaType> fea_types;
  std::vector<size_t> split_fea;
  std::vector<Value> split_value;
  std::vector<char> is_leaf;
  RegTree() {
    id = 0;
    weight = 1.0;
    fea_types.resize(MAX_NODE_SIZE, FeaType::CONT);
    split_fea.resize(MAX_NODE_SIZE, 0);
    split_value.resize(MAX_NODE_SIZE, {.v=0.0});
    is_leaf.resize(MAX_NODE_SIZE, 'a');
  }
  RegTree(size_t id, float weight, 
    std::vector<FeaType> &fea_types,
    std::vector<size_t> &split_fea,
    std::vector<Value> &split_value,
    std::vector<char> &is_leaf) {
    this->id = id;
    this->weight = weight;
    this->fea_types=fea_types;
    this->split_fea=split_fea;
    this->split_value=split_value;
    this->is_leaf=is_leaf;
  }
    void  Print(){
      std::cout << "printing reg tree ---------------- \n";
      std::cout << "id: " << id << "\n";
      std::cout << "weight: " << weight<< "\n";
      std::cout << "fea_types: ";
      for (int i=0; i<MAX_NODE_SIZE; i++){std::cout << fea_types[i]<<", ";}
      std::cout << "\n";
      std::cout << "split_fea: ";
      for (int i=0; i<MAX_NODE_SIZE; i++){std::cout << split_fea[i]<<", ";}
      std::cout << "\n";
      std::cout << "split_value: ";
      for (int i=0; i<MAX_NODE_SIZE; i++){std::cout << split_value[i].v<<", ";}
      std::cout << "\n";
      std::cout << "is_leaf: ";
      for (int i=0; i<MAX_NODE_SIZE; i++){std::cout << is_leaf[i]<<", ";}
      std::cout << "\n done ---------------- \n \n";

    }

};
typedef std::shared_ptr<RegTree> RegTreePtr;

typedef std::vector<RegTreePtr> Ensemble;
typedef std::shared_ptr<Ensemble> EnsemblePtr;

typedef std::shared_ptr<std::vector<float> > VectorPtr;
bool TreePredict(RegTreePtr tree, BatchPtr sample, VectorPtr result);
float EnsemblePredict(EnsemblePtr Ens, BatchPtr sample, VectorPtr result);

#endif  // DABTREE_TREE_H_
