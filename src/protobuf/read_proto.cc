#include <iostream>
#include <fstream>
#include <string>
#include "proto_func.h"
#include <glog/logging.h>

/*
BatchPtr read_batch_data(const std::string& file_name) {

  // Verify that the version of the protobuf that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  LOG(INFO) << "Begin read batch data ...";

  BatchPtr batchptr = std::make_shared<Batch>();
  rawdata::Matrix read_batch_data;

  // Read the existing address book.
  std::fstream input(file_name, std::ios::in | std::ios::binary);
  if (!input) {
    LOG(INFO) << file_name << ": File not found. Creating a new file.";
  } else if (!read_batch_data.ParseFromIstream(&input)) {
    LOG(ERROR) << "Failed to parse batch data.";
    return NULL;
  }

  unsigned int width = read_batch_data.width();
  unsigned int height = read_batch_data.height();
  std::vector<FeaType> ft;
  for (size_t i = 0; i < width; ++i) {
  	ft.push_back(FeaType(read_batch_data.fea_types(i)));
  }
  batchptr->set_fea_type(ft);

  for (size_t i = 0; i < height; ++i) {
  	std::vector<Value> sample;
  	for (size_t j = 0; j < width; ++j) {
  		if (batchptr->get_fea_type(j) == CONT){
  			sample.push_back({.v = read_batch_data.data(i * width + j).v()});
  		} else if (batchptr->get_fea_type(j) == DISC) {
  			sample.push_back({.cls = read_batch_data.data(i * width + j).cls()});
  		} else if (batchptr->get_fea_type(j) == RANK) {
  			sample.push_back({.level = static_cast<int>(read_batch_data.data(i * width + j).level())});
  		} else {
  			LOG(ERROR) << "Batch data type error.";
  			return NULL;
  		}
  	}
  	batchptr->add_data(sample);
  }

  return batchptr;

}
*/



// RegTreePtr read_tree(const std::string& file_name) {

//   // Verify that the version of the protobuf that we linked against is
//   // compatible with the version of the headers we compiled against.
//   GOOGLE_PROTOBUF_VERIFY_VERSION;
//   LOG(INFO) << "Begin read tree data ...";

//   RegTreePtr tree = std::make_shared<RegTree>();
//   dabtree::RegTree read_tree;

//   // Read the existing address book.
//   std::fstream input(file_name, std::ios::in | std::ios::binary);
//   if (!input) {
//     LOG(INFO) << file_name << ": File not found. Creating a new file.";
//   } else if (!read_tree.ParseFromIstream(&input)) {
//     LOG(ERROR) << "Failed to parse batch data.";
//     return NULL;
//   }

//   // TODO: 

//   tree->id = read_tree.id();
//   tree->weight = read_tree.weight();

//   for (size_t i = 0; i < read_tree.fea_types_size(); ++i) {
//   	tree->fea_types[i] = FeaType(read_tree.fea_types(i));
//   }

//   for (size_t i = 0; i < read_tree.split_fea_size(); ++i) {
//   	tree->split_fea[i] = read_tree.split_fea(i);
//   }

//   for (size_t i = 0; i < read_tree.split_value_size(); ++i) {
//   	if (tree->fea_types[i] == CONT){
//   		tree->split_value[i] = {.v = read_tree.split_value(i).v()};
// 		} else if (tree->fea_types[i] == DISC) {
// 			tree->split_value[i] = {.cls = read_tree.split_value(i).cls()};
// 		} else if (tree->fea_types[i] == RANK) {
// 			tree->split_value[i] = {.n = static_cast<int>(read_tree.split_value(i).n())};
// 		} else {
// 			LOG(ERROR) << "Tree value data type error.";
// 			return NULL;
// 		}
//   }
  
//   for (size_t i = 0; i < read_tree.is_leaf_size(); ++i) {
//   	tree->is_leaf[i] = read_tree.is_leaf(i);
//   }

//   return tree;

// }


