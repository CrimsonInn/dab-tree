#ifndef DABTREE_MATRIX_H_
#define DABTREE_MATRIX_H_
#include <iostream>
#include <memory>
#include <vector>
#include <glog/logging.h>
#include <mutex>

const size_t MAX_NODE_SIZE = 64;
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
  //  std::mutex mu;
public:
  Matrix() {}
  
  
  Matrix& operator=(const Matrix&) =delete;
  void Create(std::vector<std::vector<Value>>& d,
              std::vector<FeaType>& t) {
    data_ = d;
    fea_types_ = t;
    width_ = d[0].size();
    height_ = d.size();
  }
  
  void Create(size_t width, size_t height) {
    data_ = std::vector<std::vector<Value>>(height, std::vector<Value>(width));
    width_ = width;
    height_ = height;
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
    width_ = width;
    height_ = height;
  }
  
  bool Empty() {
    return data_.empty();
  }
  
  size_t GetWidth() {
    return width_;
  }
  
  size_t GetHeight() {
    return height_;
  }
  
  void resize(size_t height, size_t width) {
    data_.resize(height, std::vector<Value>(width));
    width_ = width;
    height_ = height;
  }
  
  Value operator() (size_t row_id, size_t col_id) {
    return data_[row_id][col_id];
  }
  
  FeaType fea_type(size_t col_id) {
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
    data_[row_id][col_id] = value;
  }
  
  void Copy(size_t row_id, const std::vector<Value>& values) {
    data_[row_id] = values;
  }
  
  void Copy(const std::vector<std::vector<Value>>& values) {
    data_ = values;
    width_ = values[0].size();
    height_ = values.size();
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

  void Copy(std::shared_ptr<Matrix> m) {
    data_ = m->data();
    fea_types_ = m->fea_types();
    width_ = data_[0].size();
    height_ = data_.size();
  }
  
  void Add(const std::vector<Value>& values) {
    data_.push_back(values);
    height_++;
    width_ = values.size();
  }
  
  void AddOneRow(size_t col_num) {
    std::vector<Value> values;
    values.resize(col_num);
    data_.push_back(values);
    width_ = col_num;
    height_++;
  }

  void set_width(size_t width) {
    width_ = width;
  }

  void set_height(size_t height) {
    height_ = height;
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
  size_t width_ = 0;
  size_t height_ = 0;
};

typedef std::shared_ptr<Matrix> MatrixPtr;
typedef std::shared_ptr<std::vector<float>> VectorPtr;


#endif  // DABTREE_MATRIX_H_

