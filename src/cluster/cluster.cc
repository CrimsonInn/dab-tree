
#include <iostream>
#include "mpi.h"
#include "tree.h"
#include "message_tree.h"
#include "cluster.h"
#include <cstring>
#include <glog/logging.h>



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
	MPI_Datatype types[4]={MPI_FLOAT, my_MPI_SIZE_T, my_MPI_SIZE_T, my_MPI_SIZE_T};
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
		EnsembleMessageTreePtr tree = Master(round, comm_sz, MPI_TREE);
		MPI_Barrier(MPI_COMM_WORLD);
		cout << "master print trees \n";

		for(auto tr : *tree){
			cout<<(tr->Print());
		}
		cout << "master success \n";
	}
	else{
		Worker(round,myrank, MPI_TREE);
		cout << "worker success \n";
		MPI_Barrier(MPI_COMM_WORLD);
	}

    

    MPI_Type_free(&MPI_TREE);
    MPI_Finalize();
    return 0;
}


EnsembleMessageTreePtr Master(int round, int comm_sz, MPI_Datatype &MPI_TREE){
	int total_tree = round*(comm_sz-1);

	int current_index=0;
	MPI_Status stat;
	//local tree
	EnsembleMessageTreePtr trees(new vector<MessageTreePtr>);
	while (current_index<total_tree){
		MessageTreePtr new_tree (new MessageTree);

		MPI_Recv(new_tree.get(), 1, MPI_TREE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);

		int remote_count = new_tree->id;
		int target_rank = stat.MPI_SOURCE;

		cout << "master receive tree from " << target_rank << "\n";
		
		new_tree->id = current_index;
		trees->push_back(new_tree);
		//new_tree->Print();
		
		int send_count = current_index - remote_count + 1;
		cout << "master send count "<< send_count << " to " << target_rank << "\n";
		//send tree count here
		MPI_Ssend(&send_count, 1, MPI_INT, target_rank,0,MPI_COMM_WORLD);

		//for loop to send trees
		for(int i= remote_count; i<=current_index; i++){
			cout << "master send tree " << i << " to " << target_rank << "\n";
			MPI_Ssend(trees->at(i).get(), 1, MPI_TREE, target_rank, 0, MPI_COMM_WORLD);
		}
		current_index += 1;
		cout << "current count: "<< current_index <<", need "<<total_tree << "\n";
		
	}
	return trees;

}

void Worker(int round, int myrank, MPI_Datatype &MPI_TREE){
	int local_count=0;
	MPI_Status stat;
	RegTreePtr vec_tree(new RegTree);
	for (int i=0; i<round;i++){
		//call train local here
		vec_tree->InitRandom(local_count);//TrainLocal();
		cout<< "tree on worker "<< myrank << " is "<< vec_tree->NumTrees()<<" local_count is "<< local_count << "\n";
		cout<<(vec_tree->Print(local_count));

		MessageTreePtr message_tree = vec_tree -> GetMessageTree(local_count);

		cout << "worker "<< myrank << " send tree "<< local_count << "\n";
		message_tree ->id=local_count;
		MPI_Ssend(message_tree.get(), 1, MPI_TREE, 0, 0, MPI_COMM_WORLD);

		int receive_count; 
		MPI_Recv(&receive_count, 1, MPI_INT, 0,0,MPI_COMM_WORLD, &stat);
		cout << "worker "<< myrank << " receive count "<< receive_count<< "\n";
		for (int j=0; j <receive_count; j++){
			MPI_Recv(message_tree.get(), 1, MPI_TREE, 0,0,MPI_COMM_WORLD, &stat);
			cout << "worker "<< myrank << " receive tree id "<< message_tree->id << ", " << local_count << "\n";
			vec_tree->Copy(message_tree);
			//vec_tree->Print(local_count);
			cout << "worker "<< myrank << " pushed tree"<< "\n";
			local_count=local_count+1;
		}
		cout << "worker "<< myrank <<" round "<<i+1 << "\n";
		
	}
	return;
}



