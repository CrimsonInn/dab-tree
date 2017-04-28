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
    RegTree tree;
    tree.SetType(dp.get_fea_types());
    batch_size = 10000;
    step_size = 0.1;
  }
  
  void Validate() {
    auto batch_ptr = dp.get_validation();
    VectorPtr result_ptr = std::make_shared<std::vector<float>>();
    batch_ptr->SetType(0, FeaType::DISC);
    tree.Predict(batch_ptr, result_ptr);
    
    float loss = 0.0;
    float err = 0;
    for (size_t i = 0; i < batch_ptr->GetHeight(); ++i) {
      float y = (*batch_ptr)(i, 0).cls == 0? -1 : 1;
      float margin = (*result_ptr)[i] * y;
      //      float tmp = margin < 1? 1-margin: 0;
      float tmp = std::exp(-margin);
      loss += tmp;
      if (margin < 0)
        err += 1;
    }
    LOG(INFO) << "Validation Loss=" << loss/result_ptr->size() << ", Accuracy=" << 1-err*1.0/result_ptr->size();

  }
  
  void TrainOneBatch() {
    dp.get_next_batch(batch_ptr, batch_size);
    VectorPtr result_ptr = std::make_shared<std::vector<float>>();
    batch_ptr->SetType(0, FeaType::DISC);
    tree.Predict(batch_ptr, result_ptr);
    
    float loss = 0.0;
    float err = 0;
    for (size_t i = 0; i < batch_ptr->GetHeight(); ++i) {
      float y = (*batch_ptr)(i, 0).cls == 0? -1 : 1;
      float margin = (*result_ptr)[i] * y;
      
      //      float tmp = margin < 1? 1-margin: 0;
      //      if (margin < 1) batch_ptr->SetValue(i, 0, {.v = y});
      //      else
      //        batch_ptr->SetValue(i, 0, {.v = 0.0});
      
      float tmp = std::exp(-margin);
      batch_ptr->SetValue(i, 0, {.v = y*tmp});
      
      
      loss += tmp;
      if (margin < 0)
        err += 1;
    }
    LOG(INFO) << "Training Loss=" << loss/batch_ptr->GetHeight() << ", Accuracy=" << 1-err*1.0/batch_ptr->GetHeight();
    batch_ptr->SetType(0, FeaType::CONT);
    tree.TrainOneTree(batch_ptr, step_size);
    if (tree.NumTrees() % 10 == 0) Validate();
    step_size /= 1;
  }
  
private:
  DataProvider dp;
  MatrixPtr batch_ptr = std::make_shared<Matrix>();
  size_t batch_size;
  float step_size;
};
