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

RegTreePtr read_tree(const std::string& file_name);
bool write_tree(RegTreePtr tree, const std::string& file_name);

#endif 
