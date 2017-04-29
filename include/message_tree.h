#ifndef DABTREE_MESTREE_H_
#define DABTREE_MESTREE_H_

#include "tree.h"
#include "matrix.h"
#include <cstdint>
#include <climits>
#include <sstream>
#include <string>



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

const size_t MESSAGE_TREE_BUNDDLE = 10;




class MessageTree {
public:
  float weight;
  size_t id;
  Value feas[MAX_NODE_SIZE];
  Value values[MAX_NODE_SIZE];
  MessageTree()=default;
  MessageTree(size_t id, float weight, const std::vector<Value>& feas, const std::vector<Value>& values) {
    this->id=id;
    this->weight=weight;
    memcpy(this->feas,  &(feas[0]), feas.size() * sizeof(feas[0]));
    memcpy(this->values,  &(values[0]), values.size() * sizeof(values[0]));
  }

  std::string Print() {
    std::ostringstream s;
    s<< "printing message tree ---------------- \n";
    s << "id: " << id << "\n";
    s << "weight: " << weight << "\n";
    s << "feas: ";
    for (int i=0; i< MAX_NODE_SIZE; i++){s<< feas[i].v<<", ";}
    s << "\n";
    s << "values: ";
    for (int i=0; i< MAX_NODE_SIZE; i++){s << values[i].v<<", ";}
    s << "\n done ---------------- \n \n";
    return s.str();
  }
};

class MessageTreeBunddle{
public:
  MessageTree message_trees[MESSAGE_TREE_BUNDDLE];

};

typedef std::shared_ptr<std::vector<MessageTreeBunddle> > EnsembleMessageTreeBunddlePtr;
typedef std::shared_ptr<MessageTreeBunddle> MessageTreeBunddlePtr;
typedef std::shared_ptr<MessageTree> MessageTreePtr;
typedef std::shared_ptr<std::vector<MessageTreePtr> > EnsembleMessageTreePtr;


#endif
