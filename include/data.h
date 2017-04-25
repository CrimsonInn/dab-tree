#ifndef DABTREE_DATA_H_
#define DABTREE_DATA_H_

#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <string>
#include "matrix.h"
#include "data.pb.h"
#include "matrix.pb.h"

class DataProvider {
public:

  DataProvider(){ row_index = 0; }
  DataProvider(const std::string &file_name);

  size_t num_samples() {
    return samples_.GetHeight();
  }

  size_t num_feas() {
    return samples_.GetWidth();
  }

  FeaType get_fea_type(size_t index) {
    return samples_.fea_type(index);
  }
  
  std::vector<FeaType> get_fea_types() {
    return samples_.fea_types();
  }

  Value get_value(size_t row_id, size_t col_id) {
    return samples_(row_id, col_id);
  }

  void set_fea_type(const std::vector<FeaType>& types) {
    samples_.SetType(types);
  }

  void add_data(const std::vector<Value>& value) {
    samples_.Add(value);
  }

  MatrixPtr get_samples() {
    MatrixPtr samptr = std::make_shared<Matrix>(samples_);
    return samptr;
  }

  void get_next_batch(MatrixPtr batch_ptr, size_t batch_size, size_t width);

  void print_samples(size_t row_num);

private:
  int row_index;
  Matrix samples_ = Matrix();
};


#endif  // DABTREE_DATA_H_
