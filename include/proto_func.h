#ifndef PROTO_FUNC_H_
#define PROTO_FUNC_H_

#include <vector>
#include <memory>
#include "data.h"
#include "tree.h"
#include "data.pb.h"
#include "tree.pb.h"

BatchPtr read_batch_data(const std::string& file_name);
bool write_batch_data(BatchPtr batch_data, const std::string& file_name);

void read_tree();
void write_tree();

#endif 
