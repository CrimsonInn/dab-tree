#include "trainer.h"
#include <gtest/gtest.h>
#include <iostream>
#include <memory>

TEST(TreeTest, TrainOneBatch) {
  Trainer trainer("BATCH_DATA_FILE");
//  Trainer trainer("1");

  for (size_t i = 0; i < 50; ++i) {
      trainer.TrainOneBatch();
    }
//  trainer.tree.Print();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
