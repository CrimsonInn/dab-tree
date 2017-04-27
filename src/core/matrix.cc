#include <algorithm>
#include "matrix.h"
#include <iostream>
#include <glog/logging.h>

Matrix::Matrix(size_t width, size_t height, FeaType type) {
  fea_types_ = {type};
  if (type == FeaType::CONT) {
      data_ = std::vector<std::vector<Value>>(height, std::vector<Value>(width, {.v=0.0}));
    } else if (type == FeaType::DISC) {
      data_ = std::vector<std::vector<Value>>(height, std::vector<Value>(width, {.cls=0}));
    } else if (type == FeaType::RANK) {
      data_ = std::vector<std::vector<Value>>(height, std::vector<Value>(width, {.level=0}));
    }
}

void Matrix::Sort(size_t col_id, size_t low, size_t high) {
  FeaType type = fea_type(col_id);
  if (type == FeaType::CONT) {
      std::sort(data_.begin() + low,
                data_.begin() + high,
                [col_id](const std::vector<Value>& a, const std::vector<Value>& b) {
                    return a[col_id].v < b[col_id].v;
                });
    } else if (type == FeaType::DISC) {
      std::sort(data_.begin() + low,
                data_.begin() + high,
                [col_id](const std::vector<Value>& a, const std::vector<Value>& b) {
                    return a[col_id].cls < b[col_id].cls;
                });
    } else if (type == FeaType::RANK) {
      std::sort(data_.begin() + low,
                data_.begin() + high,
                [col_id](const std::vector<Value>& a, const std::vector<Value>& b) {
                    return a[col_id].level < b[col_id].level;
                });
    }
}

size_t Matrix::Split(size_t col_id, size_t low, size_t high, size_t cls) {
  CHECK_EQ(fea_type(col_id), FeaType::DISC);

  std::sort(data_.begin() + low,
            data_.begin() + high,
            [cls, col_id](const std::vector<Value>& a, const std::vector<Value>& b) {
                return (a[col_id].cls-cls)*(a[col_id].cls-cls) > (b[col_id].cls-cls)*(b[col_id].cls-cls);
            });
  size_t value = data_[high-1][col_id].cls;
  for (size_t i = high-1; i >= low; --i) {
    if (data_[i][col_id].cls != value) return i+1;
  }
  return low;
}

float Matrix::ColMean(size_t col_id, size_t low, size_t high) {
  FeaType type = fea_type(col_id);
  CHECK_EQ(type, FeaType::CONT);
  if (high <= low) return 0.0;
  float result = 0.0;
  for (size_t row_id = low; row_id < high; ++row_id)
    result += data_[row_id][col_id].v;
  return result / (high - low);
}

float Matrix::SSE(size_t low, size_t high) {
  FeaType type = fea_type(0);
  CHECK_EQ(type, FeaType::CONT);
  if (high <= low) return 0.0;
  float mean = ColMean(0, low, high);
  float result = 0.0;
  for (size_t row_id = low; row_id < high; ++row_id) {
    float tmp = data_[row_id][0].v - mean;
    result += tmp*tmp;
  }
  return result;
}



void Matrix::Print(size_t row_num) {
  unsigned int height = data_.size();
  CHECK_LT(0, height) << "Access: empty data";
  unsigned int width_fea = fea_types_.size();
  CHECK_LT(0, width_fea) << "Access: empty fea_types";
  unsigned int width = data_[0].size();
  std::cout << "width: " << width << std::endl;
  std::cout << "height: " << height << std::endl;
  
  std::cout << "Types:" << std::endl;
  for (size_t i = 0; i < width; ++i) {
    if (fea_type(i) == CONT) {
      std::cout << "CONT" << " ";
    } else if (fea_type(i) == DISC) {
      std::cout << "DISC" << " ";
    } else if (fea_type(i) == RANK) {
      std::cout << "RANK" << " ";
    } else {
      LOG(ERROR) << "Data type error.";
      return ;
    }
  }
  std::cout << std::endl;

  std::cout << "Values:" << std::endl;
  for (size_t i = 0; i < row_num; ++i) {
    for (size_t j = 0; j < width; ++j) {
      if (fea_type(j) == CONT){
        std::cout << "v: " << data_[i][j].v << " ";
      } else if (fea_type(j) == DISC) {
        std::cout << "cls: " << data_[i][j].cls << " ";
      } else if (fea_type(j) == RANK) {
        std::cout << "level: " << data_[i][j].level << " ";
      } else {
        LOG(ERROR) << "Data type error.";
        return ;
      }
    }
    std::cout << std::endl;
  }
  return ;
}


