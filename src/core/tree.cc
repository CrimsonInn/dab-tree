#include "tree.h"
#include <glog/logging.h>

const size_t MIN_SAMPLENUM_SPLIT = 10;


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

struct node {
  size_t row_id;
  size_t col_id;
  size_t low;
  size_t high;
};

void RegTree::GrowNode(MatrixPtr batch_ptr, node cur_node,
                       size_t low, size_t high) {
  if (cur_node.col_id >= MAX_NODE_SIZE) return;
  if (high - low <= MIN_SAMPLENUM_SPLIT) {
      split_value_.SetValue(cur_node.row_id, cur_node.col_id, {.v=batch_ptr->ColMean(0, low, high)});
      split_fea_.SetValue(cur_node.row_id, cur_node.col_id, {.cls = 0});
      return;
  }

  float best_sse = 0.0;
  size_t best_fea = 0;
  for (size_t fea_id = 1; fea_id < batch_ptr->GetWidth(); ++fea_id) {
    batch_ptr->Sort(fea_id, cur_node.low, cur_node.high);

    if (batch_ptr->fea_type(fea_id) == FeaType::DISC) {
      std::vector<size_t> counts;
      std::vector<float> sss = {batch_ptr->};
      std::vector<float> sums;
      std::vector<size_t> splits;
      float count = 1;
      for (size_t cur_row = low+1; cur_row < high; ++cur_row) {

      }

    }

    size_t split_row = cur_node.low + MIN_SAMPLENUM_SPLIT;
    while (split_row <= high) {
      float cur_sse = batch_ptr->SSE(cur_node.low, split_row) + SSE(split_row, cur_node.high);
      if (best_fea == 0 || cur_sse < best_sse) {
        best_sse = cur_sse;
        best_fea = fea_id;
      }
    }

  }
}

void RegTree::TrainOneTree(MatrixPtr batch_ptr, float w) {
  std::vector<Value> feas;
  feas.resize(MAX_NODE_SIZE, {.cls=0});
  std::vector<Value> values;
  values.resize(MAX_NODE_SIZE, {.v=0.0});

  std::vector<node> non_terminal = {{.id=1, .low=0, .high=batch_ptr->GetHeight()}};
  while (!non_terminal.empty()) {
    auto cur_node = non_terminal.back();
    non_terminal.pop_back();
    for (size_t fea_id = 1; fea_id < batch_ptr->GetWidth(); ++fea_id) {
      batch_ptr->Sort(fea_id, cur_node.low, cur_node.high);
    }

  }
}

