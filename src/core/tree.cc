#include "tree.h"
#include <algorithm>
#include <glog/logging.h>
#include "thread"
#include <mutex>

bool RegTree::Predict(MatrixPtr batch_ptr, VectorPtr result_ptr) {
  size_t sample_num = batch_ptr->GetHeight();
  if (sample_num == 0)
    return true;
  result_ptr->resize(sample_num, 0.0);
  std::fill(result_ptr->begin(), result_ptr->end(), 0.0);
  if (NumTrees() == 0) {
    return true;
  }
  
  auto& batch = *batch_ptr;
  
  std::vector<std::thread> threads;
  auto fea_types = split_value_->fea_types();
  
  for (size_t tid = 0; tid < THREAD_NUM; tid++) {
    threads.emplace_back(std::thread([&, this, tid](){
      for (size_t i = tid; i < sample_num; i += THREAD_NUM) {
        for (size_t j = 0; j < NumTrees(); ++j) {
          size_t cur_node = 1;
          while (true) {
            size_t cur_fea = split_fea(j, cur_node);
            Value cur_value = split_value(j, cur_node);
            if (cur_fea == 0) {
              (*result_ptr)[i] += weight[j] * cur_value.v;
              break;
            }
            FeaType cur_type = fea_types[cur_fea];
            CHECK_EQ(cur_type, batch.fea_type(cur_fea)) << "types not match";
            Value sample_value = batch(i, cur_fea);
            cur_node = cur_node*2;
            if (cur_type == FeaType::CONT && sample_value.v >= cur_value.v) {
              cur_node += 1;
            } else if (cur_type == FeaType::DISC && sample_value.cls == cur_value.cls) {
              cur_node += 1;
            } else {
            }
          }
        }
      } // inner for loop
      
    })); // thread
    
  }// thread outer loop
  std::for_each(threads.begin(), threads.end(), mem_fn(&std::thread::join));
  return true;
}

