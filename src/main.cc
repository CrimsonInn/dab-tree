#include <iostream>
#include <glog/logging.h>
#include "data.h"
#include "tree.h"
#include "matrix.h"
#include "proto_func.h"

int main() {

	const std::string file_name = "BATCH_DATA_FILE";
  // auto ptr = read_batch_data("BATCH_DATA_FILE");
  // print_batch_data(ptr);
  // LOG(INFO) << ptr->num_samples();
  // RegTreePtr tree = std::make_shared<RegTree>();
  // tree->SetType(ptr->get_fea_types());
  // tree->TrainOneTree(ptr->get_samples(), 1.0);
  // tree->Print();
	DataProvider data_provider = DataProvider(file_name);
	data_provider.print_samples(3);

  MatrixPtr test_matrix_ptr = std::make_shared<Matrix>();
  unsigned int width = data_provider.num_feas();
  // std::cout << width << std::endl;
  data_provider.get_next_batch(test_matrix_ptr, 3);

  // std::cout << test_matrix_ptr->GetHeight() << std::endl;
  // std::cout << test_matrix_ptr->GetWidth() << std::endl;
  // for (size_t i = 0; i < test_matrix_ptr->GetWidth(); ++i) {
  //   std::cout << test_matrix_ptr->fea_type(i) << " ";
  // }
  // std::cout << std::endl;

  test_matrix_ptr->Print(2);

  return 0;
}
