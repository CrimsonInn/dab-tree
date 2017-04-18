#ifndef DABTREE_TREE_H_
#define DABTREE_TREE_H_

#include <vector>
#include <memory>
#include <cstdlib>
#include <ctime>
#include "data.h"
#include "message_tree.h"
#include "matrix.h"

//const size_t MAX_NODE_SIZE = 64;


/* Regression Tree
 * 1-based indexing
 * If node is leaf, split_value is the predicted value;
 * otherwise split_value is the split value at this node.
*/
class RegTree {
public:
  std::vector<float> weight = {1.0};
  std::vector<size_t> id = {1};
  bool Predict(MatrixPtr batch_ptr, VectorPtr result_ptr);

  size_t NumTrees() {
    return split_value_.GetHeight();
  }

  size_t split_fea(size_t tree_id, size_t node_id) {
    return split_fea_(tree_id, node_id).cls;
  }

  Value split_value(size_t tree_id, size_t node_id) {
    return split_value_(tree_id, node_id);
  }

  FeaType split_type(size_t tree_id, size_t node_id) {
    return split_value_.fea_type(node_id);
  }

  void Copy(size_t row_id,
            const std::vector<Value>& feas,
            const std::vector<Value>& values,
            float w=1.0) {
    split_fea_.Copy(row_id, feas);
    split_value_.Copy(row_id, values);
    weight[row_id] = w;
  }

  void Add(const std::vector<Value>& feas,
           const std::vector<Value>& values,
           float w=1.0) {
    split_fea_.Add(feas);
    split_value_.Add(values);
    weight.push_back(w);
  }

  void Add() {
    split_fea_.Add(std::vector<Value>(MAX_NODE_SIZE, {.v=0.0}));
    split_value_.Add(std::vector<Value>(MAX_NODE_SIZE, {.v=0.0}));
    weight.push_back(1.0);
  }

  void AddRandom(){
    srand(time(NULL));
    std::vector<Value> fea(MAX_NODE_SIZE, {.v=0.0});
    std::vector<Value> value(MAX_NODE_SIZE, {.v=0.0});
    for (int i=0; i<MAX_NODE_SIZE; i++){
      fea[i].v=std::rand();
      value[i].v=std::rand();
    }
    split_fea_.Add(fea);
    split_value_.Add(value);
    weight.push_back(1.0);
  }

  void SetType(const std::vector<FeaType>& types) {
    split_value_.SetType(types);
  }

  MessageTreePtr GetMessageTree(size_t id){
    MessageTreePtr message_tree ( new MessageTree(
          id,
          weight[id],
          split_fea_.Get(id),
          split_value_.Get(id)
        )
      );
    return message_tree;
  }

  void Print(size_t id){
      std::cout << "printing message tree ---------------- \n";
      std::cout << "id: " << id << "\n";
      std::cout << "weight: " << weight[id] << "\n";
      std::cout << "feas: ";
      for (int i=0; i<MAX_NODE_SIZE; i++){std::cout << split_fea_.Get(id)[i].v<<", ";}
      std::cout << "\n";
      std::cout << "values: ";
      for (int i=0; i<MAX_NODE_SIZE; i++){std::cout << split_value_.Get(id)[i].v<<", ";}
      std::cout << "\n done ---------------- \n \n";
  }

  void Copy(MessageTreePtr tree_ptr) {
    split_fea_.Copy(tree_ptr->id, *(new std::vector<Value>( std::begin(tree_ptr->feas), std::end(tree_ptr->feas))));
    split_value_.Copy(tree_ptr->id, *(new std::vector<Value>( std::begin(tree_ptr->values), std::end(tree_ptr->values))));
    weight[tree_ptr->id] = tree_ptr->weight;
  }


private:
  Matrix split_fea_ = Matrix(MAX_NODE_SIZE, 1, FeaType::DISC);
  Matrix split_value_ = Matrix(MAX_NODE_SIZE, 1);

};


typedef std::shared_ptr<RegTree> RegTreePtr;


#endif  // DABTREE_TREE_H_
