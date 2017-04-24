#include <iostream>
#include <glog/logging.h>
#include "data.h"
#include "cluster.h"
//#include "proto_func.h"

int main(int argv, char * args[]) {

//  std::string file_name = "BATCH_DATA_FILE";
//  BatchPtr batch_data = std::make_shared<Batch>();;

//  batch_data->fea_types.push_back(CONT);
//  batch_data->fea_types.push_back(DISC);
//  batch_data->fea_types.push_back(RANK);
//  std::vector<Value> sample = {{.v=1}, {.cls=0}, {.n=2}};
//  batch_data->samples.push_back(sample);
//  batch_data->samples.push_back(sample);

//  write_batch_data(batch_data, file_name);
  cout << args[1][0] << "\n";
  Dabtree(args[1][0]-'0');


  return 0;
}
