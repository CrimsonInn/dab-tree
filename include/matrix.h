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

  size_t GetWidth() {
    return data_[0].size();
  }

  size_t GetHeight() {
    return data_.size();
  }

  Value operator() (size_t row_id, size_t col_id) {
    return data_[row_id][col_id];
  }

  FeaType fea_type(size_t col_id) {
    return fea_types_[col_id];
  }

  void SetType(const std::vector<FeaType>& types) {
    fea_types_ = types;
  }

  void Copy(size_t row_id, const std::vector<Value>& values) {
    data_[row_id] = values;
  }

  void Copy(const std::vector<std::vector<Value>>& values) {
    data_ = values;
  }

  std::vector<Value>& Get(size_t row_id = -1){
    if (row_id== -1){
      row_id = this->GetHeight();
    }
    return data_[row_id];
  }

  void AddZero(){
    data_.push_back(std::vector<Value>(MAX_NODE_SIZE, {.v=0.0}));
  }
  void Add(const std::vector<Value>& values) {
    data_.push_back(values);
  }


private:
  std::vector<std::vector<Value>> data_;
  std::vector<FeaType> fea_types_;
};

typedef std::shared_ptr<Matrix> MatrixPtr;
typedef std::shared_ptr<std::vector<float>> VectorPtr;


#endif  // DABTREE_MATRIX_H_

