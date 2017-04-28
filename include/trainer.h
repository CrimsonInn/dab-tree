#include "matrix.h"
#include "data.h"
#include "tree.h"
#include "proto_func.h"
#include <iostream>
#include <string>
#include <cmath>

class Trainer {
public:
  RegTree tree;

  Trainer(const std::string proto_name) {
    dp = DataProvider(proto_name);
    tree = RegTree();
    tree.SetType(dp.get_fea_types());
    batch_size = 10000;
    step_size = 0.01;
  }

  void TrainOneBatch() {
    dp.get_next_batch(batch_ptr, batch_size);
    VectorPtr result_ptr = std::make_shared<std::vector<float>>();
    batch_ptr->SetType(0, FeaType::DISC);
    tree.Predict(batch_ptr, result_ptr);

    float loss = 0.0;
    float err = 0;
    for (size_t i = 0; i < batch_ptr->GetHeight(); ++i) {
//      int y = 1;
      float y = (*batch_ptr)(i, 0).cls == 0? -1 : 1;
      float margin = (*result_ptr)[i] * y;

//      float tmp = margin < 1? 1-margin: 0;
      float tmp = std::exp(-margin);
      loss += tmp;
//      if (margin < 1) batch_ptr->SetValue(i, 0, {.v = y});
//      else
//        batch_ptr->SetValue(i, 0, {.v = 0.0});
      batch_ptr->SetValue(i, 0, {.v = y*tmp});

      if (margin < 0)
        err += 1;
    }
    LOG(INFO) << "Loss=" << loss/batch_ptr->GetHeight() << ", err rate=" << err/batch_ptr->GetHeight();
    batch_ptr->SetType(0, FeaType::CONT);
//    batch_ptr->Print();
    tree.TrainOneTree(batch_ptr, step_size);
    step_size /= 1.0;
}

private:
  DataProvider dp;
  MatrixPtr batch_ptr = std::make_shared<Matrix>();
  size_t batch_size;
  float step_size;
};
