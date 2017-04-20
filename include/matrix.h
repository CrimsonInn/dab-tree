#ifndef DABTREE_MATRIX_H_
#define DABTREE_MATRIX_H_

#include <memory>
#include <vector>
#include <glog/logging.h>
#include "data.h"

class Matrix {
public:

  Matrix(std::vector<std::vector<Value>>& d,
         std::vector<FeaType>& t) :
    data_(d), fea_types_(t) {}

  Matrix(size_t width, size_t height) {
    data_ = std::vector<std::vector<Value>>(height, std::vector<Value>(width));
  }

  Matrix(size_t width, size_t height, FeaType type) {
    fea_types_ = {type};
    if (type == FeaType::CONT) {
        data_ = std::vector<std::vector<Value>>(height, std::vector<Value>(width, {.v=0.0}));
      } else if (type == FeaType::DISC) {
        data_ = std::vector<std::vector<Value>>(height, std::vector<Value>(width, {.cls=0}));
      } else if (type == FeaType::RANK) {
        data_ = std::vector<std::vector<Value>>(height, std::vector<Value>(width, {.level=0}));
      }
  }

  bool Empty() {
    return data_.empty();
  }

  size_t GetWidth() {
    if (GetHeight() == 0) return 0;
    return data_[0].size();
  }

  size_t GetHeight() {
    return data_.size();
  }

  Value operator() (size_t row_id, size_t col_id) {
    CHECK_LT(row_id, GetHeight()) << "Access: row_id out of bound";
    CHECK_LT(col_id, GetWidth()) << "Access: col_id out of bound";
    return data_[row_id][col_id];
  }

  FeaType fea_type(size_t col_id) {
    CHECK_LT(col_id, fea_types_.size()) << "Access: feature id out of bound";
    return fea_types_[col_id];
  }

  std::vector<FeaType> fea_type() {
    return fea_types_;
  }

  void SetType(const std::vector<FeaType>& types) {
    fea_types_ = types;
  }

  void SetValue(size_t row_id, size_t col_id, const Value& value) {
    CHECK_LT(row_id, GetHeight()) << "Set: row_id out of bound";
    CHECK_LT(col_id, GetWidth()) << "Set: col_id out of bound";
    data_[row_id][col_id] = value;
  }

  void Copy(size_t row_id, const std::vector<Value>& values) {
    CHECK_LT(row_id, GetHeight()) << "Copy: row_id out of bound";
    data_[row_id] = values;
  }

  void Copy(const std::vector<std::vector<Value>>& values) {
    data_ = values;
  }

  void Add(const std::vector<Value>& values) {
    data_.push_back(values);
  }

  void AddOneRow(size_t col_num) {
    std::vector<Value> values;
    values.resize(col_num);
    data_.push_back(values);
  }

  void Sort(size_t col_id, size_t low, size_t high) {
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

  size_t Split(size_t col_id, size_t low, size_t high, size_t cls) {
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

  float ColMean(size_t col_id, size_t low, size_t high) {
    FeaType type = fea_type(col_id);
    CHECK_EQ(type, FeaType::CONT);
    if (high <= low) return 0.0;
    float result = 0.0;
    for (size_t row_id = low; row_id < high; ++row_id)
      result += data_[row_id][col_id].v;
    return result / (high - low);
  }

  float SSE(size_t low, size_t high) {
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


private:
  std::vector<std::vector<Value>> data_;
  std::vector<FeaType> fea_types_;
};

typedef std::shared_ptr<Matrix> MatrixPtr;
typedef std::shared_ptr<std::vector<float>> VectorPtr;


#endif  // DABTREE_MATRIX_H_

