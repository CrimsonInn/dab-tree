#include "tree.h"
#include <glog/logging.h>



bool TreePredict(RegTreePtr tree, BatchPtr batch, VectorPtr result) {
  if (batch->samples.empty())
    return true;
  size_t sample_num = batch->samples.size();
  if (result->empty()) {
    result->resize(sample_num, 0.0);
  } else {
    CHECK_EQ(sample_num, result->size()) << "Result size is different from sample size";
  }
  for (size_t i = 0; i < sample_num; ++i) {
    auto& sample = batch->samples[i];
    size_t cur_node = 1;
    while (true) {
      CHECK_LT(cur_node, tree->split_fea.size()) << "Node is out of bound";
      if (tree->is_leaf[cur_node]) {
        (*result)[i] += tree->weight * tree->split_value[cur_node].v;
        break;
      }
      auto& fea_id = tree->split_fea[cur_node];
      auto& fea_type = tree->fea_types[cur_node];
      auto& split = tree->split_value[cur_node];
      CHECK_EQ(fea_type, batch->fea_types[fea_id]) << "types not match";
      cur_node = cur_node*2;
      if (fea_type == FeaType::CONT && sample[fea_id].v >= split.v) {
          cur_node += 1;
      } else if (fea_type == FeaType::DISC && sample[fea_id].cls != split.cls) {
          cur_node += 1;
      } else if (fea_type == FeaType::RANK && sample[fea_id].n >= split.n) {
          cur_node += 1;
      }
    }
  }
  return true;
}
