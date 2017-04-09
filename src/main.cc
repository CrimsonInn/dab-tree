#include <iostream>
#include "data.h"

int main() {
  value tmp1 = {0.2};
  value tmp2 = {.id = 2};
  std::cout << tmp1.v << std::endl;
  std::cout << tmp2.id << std::endl;

  return 0;
}
