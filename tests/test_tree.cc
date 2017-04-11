#include "tree.h"
#include <gtest/gtest.h>
#include <iostream>
#include <memory>

void PrintTree(RegTreePtr tree) {
  std::cout << "id:" << tree->id << std::endl;
}

TEST(TreeTest, Init) {
  RegTree tree = {1, 1.0, {0,1}, {{.cls=0}, {.v=0.5}}};
  PrintTree(std::make_shared<RegTree>(tree));
  EXPECT_EQ(1, 1);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
