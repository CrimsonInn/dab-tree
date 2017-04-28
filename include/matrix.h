#ifndef DABTREE_MATRIX_H_
#define DABTREE_MATRIX_H_
#include <iostream>
#include <memory>
#include <vector>
#include <glog/logging.h>
#include <mutex>

enum FeaType {
  CONT = 0,
  DISC = 1,
  RANK = 2
};

union Value {
  float v; //type 0, continuous feature
  size_t cls; // type 1, discrete feature
  int level; //type 2, rank feature
};

class Matrix {
  std::mutex mu;
public:
  Matrix() {}


  Matrix& operator=(const Matrix&) =delete;
  void Create(std::vector<std::vector<Value>>& d,
              std::vector<FeaType>& t) {
    data_ = d;
    fea_types_ = t;
  }

  void Create(size_t width, size_t height) {
    data_ = std::vector<std::vector<Value>>(height, std::vector<Value>(width));
  }

  void Create(size_t width, size_t height, FeaType type) {
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

  void resize(size_t height, size_t width) {
    data_.resize(height, std::vector<Value>(width));
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

  std::vector<FeaType> fea_types() {
    return fea_types_;
  }

  std::vector<std::vector<Value>> data(){
    return data_;
  }


  std::vector<Value> data(size_t row_id) {
    return data_[row_id];
  }

  void SetType(const std::vector<FeaType>& types) {
    fea_types_ = types;
  }

  void SetType(size_t fea_id, FeaType type) {
    fea_types_[fea_id] = type;
  }

  void SetValue(size_t row_id, size_t col_id, const Value& value) {
    CHECK_LT(row_id, GetHeight()) << "Set: row_id out of bound";
    CHECK_LT(col_id, GetWidth()) << "Set: col_id out of bound";
    data_[row_id][col_id] = value;
  }

  void Copy(size_t row_id, const std::vector<Value>& values) {
    CHECK_LT(row_id, GetHeight()) << "Copy: row_id out of bound";
    data_[row_id] = values;
    // std::cout << "Copy: " << data_[row_id].size() << std::endl;
  }

  void Copy(const std::vector<std::vector<Value>>& values) {
    data_ = values;
  }


  void Copy(std::shared_ptr<Matrix> m) {
    data_ = m->data();
    fea_types_ = m->fea_types();
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
  void Sort(size_t col_id, size_t low, size_t high, size_t cls);

  size_t Split(size_t col_id, size_t low, size_t high, size_t cls);
  float ColMean(size_t col_id, size_t low, size_t high);
  float SSE(size_t low, size_t high);
  void Print(size_t row_num);
  void Print();

private:
  std::vector<std::vector<Value>> data_;
  std::vector<FeaType> fea_types_;
};

typedef std::shared_ptr<Matrix> MatrixPtr;
typedef std::shared_ptr<std::vector<float>> VectorPtr;


#endif  // DABTREE_MATRIX_H_

