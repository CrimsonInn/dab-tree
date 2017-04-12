
#include <iostream>
#include "mpi.h"
#include "tree.h"
#include <cstring>

#define DLLEXPORT extern "C"
using namespace std;


struct MessageTree{
	size_t id;
	float weight;
	FeaType fea_types[MAX_NODE_SIZE];
  	size_t split_fea[MAX_NODE_SIZE];
  	Value split_value[MAX_NODE_SIZE];
  	char is_leaf[MAX_NODE_SIZE];

  	MessageTree()=default;
  	MessageTree( RegTreePtr t);
  	RegTreePtr ToRegTree();
  	void Print(){
  		cout << "printing message tree ---------------- \n";
  		cout << "id: " << id << "\n";
  		cout << "weight: " << weight << "\n";
  		cout << "fea_types: ";
  		for (int i=0; i<MAX_NODE_SIZE; i++){cout << fea_types[i]<<", ";}
  		cout << "\n";
  		cout << "split_fea: ";
  		for (int i=0; i<MAX_NODE_SIZE; i++){cout << split_fea[i]<<", ";}
  		cout << "\n";
  		cout << "split_value: ";
  		for (int i=0; i<MAX_NODE_SIZE; i++){cout << split_value[i].v<<", ";}
  		cout << "\n";
  		cout << "is_leaf: ";
  		for (int i=0; i<MAX_NODE_SIZE; i++){cout << is_leaf[i]<<", ";}
  		cout << "\n done ---------------- \n \n";

  	}
};

typedef std::shared_ptr<MessageTree> MessageTreePtr;
typedef std::shared_ptr<vector<MessageTreePtr> > EnsembleMessageTreePtr;

MessageTree::MessageTree(RegTreePtr t){
	id = t->id;
	weight = t->weight;
	memcpy(fea_types,  &(t->fea_types[0]), t->fea_types.size() * sizeof(t->fea_types[0]));
	memcpy(split_fea,  &(t->split_fea[0]), t->split_fea.size() * sizeof(t->split_fea[0]));
	memcpy(split_value,  &(t->split_value[0]), t->split_value.size() * sizeof(t->split_value[0]));
	for (int i =0; i< t->is_leaf.size(); i++){is_leaf[i] = t->is_leaf[i];}
	//memcpy(is_leaf,  &(t->is_leaf[0]), t->is_leaf.size() * sizeof(t->is_leaf[0]));
}

RegTreePtr MessageTree::ToRegTree(void){
	RegTreePtr reg_tree ( new RegTree(
		id, 
		weight,
		*(new vector<FeaType>( begin(fea_types), end(fea_types))),
		*(new vector<size_t>( begin(split_fea), end(split_fea))),
		//new vector<int>(split_fea, split_fea + sizeof split_fea / sizeof split_fea[0]),
		//new vector<Value>(split_value, split_value + sizeof split_value / sizeof split_value[0])
		*(new vector<Value>( begin(split_value), end(split_value))),
		*(new vector<char>( begin(is_leaf), end(is_leaf)))
	));

	return reg_tree;
}



// this is only a test of python interface
DLLEXPORT int run(){
	//int myrank, comm_sz;

    cout << "success \n";
    return 1;
}

void Master(int round, int comm_sz, MPI_Datatype &MPI_TREE);
void Worker(int round, int myrank, MPI_Datatype &MPI_TREE);

DLLEXPORT int Dabtree(int round){
	int myrank, comm_sz;
	cout << "start project"<< "\n";
	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	//create mpi tree type
	MPI_Datatype types[6]={MPI_LONG_LONG, MPI_FLOAT, MPI_INT, MPI_LONG_LONG, MPI_LONG_LONG, MPI_CHAR};
	int bls[6] = {1, 1, MAX_NODE_SIZE, MAX_NODE_SIZE, MAX_NODE_SIZE, MAX_NODE_SIZE};
	MPI_Aint disps[6];
	disps[0]=0;
	disps[1]=offsetof(MessageTree,weight);
	disps[2]=offsetof(MessageTree,fea_types);
	disps[3]=offsetof(MessageTree,split_fea);
	disps[4]=offsetof(MessageTree,split_value);
	disps[5]=offsetof(MessageTree,is_leaf);

	MPI_Datatype MPI_TREE;
	MPI_Type_create_struct(6,bls,disps,types,&MPI_TREE);
	MPI_Type_commit(&MPI_TREE);


	cout << "start working"<< "\n";
	if (myrank==0){
		Master(round, comm_sz, MPI_TREE);
	}
	else{
		Worker(round,myrank, MPI_TREE);
	}

    cout << "success \n";

    MPI_Type_free(&MPI_TREE);
    MPI_Finalize();
    return 0;
}


void Master(int round, int comm_sz, MPI_Datatype &MPI_TREE){
	int total_tree = round*(comm_sz-1);
	int current_count=0;
	MPI_Status stat;
	//local tree
	EnsembleMessageTreePtr trees(new vector<MessageTreePtr>);
	while (current_count<=total_tree){
		MessageTreePtr new_tree (new MessageTree);

		MPI_Recv(new_tree.get(), 1, MPI_TREE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);

		int remote_count = new_tree->id;
		int target_rank = stat.MPI_SOURCE;

		cout << "master receive tree from " << target_rank << "\n";
		current_count += 1;
		new_tree->id = current_count;
		trees->push_back(new_tree);
		new_tree->Print();
		cout << "master send count to " << target_rank << "\n";
		int send_count = current_count - remote_count;
		//send tree count here
		MPI_Send(&send_count, 1, MPI_INT, target_rank,0,MPI_COMM_WORLD);

		//for loop to send trees
		for(int i= remote_count; i<current_count; i++){
			cout << "master send tree " << i << " to " << target_rank << "\n";
			MPI_Send(trees->at(i).get(), 1, MPI_TREE, target_rank, 0, MPI_COMM_WORLD);
		}
		
	}

}

void Worker(int round, int myrank, MPI_Datatype &MPI_TREE){
	int local_count=0;
	MPI_Status stat;
	EnsemblePtr vec_tree(new Ensemble);
	for (int i=0; i<round;i++){
		//call train local here
		RegTreePtr treeptr( new RegTree);//TrainLocal();
		treeptr->Print();
		MessageTree message_tree(treeptr);

		cout << "worker send tree"<< "\n";
		message_tree.id=local_count;
		MPI_Send(&message_tree, 1, MPI_TREE, 0, 0, MPI_COMM_WORLD);

		int receive_count; 
		MPI_Recv(&receive_count, 1, MPI_INT, 0,0,MPI_COMM_WORLD, &stat);
		cout << "worker receive count"<< "\n";
		for (int i=local_count; i < local_count+receive_count; i++){
			MPI_Recv(&message_tree, 1, MPI_TREE, 0,0,MPI_COMM_WORLD, &stat);
			cout << "worker receive tree"<< "\n";
			message_tree.Print();
			vec_tree->push_back(message_tree.ToRegTree());
			cout << "worker pushed tree"<< "\n";
		}
		local_count=message_tree.id;
	}
}

int main(){
	Dabtree(2);
}



