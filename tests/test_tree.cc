#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include "data.h"
#include "tree.h"

void PrintTree(RegTreePtr tree) {
  std::cout << "id:" << tree->id << std::endl;
}


TEST(TreeTest, PredictOneSample) {
  RegTreePtr tree = std::make_shared<RegTree>();
  tree->fea_types[1] = FeaType::CONT;
  tree->fea_types[2] = FeaType::DISC;
  tree->fea_types[3] = FeaType::RANK;
  tree->split_fea[1] = 0;
  tree->split_fea[2] = 1;
  tree->split_fea[3] = 2;
  tree->is_leaf[1] = false;
  tree->is_leaf[2] = false;
  tree->is_leaf[3] = false;
  tree->split_value[1] = {.v=0.5};
  tree->split_value[2] = {.cls=1};
  tree->split_value[3] = {.n=1};
  tree->split_value[4] = {0.4};
  tree->split_value[5] = {0.5};
  tree->split_value[6] = {0.6};
  tree->split_value[7] = {0.7};

  BatchPtr batch = std::make_shared<Batch>();
  batch->fea_types.resize(3);
  batch->fea_types[0] = FeaType::CONT;
  batch->fea_types[1] = FeaType::DISC;
  batch->fea_types[2] = FeaType::RANK;
  std::vector<Value> sample = {{.v=1}, {.cls=0}, {.n=2}};
  batch->samples.push_back(sample);

  VectorPtr result = std::make_shared<std::vector<float>>();
  TreePredict(tree, batch, result);
  EXPECT_EQ(1, result->size());
  ASSERT_FLOAT_EQ(0.7, (*result)[0]);

  (*result)[0] = 0.0;
  batch->samples[0][2].n=0;
  TreePredict(tree, batch, result);
  EXPECT_EQ(1, result->size());
  ASSERT_FLOAT_EQ(0.6, (*result)[0]);

  (*result)[0] = 0.0;
  batch->samples[0][0].v=0.4;
  TreePredict(tree, batch, result);
  EXPECT_EQ(1, result->size());
  ASSERT_FLOAT_EQ(0.5, (*result)[0]);

  (*result)[0] = 0.0;
  batch->samples[0][1].cls=1;
  TreePredict(tree, batch, result);
  EXPECT_EQ(1, result->size());
  ASSERT_FLOAT_EQ(0.4, (*result)[0]);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
