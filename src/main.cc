#include <iostream>
#include <glog/logging.h>
#include <gflags/gflags.h>
#include "data.h"
#include "tree.h"
#include "matrix.h"
#include "proto_func.h"
#include "trainer.h"

DEFINE_double(lr, 1.0, "learning rate");
DEFINE_uint64(batch, 10000, "batch size");
DEFINE_string(train, "BATCH_DATA_FILE", "training data file");
DEFINE_uint64(threads, 1, "thread num");
DEFINE_uint64(stage, 50, "boosting stage num");
DEFINE_uint64(splits, 100, "splits num for continuous feature");
DEFINE_uint64(nodes, 64, "tree node num");

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  Trainer trainer(FLAGS_train, FLAGS_batch, FLAGS_lr, FLAGS_threads, FLAGS_splits, FLAGS_nodes);
  for (size_t i = 0; i < FLAGS_stage; ++i) {
    trainer.TrainOneBatch();
  }
//    trainer.tree.Print();
  gflags::ShutDownCommandLineFlags();

  return 0;
}
