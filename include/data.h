#ifndef DABTREE_DATA_H_
#define DABTREE_DATA_H_

#include <iostream>
#include <vector>
#include <memory>
#include "matrix.h"

// enum FeaType {
//   CONT = 0,
//   DISC = 1,
//   RANK = 2
// };

// union Value {
//   float v; //type 0, continuous feature
//   size_t cls; // type 1, discrete feature
//   int level; //type 2, rank feature
// };

// struct Batch {
//   std::vector<FeaType> fea_types;
//   std::vector<std::vector<Value>> samples;
// };

// typedef std::shared_ptr<Batch> BatchPtr;


class Batch {
public:

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

private:
  Matrix samples_ = Matrix();
};

typedef std::shared_ptr<Batch> BatchPtr;

#endif  // DABTREE_DATA_H_
