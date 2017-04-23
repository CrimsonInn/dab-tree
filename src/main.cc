#include <iostream>
#include <glog/logging.h>
#include "data.h"
#include "tree.h"
#include "matrix.h"
#include "proto_func.h"

int main() {

  auto ptr = read_batch_data("BATCH_DATA_FILE");
  print_batch_data(ptr);
  LOG(INFO) << ptr->num_samples();
  RegTreePtr tree = std::make_shared<RegTree>();
  tree->SetType(ptr->get_fea_types());
  tree->TrainOneTree(ptr->get_samples(), 1.0);
  tree->Print();
  return 0;
}
