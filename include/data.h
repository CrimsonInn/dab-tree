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

typedef std::vector<Value> Sample;
typedef std::shared_ptr<Sample> SamplePtr;

struct DabData {
  std::vector<FeaType> types;
  std::vector<Sample> values;
};

