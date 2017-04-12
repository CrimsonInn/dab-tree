#include <iostream>
#include <fstream>
#include <string>
#include "proto_func.h"
#include "data.h"
#include "tree.h"
#include <glog/logging.h>


void read_batch_data() {

  // Verify that the version of the protobuf that we linked against is
  // compatible with the version of the headers we compiled against.
  // GOOGLE_PROTOBUF_VERIFY_VERSION;
  LOG(INFO) << "Begin read batch data ...";
  std::cout << "hehe" << std::endl;

  return;
}

void read_tree() {
	return;
}

//int main(int argc, char const *argv[])
//{
//	google::InitGoogleLogging(argv[0]);
//    //FLAGS_log_dir = "./log";
//	LOG(INFO) << "Begin read batch data ...";
//	/* code */
//	std::cout << "Hi" <<std::endl;
//	// GLOG_logtostderr=1;
//	return 0;
//}