/*
void print_batch_data(BatchPtr batchptr) {

  MatrixPtr matrixptr = batchptr->get_samples();
  unsigned int width = matrixptr->GetWidth();
  unsigned int height = matrixptr->GetHeight();
  std::cout << "width: " << width << std::endl;
  std::cout << "height: " << height << std::endl;
	
	std::cout << "Types:" << std::endl;
	
	
	for (size_t i = 0; i < width; ++i) {
		if (matrixptr->fea_type(i) == CONT) {
			std::cout << "CONT" << " ";
		} else if (matrixptr->fea_type(i) == DISC) {
			std::cout << "DISC" << " ";
		} else if (matrixptr->fea_type(i) == RANK) {
			std::cout << "RANK" << " ";
		} else {
			LOG(ERROR) << "Batch data type error.";
  		return ;
		}
	}
	std::cout << std::endl;
	
	
	auto &matrix = (*matrixptr);
	std::cout << "Values:" << std::endl;
	for (size_t i = 0; i < 3; ++i) {
		for (size_t j = 0; j < width; ++j) {
			if (matrixptr->fea_type(j) == CONT){
				std::cout << "v: " << matrix(i, j).v << " ";
			} else if (matrixptr->fea_type(j) == DISC) {
				std::cout << "cls: " << matrix(i, j).cls << " ";
			} else if (matrixptr->fea_type(j) == RANK) {
				std::cout << "n: " << matrix(i, j).level << " ";
			} else {
				LOG(ERROR) << "Batch data type error.";
  			return ;
			}
		}
		std::cout << std::endl;
	}
	return ;
}
*/

// void print_tree(RegTreePtr tree) {

// 	std::cout << "Begin print tree ..." << std::endl;
// 	std::cout << "id: " << tree->id << std::endl;
// 	std::cout << "weight: " << tree->weight << std::endl;

// 	std::cout << "Type:" << std::endl;
// 	for (auto fea : tree->fea_types) {
// 		if (FeaType(fea) == CONT) {
// 			std::cout << "CONT" << " ";
// 		} else if (FeaType(fea) == DISC) {
// 			std::cout << "DISC" << " ";
// 		} else if (FeaType(fea) == RANK) {
// 			std::cout << "RANK" << " ";
// 		} else {
// 			LOG(ERROR) << "Batch data type error.";
//   		return ;
// 		}
// 	}
// 	std::cout << std::endl;

// 	std::cout << "Split feature:" << std::endl;
// 	for (auto s_f : tree->split_fea) {
// 		std::cout << s_f << " ";
// 	}
// 	std::cout << std::endl;

// 	std:: cout << "Split Value: " << std::endl;
// 	for (size_t i = 0; i < tree->split_value.size(); ++i) {
// 		if (tree->fea_types[i] == CONT){
// 			std::cout << "v: " << tree->split_value[i].v << " ";
// 		} else if (tree->fea_types[i] == DISC) {
// 			std::cout << "cls: " << tree->split_value[i].cls << " ";
// 		} else if (tree->fea_types[i] == RANK) {
// 			std::cout << "n: " << tree->split_value[i].n << " ";
// 		} else {
// 			LOG(ERROR) << "Batch data type error.";
// 			return ;
// 		}
// 	}
// 	std::cout << std::endl;

// 	std::cout << "Is leaf: " << std::endl;
// 	for (auto i_l : tree->is_leaf) {
// 		std::cout << i_l << " ";
// 	}
// 	std::cout << std::endl;

// 	return ;
// }


// int main(int argc, char const *argv[]) {

// 	// std::string file_name = "BATCH_DATA_FILE";

// 	// BatchPtr batch = std::make_shared<Batch>();
// 	// batch = read_batch_data(file_name);
// 	// print_batch_data(batch);

// 	std::string tree_file_name = "TREE_DATA_FILE";
// 	RegTreePtr tree = std::make_shared<RegTree>();
// 	tree = read_tree(tree_file_name);
// 	print_tree(tree);

// 	return 0;
// }
