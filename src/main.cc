#include <iostream>
#include "data.h"
#include "proto_func.h"

int main() {
  Value tmp1 = {0.2};
  Value tmp2 = {.cls = 2};
  std::cout << tmp1.v << std::endl;
  std::cout << tmp2.cls << std::endl;

  read_batch_data();

  return 0;
}
