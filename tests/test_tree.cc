#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include "data.h"
#include "tree.h"
#include "trainer.h"

TEST(TreeTest, TrainOneBatch) {
  Trainer trainer("BATCH_DATA_FILE");
  for (size_t i = 0; i < 20; ++i) {
      trainer.TrainOneBatch();
    }
  trainer.tree.Print();
}


TEST(TreeTest, OneTreePredict) {
  RegTreePtr tree = std::make_shared<RegTree>();
  tree->SetType({FeaType::CONT, FeaType::CONT, FeaType::DISC, FeaType::RANK});
  std::vector<Value> feas = {{.cls=0}, {.cls=1}, {.cls=2}, {.cls=3}};
  feas.resize(MAX_NODE_SIZE, {.cls=0});
  std::vector<Value> values = {{.v=0},
                               {.v=0.5}, {.cls=1}, {.level=1},
                               {.v=0.4}, {.v=0.5}, {.v=0.6}, {.v=0.7}};
  values.resize(MAX_NODE_SIZE, {.v=0.0});
  tree->AddOneTree(feas, values);
//  tree->Print();

  MatrixPtr batch = std::make_shared<Matrix>(Matrix(4, 4));
  batch->SetType({FeaType::CONT, FeaType::CONT, FeaType::DISC, FeaType::RANK});
  batch->Copy(0, {{.v=0.0}, {.v=1}, {.cls=0}, {.level=2}});
  batch->Copy(1, {{.v=0.0}, {.v=1}, {.cls=0}, {.level=0}});
  batch->Copy(2, {{.v=0.0}, {.v=0.4}, {.cls=0}, {.level=0}});
  batch->Copy(3, {{.v=0.0}, {.v=0.4}, {.cls=1}, {.level=0}});

  VectorPtr result = std::make_shared<std::vector<float>>();
  tree->Predict(batch, result);
  ASSERT_FLOAT_EQ(0.7, (*result)[0]);
  ASSERT_FLOAT_EQ(0.6, (*result)[1]);
  ASSERT_FLOAT_EQ(0.75, (*result)[2]);
  ASSERT_FLOAT_EQ(0.4, (*result)[3]);
}

TEST(TreeTest, EnsemblePredict) {
  RegTreePtr tree = std::make_shared<RegTree>();
  tree->SetType({FeaType::CONT, FeaType::CONT, FeaType::DISC, FeaType::RANK});
  std::vector<Value> feas = {{.cls=0}, {.cls=1}, {.cls=2}, {.cls=3}};
  feas.resize(MAX_NODE_SIZE, {.cls=0});
  std::vector<Value> values = {{.v=0},
                               {.v=0.5}, {.cls=1}, {.level=1},
                               {.v=0.4}, {.v=0.5}, {.v=0.6}, {.v=0.7}};
  values.resize(MAX_NODE_SIZE, {.v=0.0});
  tree->AddOneTree(feas, values);
  tree->AddOneTree(feas, values, 0.5);
//  tree->Print();


  MatrixPtr batch = std::make_shared<Matrix>(Matrix(4, 4));
  batch->SetType({FeaType::CONT, FeaType::CONT, FeaType::DISC, FeaType::RANK});
  batch->Copy(0, {{.v=0.0}, {.v=1}, {.cls=0}, {.level=2}});
  batch->Copy(1, {{.v=0.0}, {.v=1}, {.cls=0}, {.level=0}});
  batch->Copy(2, {{.v=0.0}, {.v=0.4}, {.cls=0}, {.level=0}});
  batch->Copy(3, {{.v=0.0}, {.v=0.4}, {.cls=1}, {.level=0}});

  VectorPtr result = std::make_shared<std::vector<float>>();
  tree->Predict(batch, result);
  ASSERT_FLOAT_EQ(0.7 + 0.35, (*result)[0]);
  ASSERT_FLOAT_EQ(0.6 + 0.3, (*result)[1]);
  ASSERT_FLOAT_EQ(0.5 + 0.25, (*result)[2]);
  ASSERT_FLOAT_EQ(0.4 + 0.2, (*result)[3]);
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
