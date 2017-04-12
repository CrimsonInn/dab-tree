#ifndef PROTO_FUNC_H_
#define PROTO_FUNC_H_

#include <vector>
#include <memory>
#include "data.pb.h"
#include "tree.pb.h"

void read_batch_data();
void write_batch_data();

void read_tree();
void write_tree();

#endif 
