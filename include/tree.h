#ifndef DABTREE_TREE_H_
#define DABTREE_TREE_H_

#include <vector>
#include <memory>
#include "data.h"
#include "matrix.h"

const size_t MAX_NODE_SIZE = 64;


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
            const std::vector<Value>& values) {
    split_fea_.Copy(row_id, feas);
    split_value_.Copy(row_id, values);
  }

  void SetType(const std::vector<FeaType>& types) {
    split_value_.SetType(types);
  }

private:
  Matrix split_fea_ = Matrix(MAX_NODE_SIZE, 1, FeaType::DISC);
  Matrix split_value_ = Matrix(MAX_NODE_SIZE, 1);

};


typedef std::shared_ptr<RegTree> RegTreePtr;


#endif  // DABTREE_TREE_H_
