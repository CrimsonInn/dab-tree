#include "data.h"
#include <glog/logging.h>

DataProvider::DataProvider(const std::string &file_name) {

  row_index = 0;

  // Verify that the version of the protobuf that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  LOG(INFO) << "Init DataProvider ..." ;
  LOG(INFO) << "Begin read protobuf file ..." ;
  rawdata::Matrix read_batch_data;

  // Read the existing address book.
  std::fstream input(file_name, std::ios::in | std::ios::binary);
  if (!input) {
    LOG(INFO) << file_name << ": File not found. Creating a new file.";
  } else if (!read_batch_data.ParseFromIstream(&input)) {
    LOG(ERROR) << "Failed to parse protobuf file.";
    return ;
  }

  unsigned int width = read_batch_data.width();
  unsigned int height = read_batch_data.height();

  std::vector<FeaType> ft;
  for (size_t i = 0; i < width; ++i) {
    ft.push_back(FeaType(read_batch_data.fea_types(i)));
  }
  samples_.SetType(ft);

  for (size_t i = 0; i < height; ++i) {
    std::vector<Value> sample;
    for (size_t j = 0; j < width; ++j) {
      if (samples_.fea_type(j) == CONT){
        sample.push_back({.v = read_batch_data.data(i * width + j).v()});
      } else if (samples_.fea_type(j) == DISC) {
        sample.push_back({.cls = read_batch_data.data(i * width + j).cls()});
      } else if (samples_.fea_type(j) == RANK) {
        sample.push_back({.level = static_cast<int>(read_batch_data.data(i * width + j).level())});
      } else {
        LOG(ERROR) << "Protobuf data type error.";
        return ;
      }
    }
    samples_.Add(sample);
  }
}


void DataProvider::print_samples(size_t row_num) {
  unsigned int width = samples_.GetWidth();
  unsigned int height = samples_.GetHeight();
  std::cout << "width: " << width << std::endl;
  std::cout << "height: " << height << std::endl;
  
  std::cout << "Types:" << std::endl;
  for (size_t i = 0; i < width; ++i) {
    if (samples_.fea_type(i) == CONT) {
      std::cout << "CONT" << " ";
    } else if (samples_.fea_type(i) == DISC) {
      std::cout << "DISC" << " ";
    } else if (samples_.fea_type(i) == RANK) {
      std::cout << "RANK" << " ";
    } else {
      LOG(ERROR) << "Data type error.";
      return ;
    }
  }
  std::cout << std::endl;

  std::cout << "Values:" << std::endl;
  for (size_t i = 0; i < row_num; ++i) {
    for (size_t j = 0; j < width; ++j) {
      if (samples_.fea_type(j) == CONT){
        std::cout << "v: " << samples_(i, j).v << " ";
      } else if (samples_.fea_type(j) == DISC) {
        std::cout << "cls: " << samples_(i, j).cls << " ";
      } else if (samples_.fea_type(j) == RANK) {
        std::cout << "level: " << samples_(i, j).level << " ";
      } else {
        LOG(ERROR) << "Data type error.";
        return ;
      }
    }
    std::cout << std::endl;
  }
  return ;
}

void DataProvider::get_next_batch(MatrixPtr batch_ptr, size_t batch_size) {
  size_t width = samples_.GetWidth();
  // batch_ptr = std::make_shared<Matrix>();
  batch_ptr->resize(batch_size, width);
  batch_ptr->SetType(samples_.fea_types());
  int height = samples_.GetHeight();
  size_t i = 0;
  for (i = 0; i < batch_size; ++i) {
//    batch_ptr->AddOneRow(num_feas());
    batch_ptr->Copy(i, samples_.data((i + row_index) % height));
  }
  row_index = (i + row_index) % height; 
}
