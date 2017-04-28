#ifndef DABTREE_TREE_H_
#define DABTREE_TREE_H_

#include <vector>
#include <memory>
#include "random"
#include "data.h"
#include "message_tree.h"
#include "matrix.h"

#include <glog/logging.h>
#include <sstream> 
#include <string>

const size_t SAMPLENUM_SPLIT = 100;
const size_t THREAD_NUM = 4;


struct node {
  size_t row_id;
  size_t col_id;
  size_t low;
  size_t high;
};

/* Regression Tree
 * 1-based indexing
 * If node is leaf, split_value is the predicted value;
 * otherwise split_value is the split value at this node.
 */
class RegTree {
public:
  RegTree() {
    split_fea_ = std::make_shared<Matrix>();
    split_value_ = std::make_shared<Matrix>();
    split_fea_->Create(MAX_NODE_SIZE, 0, FeaType::DISC);
    split_value_->Create(MAX_NODE_SIZE, 0);
  }
  
  std::vector<float> weight;
  std::vector<size_t> id;
  
  bool Predict(MatrixPtr batch_ptr, VectorPtr result_ptr);
  void TrainOneTree(MatrixPtr batch_ptr);
  
  size_t NumTrees() {
    return split_value_->GetHeight();
  }
  
  size_t split_fea(size_t tree_id, size_t node_id) {
    return (*split_fea_)(tree_id, node_id).cls;
  }
  
  Value split_value(size_t tree_id, size_t node_id) {
    return (*split_value_)(tree_id, node_id);
  }
  
  FeaType split_type(size_t fea_id) {
    return split_value_->fea_type(fea_id);
  }
  
  void Copy(size_t row_id,
            const std::vector<Value>& feas,
            const std::vector<Value>& values,
            float w=1.0) {
    CHECK_LT(row_id, NumTrees()) << "(Tree) Copy: row_id out of bound";
    split_fea_->Copy(row_id, feas);
    split_value_->Copy(row_id, values);
    weight[row_id] = w;
  }
  
  void AddOneTree(const std::vector<Value>& feas,
                  const std::vector<Value>& values,
                  float w=1.0) {
    split_fea_->Add(feas);
    split_value_->Add(values);
    weight.push_back(w);
  }


  void Add() {
    split_fea_->Add(std::vector<Value>(MAX_NODE_SIZE, {.v=0.0}));
    split_value_->Add(std::vector<Value>(MAX_NODE_SIZE, {.v=0.0}));
    weight.push_back(1.0);
  }

  void Fill(int numTree){
    for (int i = this->NumTrees(); i<numTree; i++){
      split_fea_->AddZero();
      split_value_->AddZero();
      weight.push_back(1);
    }
  }

  void InitRandom(int id){
    this->Fill(id+1);
    
    std::random_device rd;
    std::mt19937 e2(rd());
    std::uniform_real_distribution<> dist(0, 10);

    std::vector<Value> fea(MAX_NODE_SIZE, {.v=0.0});
    std::vector<Value> value(MAX_NODE_SIZE, {.v=0.0});
    for (int i=0; i<MAX_NODE_SIZE; i++){
      fea[i].v=dist(e2);
      value[i].v=dist(e2);
    }
    split_fea_->Copy(id,fea);
    split_value_->Copy(id,value);
    weight[id]=1.0;
}


  void AddOneTree(float w=1.0) {
    split_fea_->AddOneRow(MAX_NODE_SIZE);
    split_value_->AddOneRow(MAX_NODE_SIZE);
    weight.push_back(w);

  }
  
  void SetType(const std::vector<FeaType>& types) {
    split_value_->SetType(types);
  }

  MessageTreePtr GetMessageTree(size_t id){
    MessageTreePtr message_tree ( new MessageTree(
          id,
          weight[id],
          split_fea_->Get(id),
          split_value_->Get(id)
        )
      );
    return message_tree;
  }

  MessageTree GetStackMessageTree(size_t id){
    MessageTree message_tree ( 
          id,
          weight[id],
          split_fea_->Get(id),
          split_value_->Get(id)
      );
    return message_tree;
  }

  std::string Print(size_t id){
      std::ostringstream s;

      s << "printing reg tree ---------------- \n";
      s << "id: " << id << "\n";
      s << "weight: " << weight[id] << "\n";
      s << "feas: ";
      for (int i=0; i<MAX_NODE_SIZE; i++){s << split_fea_->Get(id)[i].v<<", ";}
      s << "\n";
      s << "values: ";
      for (int i=0; i<MAX_NODE_SIZE; i++){s << split_value_->Get(id)[i].v<<", ";}
      s<< "\n done ---------------- \n \n";
      return s.str();
  }

  void Copy(MessageTreePtr tree_ptr) {
    this->Fill(tree_ptr->id+1);
    split_fea_->Copy(tree_ptr->id, *(new std::vector<Value>( std::begin(tree_ptr->feas), std::end(tree_ptr->feas))));
    split_value_->Copy(tree_ptr->id, *(new std::vector<Value>( std::begin(tree_ptr->values), std::end(tree_ptr->values))));
    weight[tree_ptr->id] = tree_ptr->weight;
  }

  void Copy(MessageTree tree){
    this->Fill(tree.id+1);
    split_fea_->Copy(tree.id, *(new std::vector<Value>( std::begin(tree.feas), std::end(tree.feas))));
    split_value_->Copy(tree.id, *(new std::vector<Value>( std::begin(tree.values), std::end(tree.values))));
    weight[tree.id] = tree.weight;
  }

  FeaType GetType(size_t fea_id) {
    return split_value_->fea_type(fea_id);
  }

  MatrixPtr GetSplitFea() {
    MatrixPtr sfptr = split_fea_;
    return sfptr;
  }
  
  MatrixPtr GetSplitValue() {
    MatrixPtr svptr = split_value_;
    return svptr;
  }
  
  void Print();
  void TrainOneTree(MatrixPtr batch_ptr, float w);
private:
  MatrixPtr split_fea_;
  MatrixPtr split_value_;
  void PrintOneTree(size_t tree_id, size_t start);
  void GrowNode(MatrixPtr batch_ptr, node cur_node);
};

typedef std::shared_ptr<RegTree> RegTreePtr;

#endif  // DABTREE_TREE_H_
