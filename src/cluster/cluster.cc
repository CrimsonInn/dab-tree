
#include <iostream>
#include "mpi.h"
#include "tree.h"
#include "message_tree.h"
#include "cluster.h"
#include <cstring>




// this is only a test of python interface
DLLEXPORT int run(){
	//int myrank, comm_sz;

    cout << "success \n";
    return 1;
}



DLLEXPORT int Dabtree(int round){
	int myrank, comm_sz;
	cout << "start project"<< "\n";
	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	//create mpi tree type
	MPI_Datatype types[4]={MPI_FLOAT, MPI_LONG_LONG, MPI_LONG_LONG, MPI_LONG_LONG};
	int bls[4] = {1, 1, MAX_NODE_SIZE, MAX_NODE_SIZE};
	MPI_Aint disps[4];
	disps[0]=0;
	disps[1]=offsetof(MessageTree,id);
	disps[2]=offsetof(MessageTree,feas);
	disps[3]=offsetof(MessageTree,values);

	MPI_Datatype MPI_TREE;
	MPI_Type_create_struct(4,bls,disps,types,&MPI_TREE);
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
		
		new_tree->id = current_count;
		current_count += 1;
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
	RegTreePtr vec_tree(new RegTree);
	for (int i=0; i<round;i++){
		//call train local here
		vec_tree->AddRandom();//TrainLocal();
		vec_tree->Print(local_count);

		MessageTreePtr message_tree = vec_tree -> GetMessageTree(local_count);

		cout << "worker send tree"<< "\n";
		message_tree ->id=local_count;
		MPI_Send(message_tree.get(), 1, MPI_TREE, 0, 0, MPI_COMM_WORLD);

		int receive_count; 
		MPI_Recv(&receive_count, 1, MPI_INT, 0,0,MPI_COMM_WORLD, &stat);
		cout << "worker receive count"<< "\n";
		for (int i=local_count; i < local_count+receive_count; i++){
			MPI_Recv(message_tree.get(), 1, MPI_TREE, 0,0,MPI_COMM_WORLD, &stat);
			cout << "worker receive tree"<< "\n";
			message_tree->Print();
			vec_tree->Copy(message_tree);
			cout << "worker pushed tree"<< "\n";
		}
		local_count=message_tree->id+1;
	}
}