void RegTree::GrowNode(MatrixPtr batch_ptr, node cur_node) {
  if (cur_node.col_id >= MAX_NODE_SIZE) return;
  if (cur_node.col_id*2 >= MAX_NODE_SIZE) {
    split_value_->SetValue(cur_node.row_id, cur_node.col_id,
                           {.v=batch_ptr->ColMean(0, cur_node.low, cur_node.high)});
    split_fea_->SetValue(cur_node.row_id, cur_node.col_id, {.cls = 0});
    return;
  }
  std::vector<float> best_sses;
  best_sses.resize(batch_ptr->GetWidth(), -1.0);
  std::vector<Value> best_split_values;
  best_split_values.resize(batch_ptr->GetWidth(), {.v=0.0});
  std::vector<size_t> best_split_rows;
  best_split_rows.resize(batch_ptr->GetWidth(), 0);
  
  size_t MINIMUM_STEP = 5;
  if (SAMPLENUM_SPLIT*5 < cur_node.high - cur_node.low)
    MINIMUM_STEP = (cur_node.high - cur_node.low) / SAMPLENUM_SPLIT;

  std::vector<std::thread> threads;
  for (size_t tid = 0; tid < THREAD_NUM; ++tid) {
    threads.emplace_back(std::thread([&, batch_ptr, tid](){
      
      for (size_t fea_id = tid+1; fea_id < batch_ptr->GetWidth(); fea_id += THREAD_NUM) {
        float best_sse = -1.0;
        Value best_split_value = {.v=0.0};
        size_t best_split_row = 0;
        if (batch_ptr->fea_type(fea_id) == FeaType::DISC) {
          if (cur_node.high - cur_node.low <= MINIMUM_STEP) {
            return;
          }
          std::vector<std::pair<float, size_t>> d;
          for (size_t i = cur_node.low; i < cur_node.high; ++i)
            d.push_back(std::make_pair((*batch_ptr)(i, 0).v, (*batch_ptr)(i, fea_id).cls));
          if (d.size() < 10) return;
          std::sort(d.begin(), d.end(), [](const std::pair<float, size_t>&a, const std::pair<float, size_t>&b){return a.second < b.second;});
          std::vector<size_t> counts;
          std::vector<float> sums;
          std::vector<float> sss;
          std::vector<size_t> splits;
          float count = 1;
          float sum = d[0].first;
          float ss = sum*sum;
          size_t split = d[0].second;
          for (size_t cur_row = 1; cur_row < d.size(); ++cur_row) {
            if (split != d[cur_row].second) {
              counts.push_back(count);
              sums.push_back(sum);
              sss.push_back(ss);
              splits.push_back(split);
              count = 0;
              sum = 0.0;
              ss = 0.0;
            }
            count += 1;
            float tmp_v = d[cur_row].first;
            sum += tmp_v;
            ss += tmp_v*tmp_v;
            split = d[cur_row].second;
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
            float tmp_mean = sums[i]*1.0/counts[i];
            float tmp = sss[i] - counts[i]*(tmp_mean*tmp_mean);
            tmp_mean = (total_sum - sums[i])*1.0/(total_count - counts[i]);
            tmp += total_ss - sss[i] - (total_count - counts[i]) * tmp_mean * tmp_mean;
            if (best_sse < 0 || tmp < best_sse) {
              best_sse = tmp;
              best_split_value.cls = splits[i];
              best_split_row = cur_node.high - counts[i];
            }
          }
          best_sses[fea_id] = best_sse;
          best_split_values[fea_id] = best_split_value;
          best_split_rows[fea_id] = best_split_row;
          
        } else if (batch_ptr->fea_type(fea_id) == FeaType::CONT) {
          if (cur_node.high - cur_node.low <= MINIMUM_STEP) {
            return;
          }
          std::vector<std::pair<float, float>> d;
          for (size_t i = cur_node.low; i < cur_node.high; ++i)
            d.push_back(std::make_pair((*batch_ptr)(i, 0).v, (*batch_ptr)(i, fea_id).v));
          std::sort(d.begin(), d.end(), [](const std::pair<float, float>&a, const std::pair<float, float>&b){return a.second < b.second;});
          std::vector<size_t> counts;
          std::vector<float> sums;
          std::vector<float> sss;
          float total_count = 0;
          float total_sum = 0.0;
          float total_ss = 0.0;
          for (size_t base_row = 0; base_row < d.size(); base_row += MINIMUM_STEP) {
            for (size_t idx = 0; idx < MINIMUM_STEP; ++idx) {
              size_t cur_row = base_row+idx;
              if (cur_row >= d.size()) break;
              total_count += 1;
              float tmp_v = d[cur_row].first;
              total_sum += tmp_v;
              total_ss += tmp_v*tmp_v;
            }
            counts.push_back(total_count);
            sums.push_back(total_sum);
            sss.push_back(total_ss);
          }
          for (size_t i = 0; i < counts.size()-1; ++i) {
            float tmp_mean = sums[i]*1.0/counts[i];
            float tmp = sss[i] - counts[i]*(tmp_mean*tmp_mean);
            tmp_mean = (total_sum - sums[i])*1.0/(total_count - counts[i]);
            tmp += total_ss - sss[i] - (total_count - counts[i]) * tmp_mean * tmp_mean;
            if (best_sse < 0 || tmp < best_sse) {
              best_sse = tmp;
              best_split_row = counts[i] + cur_node.low;
              best_split_value.v = d[counts[i]].second;
            }
          }
          best_sses[fea_id] = best_sse;
          best_split_values[fea_id] = best_split_value;
          best_split_rows[fea_id] = best_split_row;
          
        } else if (batch_ptr->fea_type(fea_id) == FeaType::RANK) {
          LOG(ERROR) << "Rank feature currently not supported";
          return;
        }
      }// inner for loop
    }));// thread
  }// thread for
  std::for_each(threads.begin(), threads.end(), mem_fn(&std::thread::join));
  
  float best_sse = -1.0;
  Value best_split_value;
  size_t best_split_row;
  size_t best_fea = 0;
  
  for (size_t i = 1; i < best_sses.size(); ++i) {
    if (best_sses[i] >= 0.0) {
      if( best_sse < 0.0 || best_sses[i] < best_sse) {
        best_sse = best_sses[i];
        best_split_value = best_split_values[i];
        best_split_row = best_split_rows[i];
        best_fea = i;
      }
    }
  }
  
  //  std::for_each(threads.begin(), threads.end(), mem_fn(&std::thread::join));
  //  std::cout << "=====best sse=====" << std::endl;
  //  std::cout << cur_node.low << " " << cur_node.high  << std::endl;
  //  std::copy(best_sses.begin(), best_sses.end(), std::ostream_iterator<float>(std::cout, " "));
  //  std::cout << std::endl << "=====best row=====" << std::endl;
  //  std::copy(best_split_rows.begin(), best_split_rows.end(), std::ostream_iterator<size_t>(std::cout, " "));
  //  std::cout << std::endl << "==========" << std::endl;
  //  std::cout << "--" << best_sse << " "<< non_split_sse << std::endl;
  
  float non_split_sse = batch_ptr->SSE(cur_node.low, cur_node.high);
  
  if (best_fea == 0 || non_split_sse < best_sse) {
    split_value_->SetValue(cur_node.row_id, cur_node.col_id,
                           {.v=batch_ptr->ColMean(0, cur_node.low, cur_node.high)});
    split_fea_->SetValue(cur_node.row_id, cur_node.col_id, {.cls = 0});
    return;
  }
  split_fea_->SetValue(cur_node.row_id, cur_node.col_id, {.cls = best_fea});
  split_value_->SetValue(cur_node.row_id, cur_node.col_id, best_split_value);
  
  size_t mid = best_split_row;
  if (batch_ptr->fea_type(best_fea) == FeaType::CONT) {
    batch_ptr->Sort(best_fea, cur_node.low, cur_node.high);
  } else if (batch_ptr->fea_type(best_fea) == FeaType::DISC) {
    batch_ptr->Sort(best_fea, cur_node.low, cur_node.high, best_split_value.cls);
  }
  
  node left = {.row_id=cur_node.row_id, .col_id=cur_node.col_id*2, .low = cur_node.low, .high=mid};
  node right = {.row_id=cur_node.row_id, .col_id=cur_node.col_id*2+1, .low = mid, .high=cur_node.high};
  
  GrowNode(batch_ptr, left);
  GrowNode(batch_ptr, right);
  
  return;
}

void RegTree::TrainOneTree(MatrixPtr batch_ptr, float weight) {
  if (split_value_->Empty()) {
    split_value_->SetType(batch_ptr->fea_types());
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

