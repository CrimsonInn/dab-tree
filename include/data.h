#include <iostream>
#include <vector>

enum fea_type {
  CONT = 0,
  DISC = 1,
  RANK = 2
};

union value {
  float v; //type 0, continuous feature
  size_t id; // type 1, discrete feature
  int n; //type 2, rank feature
};

struct DabData {
  std::vector<fea_type> types;
  std::vector<std::vector<value>> values;
};

