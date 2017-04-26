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
    batch_size = 512;
    step_size = 0.01;
  }

  void TrainOneBatch() {
    dp.get_next_batch(batch_ptr, batch_size);
//    batch_ptr->Print();
    VectorPtr result_ptr = std::make_shared<std::vector<float>>();
    tree.Predict(batch_ptr, result_ptr);

    float loss = 0.0;
    float err = 0;
    for (size_t i = 0; i < batch_ptr->GetHeight(); ++i) {
      int y = 1;
      if ((*batch_ptr)(i, 0).cls == 0) y = -1;
      if ((*result_ptr)[i] * y > 1.0) {
        batch_ptr->SetValue(i, 0, {.v = 0});
      }
      else {
        loss += 1.0 - (*result_ptr)[i]*y;
        batch_ptr->SetValue(i, 0, {.v = 1});
      }

      if ((*result_ptr)[i] * y < 0)
        err += 1;
    }
    LOG(INFO) << "Loss=" << loss/batch_ptr->GetHeight() << ", err rate=" << err/batch_ptr->GetHeight();
    batch_ptr->SetType(0, FeaType::CONT);
    tree.TrainOneTree(batch_ptr, step_size);
}

private:
  DataProvider dp;
  MatrixPtr batch_ptr = std::make_shared<Matrix>();
  size_t batch_size;
  float step_size;
};
