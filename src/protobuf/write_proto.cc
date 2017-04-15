#include <iostream>
#include <fstream>
#include <string>
#include "proto_func.h"
#include "data.h"
#include "tree.h"
#include <glog/logging.h>

bool write_batch_data(BatchPtr batch_data, const std::string& file_name) {

  LOG(INFO) << "Begin write batch data ...";

  unsigned int fea_type_nums = batch_data->fea_types.size();
  unsigned int value_rows = batch_data->samples.size();
  unsigned int value_cols = 0;
  if(value_rows > 0){
  	value_cols = batch_data->samples[0].size();
  }

  // TODO: do we need to check fea_type_nums ?= value_rows?

  // Verify that the version of the protobuf that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  data::Batch write_batch_data;

  // Read the existing address book.
  std::fstream input(file_name, std::ios::in | std::ios::binary);
  if (!input) {
    LOG(INFO) << file_name << ": File not found. Creating a new file.";
  } else if (!write_batch_data.ParseFromIstream(&input)) {
    LOG(ERROR) << "Failed to parse batch data.";
    return false;
  }

  // write bach data into protobuf
  write_batch_data.set_rows(value_rows);
  write_batch_data.set_cols(value_cols);

  for (auto fea_type : batch_data->fea_types) {
  	write_batch_data.add_fea_types(data::FeaType(fea_type));
  }

  for (auto sample_row : batch_data->samples) {
  	for (size_t i = 0; i < sample_row.size(); ++i) {
  		if (batch_data->fea_types[i] == CONT){
  			write_batch_data.add_samples()->set_v(sample_row[i].v);
  		} else if (batch_data->fea_types[i] == DISC) {
  			write_batch_data.add_samples()->set_cls(sample_row[i].cls);
  		} else if (batch_data->fea_types[i] == RANK) {
  			write_batch_data.add_samples()->set_n(sample_row[i].n);
  		} else {
  			LOG(ERROR) << "Batch data type error.";
  			return false;
  		}
  	}
  }

	// Write the new address book back to disk.
  std::fstream output(file_name, std::ios::out | std::ios::trunc | std::ios::binary);
  if (!write_batch_data.SerializeToOstream(&output)) {
    LOG(ERROR) << "Failed to write batch data.";
    return false;
  } else {
    LOG(INFO) << "Success!";
  }

  // Optional:  Delete all global objects allocated by libprotobuf.
  google::protobuf::ShutdownProtobufLibrary();

  return true;

}

void write_tree() {

}

int main(int argc, char const *argv[])
{
	/* code */
	std::string file_name = "BATCH_DATA_FILE";
	BatchPtr batch_data = std::make_shared<Batch>();;

	batch_data->fea_types.push_back(CONT);
	batch_data->fea_types.push_back(DISC);
	batch_data->fea_types.push_back(RANK);

	std::vector<Value> sample = {{.v=1}, {.cls=0}, {.n=2}};
	batch_data->samples.push_back(sample);
	batch_data->samples.push_back(sample);

	write_batch_data(batch_data, file_name);
	return 0;
}
