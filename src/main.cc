#include <iostream>
#include <glog/logging.h>
#include "data.h"
//#include "proto_func.h"
#include "tree.h"

int main() {
//  std::string file_name = "BATCH_DATA_FILE";
//  BatchPtr batch_data = std::make_shared<Batch>();;

//  batch_data->fea_types.push_back(CONT);
//  batch_data->fea_types.push_back(DISC);
//  batch_data->fea_types.push_back(RANK);
//  std::vector<Value> sample = {{.v=1}, {.cls=0}, {.n=2}};
//  batch_data->samples.push_back(sample);
//  batch_data->samples.push_back(sample);

//  write_batch_data(batch_data, file_name);
  RegTreePtr tree = std::make_shared<RegTree>();
  tree->SetType({FeaType::CONT, FeaType::CONT, FeaType::DISC, FeaType::RANK});

  MatrixPtr batch = std::make_shared<Matrix>(Matrix(4, 4));
  batch->SetType({FeaType::CONT, FeaType::CONT, FeaType::DISC, FeaType::RANK});
  batch->Copy(0, {{.v=0.1}, {.v=1}, {.cls=0}, {.level=2}});
  batch->Copy(1, {{.v=0.2}, {.v=1}, {.cls=0}, {.level=0}});
  batch->Copy(2, {{.v=0.3}, {.v=0.4}, {.cls=0}, {.level=0}});
  batch->Copy(3, {{.v=0.4}, {.v=0.4}, {.cls=1}, {.level=0}});

  tree->TrainOneTree(batch, 1.0);
  tree->Print();
  return 0;
}
