#include "matrix.h"
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
