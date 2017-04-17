#include "tree.h"
#include <glog/logging.h>



bool RegTree::Predict(MatrixPtr batch_ptr, VectorPtr result_ptr) {
  size_t sample_num = batch_ptr->GetHeight();
  if (sample_num == 0)
    return true;
  if (result_ptr->empty()) {
      result_ptr->resize(sample_num, 0.0);
    }
  CHECK_EQ(sample_num, result_ptr->size()) << "Result size is different from sample size";
  auto& batch = *batch_ptr;
  auto& result = *result_ptr;
  for (size_t i = 0; i < sample_num; ++i) {
      for (size_t j = 0; j < NumTrees(); ++j) {
          size_t cur_node = 1;
          while (true) {
              CHECK_LT(cur_node, MAX_NODE_SIZE) << "Node is out of bound";
              if (split_fea(j, cur_node) == 0) {
                  result[i] += weight[j] * split_value(j, cur_node).v;
                  break;
                }
              size_t cur_fea = split_fea(j, cur_node);
              Value cur_value = split_value(j, cur_node);
              FeaType cur_type = split_type(j, cur_node);
              CHECK_EQ(cur_type, batch.fea_type(cur_fea)) << "types not match";
              cur_node = cur_node*2;
              if (cur_type == FeaType::CONT && batch(i, cur_fea).v >= cur_value.v) {
                  cur_node += 1;
                } else if (cur_type == FeaType::DISC && batch(i, cur_fea).cls != cur_value.cls) {
                  cur_node += 1;
                } else if (cur_type == FeaType::RANK && batch(i, cur_fea).level >= cur_value.level) {
                  cur_node += 1;
                }
            }
        }
    }
  return true;
}
