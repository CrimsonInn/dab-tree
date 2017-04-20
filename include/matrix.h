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

  Matrix(size_t width, size_t height, FeaType type);

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

  void Sort(size_t col_id, size_t low, size_t high);
  size_t Split(size_t col_id, size_t low, size_t high, size_t cls);
  float ColMean(size_t col_id, size_t low, size_t high);
  float SSE(size_t low, size_t high);

private:
  std::vector<std::vector<Value>> data_;
  std::vector<FeaType> fea_types_;
};

typedef std::shared_ptr<Matrix> MatrixPtr;
typedef std::shared_ptr<std::vector<float>> VectorPtr;


#endif  // DABTREE_MATRIX_H_

