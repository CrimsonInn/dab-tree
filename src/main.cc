#include <iostream>
#include <glog/logging.h>
#include "data.h"
#include "cluster.h"
//#include "proto_func.h"




int main(int argv, char * args[]) {


//  std::string file_name = "BATCH_DATA_FILE";
//  BatchPtr batch_data = std::make_shared<Batch>();;

//  write_batch_data(batch_data, file_name);
  //FLAGS_logtostderr = 1;
  //google::InitGoogleLogging(args[0]);
  cout << std::stoi(std::string(args[1])) << "\n";
  Dabtree(std::stoi(std::string(args[1])));
  return 0;
}
