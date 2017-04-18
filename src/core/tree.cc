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



void RegTree::GrowNode(MatrixPtr batch_ptr, node cur_node) {
  if (cur_node.col_id >= MAX_NODE_SIZE) return;
  if (cur_node.high - cur_node.low <= MIN_SAMPLENUM_SPLIT
      || cur_node.row_id*2 >= MAX_NODE_SIZE) {
      split_value_.SetValue(cur_node.row_id, cur_node.col_id,
        {.v=batch_ptr->ColMean(0, cur_node.low, cur_node.high)});
      split_fea_.SetValue(cur_node.row_id, cur_node.col_id, {.cls = 0});
      return;
  }

  float best_sse = 0.0;
  size_t best_fea = 0;
  Value best_split_value;
  Matrix& batch = (*batch_ptr);
  for (size_t fea_id = 1; fea_id < batch.GetWidth(); ++fea_id) {
    batch.Sort(fea_id, cur_node.low, cur_node.high);
    if (batch.fea_type(fea_id) == FeaType::DISC) {
      std::vector<size_t> counts;
      std::vector<float> sums;
      std::vector<float> sss;
      std::vector<size_t> splits;
      float count = 1;
      float sum = batch(0, 0);
      float sss = sum*sum;
      size_t split = batch(0, fea_id).cls;
      for (size_t cur_row = low+1; cur_row < high; ++cur_row) {
        if (split != batch(cur_row, fea_id).cls) {
          counts.push_back(count);
          sums.push_back(sum);
          sss.push_back(sss);
          splits.push_back(split);
          count = 0;
          sum = 0.0;
          sss = 0.0;
        }
        count += 1;
        sum += batch(cur_row, 0).v;
        sss += batch(cur_row, 0).v * batch(cur_row, 0).v;
        split = batch(cur_row, fea_id).cls;
      }
      if (count != 0) {
          counts.push_back(count);
          sums.push_back(sum);
          sss.push_back(sss);
          splits.push_back(split);
      }
      float total_sum = 0.0;
      float total_ss = 0.0;
      size_t total_count = high - low;
      for (auto& elem : sums)
          total_sum += elem;
      for (auto& elem : sss)
          total_ss += elem;
      size_t disc_best_split = 0;
      float disc_best_sse = 0.0;
      for (size_t i = 0; i < counts.size(); ++i) {
        float tmp = sss[i] - counts[i]*(sums[i]*sums[i]/counts[i]/counts[i]);
        float tmp_ss = total_ss - sss[i];
        float tmp_mean = (total_sum - sums[i])/counts[i];
        tmp_best_sse += tmp - (total_count - counts[i]) * tmp_mean * tmp_mean;
        if (best_fea == 0 || tmp < best_sse) {
          best_sse = tmp;
          best_fea = fea_id;
          best_split_value.cls = splits[i];
        }
      }
    } else if (batch.fea_type(fea_id) == FeaType::CONT) {
      size_t split_row = cur_node.low + MIN_SAMPLENUM_SPLIT;
      while (split_row <= high) {
        float cur_sse = batch.SSE(cur_node.low, split_row) + batch.SSE(split_row, cur_node.high);
        if (best_fea == 0 || cur_sse < best_sse) {
          best_sse = cur_sse;
          best_fea = fea_id;
          best_split_value.v = batch(split_row, fea_id).v;
        }
        split_row += MIN_SAMPLENUM_SPLIT;
      }
    } else if (batch.fea_type(fea_id) == FeaType::RANK) {
      size_t split_row = cur_node.low;
      int cur_value = batch(split_row, fea_id).level;
      while (true) {
        while (cur_value == batch(split_row, fea_id).level) split_row++;
        float cur_sse = batch.SSE(cur_node.low, split_row) + batch.SSE(split_row, cur_node.high);
        if (best_fea == 0 || cur_sse < best_sse) {
           best_sse = cur_sse;
           best_fea = fea_id;
           best_split_value.level = cur_value;

        }
        if (split_row < high) {
        cur_value = batch(split_row, fea_id).level;
        }
      }
    }

  }// for
  float non_split_sse = batch.SSE(cur_node.low, cur_node.high);
  if (non_split_sse < best_sse) {
      split_value_.SetValue(cur_node.row_id, cur_node.col_id,
        {.v=batch_ptr->ColMean(0, cur_node.low, cur_node.high)});
      split_fea_.SetValue(cur_node.row_id, cur_node.col_id, {.cls = 0});
      return;
  }
  split_fea_.SetValue(cur_node.row_id, cur_node.col_id, {.cls = best_fea});
  split_value_.SetValue(cur_node.row_id, cur_node.col_id, best_split_value);

  batch.Sort(best_fea, cur_node.low, cur_node.high);
  size_t mid = low;
  while(batch(mid, fea_id) != best_split_value) mid++;
  node left = {.row_id=cur_node.row_id, .col_id=cur_node.col_id*2, .low = low, .high=mid};
  node right = {.row_id=cur_node.row_id, .col_id=cur_node.col_id*2+1, .low = mid, .high=high};
  GrowNode(batch_ptr, left);
  GrowNode(batch_ptr, right);
  return;
}

void RegTree::TrainOneTree(MatrixPtr batch_ptr, float w) {
  Add();
  size_t row_id = NumTrees()-1;
  node root = {.row_id=row_id, .col_id=1, .low = 0, .high=batch_ptr->GetHeight()};
  GrowNode(batch_ptr, root);
}