void Matrix::BestSplitDISC(size_t col_id, size_t low, size_t high) {
  size_t total_count = high - low;
  std::vector<std::vector<Value>> tmpdata;
  tmpdata.resize(total_count, std::vector<Value>(2));
  for (size_t i = low; i < high; ++i) {
      tmpdata[i][0] = data_[i][0];
      tmpdata[i][1] = data_[i][col_id];
    }
  std::sort(tmpdata.begin() + low,
            tmpdata.begin() + high,
            [](const std::vector<Value>& a, const std::vector<Value>& b) {
                return a[1].cls < b[1].cls;
            });
  std::vector<size_t> counts;
  std::vector<float> sums;
  std::vector<float> sss;
  std::vector<size_t> splits;
  float count = 1;
  float sum = tmpdata[0][0].v;
  float ss = sum*sum;
  size_t split = tmpdata[0][1].cls;
  float total_sum = 0.0;
  float total_ss = 0.0;

  for (size_t cur_row = low+1; cur_row < high; ++cur_row) {
      if (split != tmpdata[cur_row][1].cls) {
          counts.push_back(count);
          sums.push_back(sum);
          total_sum += sum;
          sss.push_back(ss);
          total_ss += ss;
          splits.push_back(split);
          count = 0;
          sum = 0.0;
          ss = 0.0;
        }
      count += 1;
      sum += tmpdata[cur_row][0].v;
      ss += tmpdata[cur_row][0].v * tmpdata[cur_row][0].v;
      split = tmpdata[cur_row][1].cls;
    }
  if (count != 0) {
      counts.push_back(count);
      sums.push_back(sum);
      total_sum += sum;
      sss.push_back(ss);
      total_ss += ss;
      splits.push_back(split);
    }
  std::vector<float> results;
  results.resize(counts.size());
  for (size_t i = 0; i < counts.size(); ++i) {
      results[i] = sss[i] - counts[i]*(sums[i]*sums[i]/counts[i]/counts[i]);
      float tmp_ss = total_ss - sss[i];
      float tmp_mean = (total_sum - sums[i])/counts[i];
      results[i] += tmp_ss - (total_count - counts[i]) * tmp_mean * tmp_mean;
//      if (best_fea == 0 || tmp < best_sse) {
//          best_sse = tmp;
//          best_fea = fea_id;
//          best_split_value.cls = splits[i];
//        }
    }
  size_t best = std::max_element(results.begin(), results.end()) - results.begin();
//  float best_sse
}


void Matrix::Print() {
  unsigned int width = GetWidth();
  unsigned int height = GetHeight();
  std::cout << "width: " << width << std::endl;
  std::cout << "height: " << height << std::endl;

  std::cout << "Types:" << std::endl;
  for (size_t i = 0; i < width; ++i) {
    if (fea_type(i) == CONT) {
      std::cout << "CONT" << " ";
    } else if (fea_type(i) == DISC) {
      std::cout << "DISC" << " ";
    } else if (fea_type(i) == RANK) {
      std::cout << "RANK" << " ";
    } else {
      LOG(ERROR) << "Data type error.";
      return ;
    }
  }
  std::cout << std::endl;

  std::cout << "Values:" << std::endl;
  for (size_t i = 0; i < height; ++i) {
    for (size_t j = 0; j < width; ++j) {
      if (fea_type(j) == CONT){
        std::cout << "v: " << data_[i][j].v << " ";
      } else if (fea_type(j) == DISC) {
        std::cout << "cls: " << data_[i][j].cls << " ";
      } else if (fea_type(j) == RANK) {
        std::cout << "level: " << data_[i][j].level << " ";
      } else {
        LOG(ERROR) << "Data type error.";
        return ;
      }
    }
    std::cout << std::endl;
  }
  return ;
}
