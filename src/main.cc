#include <iostream>
#include <glog/logging.h>
#include "data.h"
#include "tree.h"
#include "matrix.h"
#include "proto_func.h"

int main() {
//  std::string file_name = "BATCH_DATA_FILE";
//  BatchPtr batch_data = std::make_shared<Batch>();;

//  batch_data->fea_types.push_back(CONT);
//  batch_data->fea_types.push_back(DISC);
//  batch_data->fea_types.push_back(RANK);
//  std::vector<Value> sample = {{.v=1}, {.cls=0}, {.n=2}};
//  batch_data->samples.push_back(sample);
//  batch_data->samples.push_back(sample);

//  write_batch_data(batch_data, file_name);

	std::string file_name = "BATCH_DATA_FILE";

	BatchPtr batchptr = std::make_shared<Batch>();
	batchptr->set_fea_type({FeaType::CONT, FeaType::CONT, FeaType::DISC, FeaType::RANK});
	batchptr->add_data({{.v=0.0}, {.v=1}, {.cls=0}, {.level=2}});
	batchptr->add_data({{.v=0.0}, {.v=1}, {.cls=0}, {.level=0}});
	batchptr->add_data({{.v=0.0}, {.v=0.4}, {.cls=0}, {.level=0}});
	batchptr->add_data({{.v=0.0}, {.v=0.4}, {.cls=1}, {.level=0}});

	// write_batch_data(batchptr, file_name);
	BatchPtr read_batch_ptr = read_batch_data(file_name);
	print_batch_data(read_batch_ptr);


  return 0;
}
