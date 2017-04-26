#include <iostream>
#include <glog/logging.h>
#include "data.h"

#include "cluster.h"
//#include "proto_func.h"


#include <time.h>
#include <sys/time.h>
double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

int main(int argv, char * args[]) {

//  std::string file_name = "BATCH_DATA_FILE";
//  BatchPtr batch_data = std::make_shared<Batch>();;

//  write_batch_data(batch_data, file_name);
  //FLAGS_logtostderr = 1;
  //google::InitGoogleLogging(args[0]);
  double time = get_wall_time();
  cout << args[1][0] << "\n";
  Dabtree(args[1][0]-'0');
  cout << "time is " << get_wall_time()-time;
  return 0;
}
