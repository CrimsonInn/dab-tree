#include <iostream>
#include <vector>

enum FeaType {
  CONT = 0,
  DISC = 1,
  RANK = 2
};

union Value {
  float v; //type 0, continuous feature
  size_t id; // type 1, discrete feature
  int n; //type 2, rank feature
};

struct DabData {
  std::vector<FeaType> types;
  std::vector<std::vector<Value>> values;
};

