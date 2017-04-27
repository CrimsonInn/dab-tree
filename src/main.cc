#include <iostream>
#include <glog/logging.h>
#include "data.h"
#include "tree.h"
#include "matrix.h"
#include "proto_func.h"
//#include "ThreadPool.h"
#include "trainer.h"
int main() {

  Trainer trainer("/Users/qinlyu/Documents/dab-tree/BATCH_DATA_FILE");
  for (size_t i = 0; i < 40; ++i) {
      trainer.TrainOneBatch();
    }
  trainer.tree.Print();

  return 0;
}
