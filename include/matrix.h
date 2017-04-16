#ifndef DABTREE_MATRIX_H_
#define DABTREE_MATRIX_H_

#include <memory>
#include <vector>
#include "data.h"

class Matrix {
public:
  std::vector<std::vector<Value>> data;
  std::vector<FeaType> fea_types;

  Matrix(std::vector<std::vector<Value>>& d,
         std::vector<FeaType>& t) :
    data(d), fea_types(t) {}

  Matrix(size_t width, size_t height) {
    data = std::vector<std::vector<Value>>(height, std::vector<Value>(width));
  }

//  Matrix(size_t width, size_t height, FeaType type) {
//    if (type == FeaType::CONT) {
//        data = std::vector<std::vector<Value>>(height,
//                                               std::vector<Value>(width, {.v=0.0}));
//      } else if ()
//  }

  size_t GetWidth() {
    return data[0].size();
  }

  size_t GetHeight() {
    return data.size();
  }


};

typedef std::shared_ptr<Matrix> MatrixPtr;



#endif  // DABTREE_MATRIX_H_

