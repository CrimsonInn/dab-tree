#include "tree.h"
#include <glog/logging.h>

const size_t MIN_SAMPLENUM_SPLIT = 2;


bool RegTree::Predict(MatrixPtr batch_ptr, VectorPtr result_ptr) {
  size_t sample_num = batch_ptr->GetHeight();
  if (sample_num == 0)
    return true;
  result_ptr->resize(sample_num, 0.0);

  if (NumTrees() == 0) {
      std::fill(result_ptr->begin(), result_ptr->end(), 0.0);
      return true;
  }

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
              FeaType cur_type = split_type(cur_fea);
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
      || cur_node.col_id*2 >= MAX_NODE_SIZE) {
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
          float sum = batch(0, 0).v;
          float ss = sum*sum;
          size_t split = batch(0, fea_id).cls;
          for (size_t cur_row = cur_node.low+1; cur_row < cur_node.high; ++cur_row) {
              if (split != batch(cur_row, fea_id).cls) {
                  counts.push_back(count);
                  sums.push_back(sum);
                  sss.push_back(ss);
                  splits.push_back(split);
                  count = 0;
                  sum = 0.0;
                  ss = 0.0;
                }
              count += 1;
              sum += batch(cur_row, 0).v;
              ss += batch(cur_row, 0).v * batch(cur_row, 0).v;
              split = batch(cur_row, fea_id).cls;
            }
          if (count != 0) {
              counts.push_back(count);
              sums.push_back(sum);
              sss.push_back(ss);
              splits.push_back(split);
            }
          float total_sum = 0.0;
          float total_ss = 0.0;
          size_t total_count = cur_node.high - cur_node.low;
          for (auto& elem : sums)
            total_sum += elem;
          for (auto& elem : sss)
            total_ss += elem;

          for (size_t i = 0; i < counts.size(); ++i) {
              float tmp = sss[i] - counts[i]*(sums[i]*sums[i]/counts[i]/counts[i]);
              float tmp_ss = total_ss - sss[i];
              float tmp_mean = (total_sum - sums[i])/counts[i];
              tmp += tmp_ss - (total_count - counts[i]) * tmp_mean * tmp_mean;
              if (best_fea == 0 || tmp < best_sse) {
                  best_sse = tmp;
                  best_fea = fea_id;
                  best_split_value.cls = splits[i];
                }
            }
        } else if (batch.fea_type(fea_id) == FeaType::CONT) {
          size_t split_row = cur_node.low + MIN_SAMPLENUM_SPLIT;
          while (split_row < cur_node.high) {
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
              while (split_row < cur_node.high && cur_value == batch(split_row, fea_id).level) split_row++;
              if (split_row == cur_node.high) break;
              cur_value = batch(split_row, fea_id).level;
              float cur_sse = batch.SSE(cur_node.low, split_row) + batch.SSE(split_row, cur_node.high);
              if (best_fea == 0 || cur_sse < best_sse) {
                  best_sse = cur_sse;
                  best_fea = fea_id;
                  best_split_value.level = cur_value;
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

  size_t mid = cur_node.low;
  if (batch.fea_type(best_fea) == FeaType::CONT) {
      batch.Sort(best_fea, cur_node.low, cur_node.high);
      while(mid < cur_node.high && batch(mid, best_fea).v < best_split_value.v) mid++;
    } else if (batch.fea_type(best_fea) == FeaType::DISC) {
      mid = batch.Split(best_fea, cur_node.low, cur_node.high, best_split_value.cls);
    } else if (batch.fea_type(best_fea) == FeaType::RANK) {
      batch.Sort(best_fea, cur_node.low, cur_node.high);
      while(batch(mid, best_fea).level < best_split_value.level) mid++;
    }
  node left = {.row_id=cur_node.row_id, .col_id=cur_node.col_id*2, .low = cur_node.low, .high=mid};
  node right = {.row_id=cur_node.row_id, .col_id=cur_node.col_id*2+1, .low = mid, .high=cur_node.high};
  GrowNode(batch_ptr, left);
  GrowNode(batch_ptr, right);
  return;
}

void RegTree::TrainOneTree(MatrixPtr batch_ptr, float weight) {
  if (split_value_.Empty()) {
      split_value_.SetType(batch_ptr->fea_types());
    } else {
      for (size_t i = 0; i < batch_ptr->GetWidth(); ++i) {
        CHECK_EQ(batch_ptr->fea_type(i), split_type(i));
        }
    }
  AddOneTree(weight);
  size_t row_id = NumTrees()-1;
  node root = {.row_id=row_id, .col_id=1, .low = 0, .high=batch_ptr->GetHeight()};
  GrowNode(batch_ptr, root);
}



void RegTree::PrintOneTree(size_t tree_id, size_t start) {
  if (split_fea(tree_id, start) == 0) {
      printf("{%lu, %f}", split_fea(tree_id, start), split_value(tree_id, start).v);
      return;
    }
  auto type = GetType(split_fea(tree_id, start));
  if (type == FeaType::CONT)
    printf("{%lu, %f}, [", split_fea(tree_id, start), split_value(tree_id, start).v);
  else if (type == FeaType::DISC)
    printf("{%lu, %lu}, [", split_fea(tree_id, start), split_value(tree_id, start).cls);
  else if (type == FeaType::RANK)
    printf("{%lu, %d}, [", split_fea(tree_id, start), split_value(tree_id, start).level);
  PrintOneTree(tree_id, start*2);
  printf(",");
  PrintOneTree(tree_id, start*2+1);
  printf("]");
}

void RegTree::Print() {
  for (size_t i = 0; i < NumTrees(); ++i) {
      printf("%f,", weight[i]);
      PrintOneTree(i, 1);
      printf("\n");
    }
}

