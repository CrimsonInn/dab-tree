#include "matrix.h"
#include "data.h"
#include "tree.h"
#include "proto_func.h"
#include <iostream>
#include <string>
#include <cmath>
#include <thread>
#include <algorithm>

class Trainer {
public:
  RegTree tree;
  size_t THREAD_NUM;
  Trainer(const std::string proto_name, size_t bs=10000, float ss=1.0,
          size_t thread_num=1, size_t splits=100, size_t max_node_size=64) {
    dp = DataProvider(proto_name, thread_num);
    tree = RegTree(thread_num, splits, max_node_size);
    THREAD_NUM = thread_num;
    tree.SetType(dp.get_fea_types());
    batch_size = bs;
    step_size = ss;
    
    z = std::make_shared<Matrix>(Matrix());
    dp.get_next_batch(z, batch_size);

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
      if (margin < -5) margin = -5.0;
      float tmp = std::exp(-margin);
      loss += tmp;
      if (margin < 0)
        err += 1;
    }
    std::cout << "Validation Loss=" << loss/result_ptr->size() << ", Accuracy=" << 1-err*1.0/result_ptr->size();
  }
  
  void TrainOneBatch() {
//    MatrixPtr batch_ptr = std::make_shared<Matrix>(Matrix());
    batch_ptr->Copy(z);
//    dp.get_next_batch(batch_ptr, batch_size);
    VectorPtr result_ptr = std::make_shared<std::vector<float>>();
    batch_ptr->SetType(0, FeaType::DISC);
    tree.Predict(batch_ptr, result_ptr);
        
    size_t sample_num = batch_ptr->GetHeight();
    std::vector<std::thread> threads;
    std::vector<float> losses;
    losses.resize(THREAD_NUM, 0.0);
    std::vector<float> errs;
    errs.resize(THREAD_NUM, 0.0);
    
    for (size_t tid = 0; tid < THREAD_NUM; ++tid) {
      threads.emplace_back([&, this, tid](){
        for (size_t i = tid; i < sample_num; i += THREAD_NUM) {
          float y = (*batch_ptr)(i, 0).cls == 0? -1 : 1;
          float margin = (*result_ptr)[i] * y;
          
//                float tmp = margin < 1? 1-margin: 0;
//                if (margin < 1) batch_ptr->SetValue(i, 0, {.v = y});
//                else
//                  batch_ptr->SetValue(i, 0, {.v = 0.0});
          if (margin < -5) margin = -5.0;
          float tmp = std::exp(-margin);
          batch_ptr->SetValue(i, 0, {.v = y*tmp});

          losses[tid] += tmp;
          if (margin < 0)
            errs[tid] += 1;
        } // inner for loop
      }); // thread
    } // thread for
    std::for_each(threads.begin(), threads.end(), mem_fn(&std::thread::join));
    float loss = 0.0;
    float err = 0;
    for (auto l : losses) loss += l;
    for (auto e : errs) err += e;
    std::cout << "Training Loss=" << loss/sample_num << ", Accuracy=" << 1-err/sample_num;
    batch_ptr->SetType(0, FeaType::CONT);
    tree.TrainOneTree(batch_ptr, step_size);
//    if (tree.NumTrees() % 10 == 0) Validate();
//    step_size /= 1;
  }
  
private:
  DataProvider dp;
  MatrixPtr batch_ptr = std::make_shared<Matrix>();
  size_t batch_size;
  float step_size;
  MatrixPtr z;
};
