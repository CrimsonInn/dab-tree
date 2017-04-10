
#include <iostream>
#include "mpi.h"
#include "tree.h"

#define DLLEXPORT extern "C"
using namespace std;


struct MessageTree{
	int id;
  	int split_id[MAX_NODE_SIZE];
  	Value split_value[MAX_NODE_SIZE];

  	MessageTree( RegTree &t);
  	RegTree &ToRegTree();
};

MessageTree::MessageTree(RegTree &t){
	id = t.id;
	memcopy(split_id, & t.split_id[0], t.split_id.size() * sizeof(t.split_id[0]));
	memcopy(split_value, & t.split_value[0], t.split_value.size() * sizeof(t.split_value[0]));
}

RegTree & MessageTree::ToRegTree(void){
	RegTree *reg_tree= new RegTree{ 
		id, 
		new vector<int>(split_id, split_id + sizeof split_id / sizeof split_id[0]),
		new vector<Value>(split_value, split_value + sizeof split_value / sizeof split_value[0])
	};

	return *reg_tree;
}




// this is only a test of python interface
DLLEXPORT int run(){
	int myrank, comm_sz;

    cout << "success \n";
    return 1;
}



void Master(int round, int comm_sz){
	int total_tree = round*(comm_sz-1);
	int current_count=0;
	MPI_Status stat;
	//local tree
	vector<MessageTree> trees;
	while (current_count<=total_tree){
		MessageTree *new_tree= new MessageTree();
		MPI_Recv(&new_tree, 1, MPI_TREE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);

		int remote_count = new_tree.id;
		int target_rank = stat.MPI_SOURCE;
		
		current_count += 1;
		new_tree.id = current_count;
		tree.appendTree(new_tree);

		int send_count = current_count - remote_count;
		//send tree count here
		MPI_Send(&send_count, 1, MPI_INT, target_rank,0,MPI_COMM_WORLD);

		//for loop to send trees
		for(int i= remote_count+1; i<current_count+1; i++){
			MPI_Send(&(tree[i]), 1, MPI_TREE, target_rank, 0, MPI_COMM_WORLD);
		}
		
	}

}

void Worker(int round, int myrank, int comm_sz){
	int local_count=0;
	VecTree vec_tree
	for (int i=0; i<round;i++){
		//call train local here
		ReGTree tree = TrainLocal();
		MessageTree message_tree = MessageTree(tree);

		message_tree.id=local_count;
		MPI_Send(&new_tree, 1, MPI_TREE, 0, 0, MPI_COMM_WORLD);

		int receive_count; 
		MPI_Recv(&receive_count, 1, MPI_INT, 0,0,MPI_COMM_WORLD);

		for (int i=local_count; i < local_count+receive_count; i++){
			MPI_Recv(&message_tree, 1, MPI_TREE, 0,0,MPI_COMM_WORLD);
			vec_tree.push_back(message_tree.ToRegTree());
		}
		local_count=message_tree.id;
	}
}


DLLEXPORT int Dabtree(int round){
	int myrank, comm_sz;

	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	//create mpi tree type
	MessageTree m;
	MPI_Datatype types[3]={MPI_INT,MPI_INT,MPI_INT};
	int bls[3] = {1,MAX_NODE_SIZE,MAX_NODE_SIZE};
	MPI_Aint disps[3];
	disps[0]=0;
	disps[1]=int(&(m.split_id[0])-(int*)&m)*sizeof(int);
	disps[2]=int(&(m.split_value[0])-(int*)&m)*sizeof(int);
	MPI_Datatype MPI_TREE
	MPI_Type_create_struct(3,bls,disps,types,&MPI_TREE);
	MPI_Type_commit(&MPI_TREE);

	if (myrank==0){
		Master(round,comm_sz);
	}
	else{
		Worker(round,myrank,comm_sz);
	}

    cout << "success \n";

    MPI_Type_free(&MPI_TREE);
    MPI_Finalize();
    return 0;
}



