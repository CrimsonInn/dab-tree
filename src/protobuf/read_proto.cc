#include <iostream>
#include <fstream>
#include <string>
#include "proto_func.h"
#include "data.h"
#include "tree.h"
#include <glog/logging.h>


BatchPtr read_batch_data(const std::string& file_name) {

  // Verify that the version of the protobuf that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  LOG(INFO) << "Begin read batch data ...";

  BatchPtr batch = std::make_shared<Batch>();
  data::Batch read_batch_data;

  // Read the existing address book.
  std::fstream input(file_name, std::ios::in | std::ios::binary);
  if (!input) {
    LOG(INFO) << file_name << ": File not found. Creating a new file.";
  } else if (!read_batch_data.ParseFromIstream(&input)) {
    LOG(ERROR) << "Failed to parse batch data.";
    return NULL;
  }

  // TODO: check clos == samples_size 

  // batch->rows = read_batch_data.rows();
  // batch->clos = read_batch_data.cols();
  unsigned int clos = read_batch_data.cols();
  for (size_t i = 0; i < read_batch_data.fea_types_size(); ++i) {
    batch->fea_types.push_back(FeaType(read_batch_data.fea_types(i)));
  }
  // TODO: do we need to use ceil() ???
  for (size_t i = 0; i < read_batch_data.samples_size()/clos; ++i) {
    std::vector<Value> sample;
    for (size_t j = 0; j < clos; ++j) {
        if (batch->fea_types[j] == CONT){
          sample.push_back({.v = read_batch_data.samples(i * clos + j).v()});
        } else if (batch->fea_types[j] == DISC) {
          sample.push_back({.cls = read_batch_data.samples(i * clos + j).id()});
        } else if (batch->fea_types[j] == RANK) {
          // TODO: check variable format
          sample.push_back({.n = static_cast<int>(read_batch_data.samples(i * clos + j).n())});
        } else {
          LOG(ERROR) << "Batch data type error.";
          return NULL;
        }
    }
    batch->samples.push_back(sample);
  }

  return batch;

}


void read_tree() {
	return;
}


void print_batch_data(BatchPtr batch) {
  for (auto fea : batch->fea_types) {
    std::cout << FeaType(fea) ;
  }
  std::cout << std::endl;

  unsigned int cols = batch->fea_types.size();
  unsigned int total = batch->samples.size();
  for (auto sample : batch->samples) {
    for (size_t j = 0; j < cols; ++j) {
      if (batch->fea_types[j] == CONT){
        std::cout << sample[j].v ;
      } else if (batch->fea_types[j] == DISC) {
        std::cout << sample[j].cls ;
      } else if (batch->fea_types[j] == RANK) {
        std::cout << sample[j].n ;
      } else {
        LOG(ERROR) << "Batch data type error.";
      return ;
      }
    }
    std::cout << std::endl;
  }
  return ;
}


int main(int argc, char const *argv[]) {

  std::string file_name = "BATCH_DATA_FILE";

  BatchPtr batch = std::make_shared<Batch>();
  batch = read_batch_data(file_name);
  print_batch_data(batch);

  return 0;
}
