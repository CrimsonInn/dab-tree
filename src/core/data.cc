#include "data.h"
#include <glog/logging.h>
#include <algorithm>
#include <random>

DataProvider::DataProvider(const std::string &file_name) {

  // Verify that the version of the protobuf that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  row_index = 0;
  validation_size = 1000;
  LOG(INFO) << "Init DataProvider ..." ;
  LOG(INFO) << "Begin read protobuf file " << file_name << " ..." ;
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

  indexes_.resize(height - validation_size);
  for (size_t i = 0; i < height - validation_size; ++i) {
    indexes_[i] = i + validation_size;
  }

  std::vector<FeaType> ft;
  for (size_t i = 0; i < width; ++i) {
    ft.push_back(FeaType(read_batch_data.fea_types(i)));
  }
  sample_ptr_->SetType(ft);

  for (size_t i = 0; i < height; ++i) {
    std::vector<Value> sample;
    // size_t index = indexes_[i];
    for (size_t j = 0; j < width; ++j) {
      if (sample_ptr_->fea_type(j) == CONT){
        sample.push_back({.v = read_batch_data.data(i * width + j).v()});
      } else if (sample_ptr_->fea_type(j) == DISC) {
        sample.push_back({.cls = read_batch_data.data(i * width + j).cls()});
      } else if (sample_ptr_->fea_type(j) == RANK) {
        sample.push_back({.level = static_cast<int>(read_batch_data.data(i * width + j).level())});
      } else {
        LOG(ERROR) << "Protobuf data type error.";
        return ;
      }
    }
    sample_ptr_->Add(sample);
  }
  LOG(INFO) << "Succeed!" ;
}


void DataProvider::print_samples(size_t row_num) {
  unsigned int width = sample_ptr_->GetWidth();
  unsigned int height = sample_ptr_->GetHeight();
  std::cout << "width: " << width << std::endl;
  std::cout << "height: " << height << std::endl;
  
  std::cout << "Types:" << std::endl;
  for (size_t i = 0; i < width; ++i) {
    if (sample_ptr_->fea_type(i) == CONT) {
      std::cout << "CONT" << " ";
    } else if (sample_ptr_->fea_type(i) == DISC) {
      std::cout << "DISC" << " ";
    } else if (sample_ptr_->fea_type(i) == RANK) {
      std::cout << "RANK" << " ";
    } else {
      LOG(ERROR) << "Data type error.";
      return ;
    }
  }
  std::cout << std::endl;

  std::cout << "Values:" << std::endl;
  for (size_t i = 0; i < row_num; ++i) {
    // size_t index = indexes_[i];
    for (size_t j = 0; j < width; ++j) {
      if (sample_ptr_->fea_type(j) == CONT){
        std::cout << "v: " << (*sample_ptr_)(i, j).v << " ";
      } else if (sample_ptr_->fea_type(j) == DISC) {
        std::cout << "cls: " << (*sample_ptr_)(i, j).cls << " ";
      } else if (sample_ptr_->fea_type(j) == RANK) {
        std::cout << "level: " << (*sample_ptr_)(i, j).level << " ";
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
  size_t width = sample_ptr_->GetWidth();
  // batch_ptr = std::make_shared<Matrix>();
  batch_ptr->resize(batch_size, width);
  batch_ptr->SetType(sample_ptr_->fea_types());
  int height = sample_ptr_->GetHeight();
  for (size_t i = 0; i < batch_size; ++i) {
    size_t index = indexes_[i];
    batch_ptr->Copy(i, sample_ptr_->data((index + row_index) % height));
  }

  // std::cout << batch_size << ", " << row_index << ", " << batch_size + row_index << std::endl;

  if (batch_size + row_index >= height) {
    // std::cout << batch_size << ", " << row_index << ", " << batch_size + row_index << std::endl;
    std::random_shuffle(indexes_.begin(), indexes_.end());
  }
  row_index = (batch_size + row_index) % height; 
}


MatrixPtr DataProvider::get_validation(size_t vali_size) {

  MatrixPtr vali_ptr = std::make_shared<Matrix>();
  validation_size = vali_size;
  size_t width = sample_ptr_->GetWidth();
  vali_ptr->resize(vali_size, width);
  vali_ptr->SetType(sample_ptr_->fea_types());
  int height = sample_ptr_->GetHeight();
  if (vali_size > height) {
    LOG(ERROR) << "Demand validation data size too large." ;
    return NULL;
  }
  for (size_t i = 0; i < vali_size; ++i) {
    vali_ptr->Copy(i, sample_ptr_->data(i));
  }
  return vali_ptr;
}

