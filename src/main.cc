#include <iostream>
#include "data.h"

int main() {
  Value tmp1 = {0.2};
  Value tmp2 = {.cls = 2};
  std::cout << tmp1.v << std::endl;
  std::cout << tmp2.cls << std::endl;

  return 0;
}
