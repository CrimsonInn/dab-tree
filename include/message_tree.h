#ifndef DABTREE_MESTREE_H_
#define DABTREE_MESTREE_H_

#include "tree.h"
//#include "data.h"
#include <cstdint>
#include <climits>

const size_t MAX_NODE_SIZE = 4;

#if SIZE_MAX == UCHAR_MAX
   #define my_MPI_SIZE_T MPI_UNSIGNED_CHAR
#elif SIZE_MAX == USHRT_MAX
   #define my_MPI_SIZE_T MPI_UNSIGNED_SHORT
#elif SIZE_MAX == UINT_MAX
   #define my_MPI_SIZE_T MPI_UNSIGNED
#elif SIZE_MAX == ULONG_MAX
   #define my_MPI_SIZE_T MPI_UNSIGNED_LONG
#elif SIZE_MAX == ULLONG_MAX
   #define my_MPI_SIZE_T MPI_UNSIGNED_LONG_LONG
#else
   #error "cannot determine size_t length"
#endif

struct MessageTree {
	float weight;
	size_t id;
	Value feas[MAX_NODE_SIZE];
	Value values[MAX_NODE_SIZE];

  	MessageTree()=default;
  	MessageTree(size_t id, float weight, const std::vector<Value>& feas, const std::vector<Value>& values);
  	void Print();
};

typedef std::shared_ptr<MessageTree> MessageTreePtr;
typedef std::shared_ptr<std::vector<MessageTreePtr> > EnsembleMessageTreePtr;

void MessageTree::Print(){
	  	std::cout << "printing message tree ---------------- \n";
  		std::cout << "id: " << id << "\n";
  		std::cout << "weight: " << weight << "\n";
  		std::cout << "feas: ";
  		for (int i=0; i< MAX_NODE_SIZE; i++){std::cout << feas[i].v<<", ";}
  		std::cout << "\n";
  		std::cout << "values: ";
  		for (int i=0; i< MAX_NODE_SIZE; i++){std::cout << values[i].v<<", ";}
  		std::cout << "\n done ---------------- \n \n";

}
MessageTree::MessageTree(size_t id, float weight, const std::vector<Value>& feas,
            const std::vector<Value>& values){
	this->id=id;
	this->weight=weight;
	memcpy(this->feas,  &(feas[0]), feas.size() * sizeof(feas[0]));
	memcpy(this->values,  &(values[0]), values.size() * sizeof(values[0]));
}



#endif