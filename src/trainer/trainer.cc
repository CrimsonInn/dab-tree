#include "matrix.h"
#include "data.h"
#include "tree.h"
#include "proto_func.h"
#include <iostream>
#include <string>

class Trainer {
public:
  Trainer(const std::string proto_name) {
    ptr = read_batch_data(proto_name);
    tree = std::make_shared<RegTree>();
    tree->SetType(ptr->get_fea_types());
    tree->TrainOneTree(ptr->get_samples(), 1.0);
    tree->Print();
  }

  if(ptr->Next(batch))


private:
  BatchPtr ptr;
  RegTree tree;
};
