#ifndef DABTREE_DATA_H_
#define DABTREE_DATA_H_

#include <iostream>
#include <vector>
#include <memory>

enum FeaType {
  CONT = 0,
  DISC = 1,
  RANK = 2
};

union Value {
  float v; //type 0, continuous feature
  size_t cls; // type 1, discrete feature
  int n; //type 2, rank feature
};

struct Batch {
  std::vector<FeaType> fea_types;
  std::vector<std::vector<Value> > samples;
};

typedef std::shared_ptr<Batch> BatchPtr;


#endif  // DABTREE_DATA_H_
