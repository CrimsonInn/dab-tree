#include "matrix.h"
#include "data.h"
#include "tree.h"
#include "proto_func.h"
#include <iostream>
#include <string>

class Trainer {
public:
  RegTree tree;

  Trainer(const std::string proto_name) {
    dp = DataProvider(proto_name);
    tree = RegTree();
    tree.SetType(dp.get_fea_types());
    batch_size = 256;
    step_size = 0.1;
  }

  void TrainOneBatch() {
    dp.get_next_batch(batch_ptr, batch_size);
    batch_ptr->Print();
    VectorPtr result;
    tree.Predict(batch_ptr, result);
    for (size_t i = 0; i < batch_ptr->GetHeight(); ++i) {
      int y = 1;
      if ((*batch_ptr)(i, 0).cls == 0) y = -1;
      if ((*result)[i] * y > 1.0)
        batch_ptr->SetValue(i, 0, {.v = 0});
      else
        batch_ptr->SetValue(i, 0, {.v = 1});
      }
  tree.TrainOneTree(batch_ptr, step_size);
}

private:
  DataProvider dp;
  MatrixPtr batch_ptr;
  size_t batch_size;
  float step_size;
};
