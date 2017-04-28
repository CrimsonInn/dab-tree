#include <algorithm>
#include "matrix.h"
#include <iostream>
#include <glog/logging.h>

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
void Matrix::Sort(size_t col_id, size_t low, size_t high, size_t cls) {
  std::sort(data_.begin() + low,
            data_.begin() + high,
            [cls, col_id](const std::vector<Value>& a, const std::vector<Value>& b) {
              return (a[col_id].cls-cls)*(a[col_id].cls-cls) > (b[col_id].cls-cls)*(b[col_id].cls-cls);
            });
}

size_t Matrix::Split(size_t col_id, size_t low, size_t high, size_t cls) {
  CHECK_EQ(fea_type(col_id), FeaType::DISC);
  CHECK_LE(high, GetHeight());
  CHECK_LT(col_id, GetWidth());
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
