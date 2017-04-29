
#include <iostream>
#include "mpi.h"
#include "tree.h"
#include "message_tree.h"
#include "cluster.h"
#include "trainer.h"
#include <cstring>
#include <glog/logging.h>
#include <stdexcept>
#include <gflags/gflags.h>

DEFINE_int64(round, 50 ,"round");
DEFINE_bool(bundle, true, "bundle");
// this is only a test of python interface
DLLEXPORT int run(){
    cout << "success \n";
    return 0;
}

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

//  std::string file_name = "BATCH_DATA_FILE";
//  BatchPtr batch_data = std::make_shared<Batch>();;

//  write_batch_data(batch_data, file_name);
  //FLAGS_logtostderr = 1;
  //google::InitGoogleLogging(args[0]);
//  cout << std::stoi(std::string(args[1])) << args[2]<< "\n";
//  bool bunddle = std::string(args[2])=="bunddle"? true:false;
  Dabtree(FLAGS_round, FLAGS_bundle);

  return 0;
}

#include <time.h>
#include <sys/time.h>
double get_wall_time(){
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

void DeclareMessageTree(MPI_Datatype &MPI_TREE){
	MPI_Datatype types[4]={MPI_FLOAT, my_MPI_SIZE_T, my_MPI_SIZE_T, my_MPI_SIZE_T};
	int bls[4] = {1, 1, tmp_MAX_NODE_SIZE, tmp_MAX_NODE_SIZE};
	MPI_Aint disps[4];
	disps[0]=0;
	disps[1]=offsetof(MessageTree,id);
	disps[2]=offsetof(MessageTree,feas);
	disps[3]=offsetof(MessageTree,values);

	
	MPI_Type_create_struct(4,bls,disps,types,&MPI_TREE);
	MPI_Type_commit(&MPI_TREE);
	
}

void DeclareMessageTreeBunddle(MPI_Datatype &MPI_TREE){
	MPI_Datatype types[4*MESSAGE_TREE_BUNDDLE];
	int bls[4*MESSAGE_TREE_BUNDDLE];
	MPI_Aint disps[4*MESSAGE_TREE_BUNDDLE];

	for (int i=0;i<MESSAGE_TREE_BUNDDLE;i++){
		types[4*i]=MPI_FLOAT;
		types[4*i+1]=my_MPI_SIZE_T;
		types[4*i+2]=my_MPI_SIZE_T;
		types[4*i+3]=my_MPI_SIZE_T;
		bls[4*i] = 1;
		bls[4*i+1] = 1;
		bls[4*i+2] = tmp_MAX_NODE_SIZE;
		bls[4*i+3] = tmp_MAX_NODE_SIZE;
		disps[4*i]=0;
		disps[4*i+1]=offsetof(MessageTree,id);
		disps[4*i+2]=offsetof(MessageTree,feas);
		disps[4*i+3]=offsetof(MessageTree,values);
	}

	MPI_Type_create_struct(4*MESSAGE_TREE_BUNDDLE,bls,disps,types,&MPI_TREE);
	MPI_Type_commit(&MPI_TREE);
	
}



DLLEXPORT int Dabtree(int round, bool bunddle){
	int myrank, comm_sz;
	cout << "start project with bunddle "<< bunddle<< "\n";
	MPI_Init(NULL,NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	//create mpi tree type
	MPI_Datatype MPI_TREE;
	//if (bunddle) DeclareMessageTreeBunddle(MPI_TREE);
	//else 
	DeclareMessageTree(MPI_TREE);

	//create mpi tree bunddle type
	//MPI_Datatype types[4]
	if(bunddle&&(round%MESSAGE_TREE_BUNDDLE !=0)) {
		throw std::invalid_argument( "round must be multiple of BUNDDLE "+ std::to_string(MESSAGE_TREE_BUNDDLE) );
	}


	cout <<"thread "<< myrank<<" start working"<< "\n";
	if (myrank==0){
		double time = get_wall_time();
		if(bunddle){
			BunddleMaster(round, comm_sz, MPI_TREE);

		}
		else{
			EnsembleMessageTreePtr tree = Master(round, comm_sz, MPI_TREE);
			//MPI_Barrier(MPI_COMM_WORLD);
			//cout << "master print trees \n";

			//for(auto tr : *tree){
			//	cout<<(tr->Print());
			//}
		}
		cout << "master success, time "<<get_wall_time()-time<< " \n";

	}
	else{
		if (bunddle) BunddleWorker(round,myrank, MPI_TREE);
		else Worker(round,myrank, MPI_TREE);
		cout << "worker success \n";
		//MPI_Barrier(MPI_COMM_WORLD);
	}

    MPI_Type_free(&MPI_TREE);
    MPI_Finalize();
    return 0;


}

void BunddleMaster(int round, int comm_sz, MPI_Datatype &MPI_TREE){
	int total_tree = round*(comm_sz-1);

	int current_index=0;
	MPI_Status stat;
	//local tree
	MessageTree trees[total_tree];
	while (current_index<total_tree){

		MessageTree new_tree[MESSAGE_TREE_BUNDDLE];

		MPI_Recv(&new_tree, MESSAGE_TREE_BUNDDLE, MPI_TREE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);

	// for (int i=0; i<MESSAGE_TREE_BUNDDLE;i++){
	// 	cout << "master print trees";
	// 	cout<<new_tree[i].Print();
	// }

		int remote_count = new_tree[0].id;
		int target_rank = stat.MPI_SOURCE;

		cout << "master receive tree from " << target_rank << "\n";
		for (int bunddle_index=0; bunddle_index < MESSAGE_TREE_BUNDDLE; bunddle_index++){
			new_tree[bunddle_index].id = current_index;
			trees[current_index]=new_tree[bunddle_index];
			current_index += 1;
		}
		//new_tree->Print();
		
		int send_count = (current_index - remote_count)/MESSAGE_TREE_BUNDDLE;
		cout << "master send bunddle count "<< send_count << " to " << target_rank << "\n";
		//send tree count here
		MPI_Ssend(&send_count, 1, MPI_INT, target_rank,0,MPI_COMM_WORLD);

		//for loop to send trees
		for(int i= 0; i<send_count; i++){
			for(int j=0; j< MESSAGE_TREE_BUNDDLE ;j++){
				new_tree[j]=trees[remote_count+i*MESSAGE_TREE_BUNDDLE+j];
			}
			cout << "master send tree from" << remote_count+MESSAGE_TREE_BUNDDLE*i<<" to "<<remote_count+MESSAGE_TREE_BUNDDLE*(i+1)-1 << " to " << target_rank << "\n";
			MPI_Ssend(&new_tree, MESSAGE_TREE_BUNDDLE, MPI_TREE, target_rank, 0, MPI_COMM_WORLD);
		}
		
		cout << "current count: "<< current_index <<", need "<<total_tree << "\n";
		
	}
	//for (int i=0; i<total_tree;i++){
	//	cout << "master print trees";
	//	cout<<trees[i].Print();
	//}
	//return trees;
}

void BunddleWorker(int round, int myrank, MPI_Datatype &MPI_TREE){
	int local_count=0;
	MPI_Status stat;
	//RegTreePtr vec_tree(new RegTree);
	Trainer trainer("BATCH_DATA_FILE");//to_string(myrank));
	MessageTree message_buffer [MESSAGE_TREE_BUNDDLE];
	for (int i=0; i<round/MESSAGE_TREE_BUNDDLE;i++){
		//call train local here
		for(int bunddle_index=0; bunddle_index<MESSAGE_TREE_BUNDDLE; bunddle_index++){
			trainer.TrainOneBatch();//TrainLocal();
			cout<< "tree trained on worker "<< myrank << " is "<< trainer.tree.NumTrees()<<" local_index is "<< local_count+bunddle_index << "\n";
			//cout<<(trainer.tree.Print(local_count+bunddle_index));
			message_buffer[bunddle_index] = trainer.tree.GetStackMessageTree(local_count+bunddle_index);
			message_buffer[bunddle_index].id=local_count+bunddle_index;
			
		}

	// for (int i=0; i<MESSAGE_TREE_BUNDDLE;i++){
	// 	cout << "buffer print trees";
	// 	cout<<message_buffer[i].Print();
	// }
		cout << "worker "<< myrank << " send tree "<< local_count<<" to" << local_count+MESSAGE_TREE_BUNDDLE-1<< "\n";
		
		MPI_Ssend(&message_buffer, MESSAGE_TREE_BUNDDLE, MPI_TREE, 0, 0, MPI_COMM_WORLD);

		int receive_count; 
		MPI_Recv(&receive_count, 1, MPI_INT, 0,0,MPI_COMM_WORLD, &stat);
		cout << "worker "<< myrank << " receive count "<< receive_count<< "\n";

		for (int j=0; j <receive_count; j++){
			MPI_Recv(&message_buffer, MESSAGE_TREE_BUNDDLE, MPI_TREE, 0,0,MPI_COMM_WORLD, &stat);
			for(int bunddle_index=0; bunddle_index<MESSAGE_TREE_BUNDDLE; bunddle_index++){
				cout << "worker "<< myrank << " receive tree id "<< message_buffer[bunddle_index].id << ", " << local_count << "\n";
				//vec_tree->Copy(message_tree);
				trainer.tree.Copy(message_buffer[bunddle_index]);
				//vec_tree->Print(local_count);
				cout << "worker "<< myrank << " pushed tree"<< "\n";
				local_count=local_count+1;
			}
		}
		cout << "worker "<< myrank <<" round "<<i+1 << "\n";
		
	}
	return;
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

		LOG(INFO) << "master receive tree from " << target_rank << "\n";
		
		new_tree->id = current_index;
		trees->push_back(new_tree);
		//new_tree->Print();
		
		int send_count = current_index - remote_count + 1;
		LOG(INFO) << "master send count "<< send_count << " to " << target_rank << "\n";
		//send tree count here
		MPI_Ssend(&send_count, 1, MPI_INT, target_rank,0,MPI_COMM_WORLD);

		//for loop to send trees
		for(int i= remote_count; i<=current_index; i++){
			//LOG(INFO) << "master send tree " << i << " to " << target_rank << "\n";
			MPI_Ssend(trees->at(i).get(), 1, MPI_TREE, target_rank, 0, MPI_COMM_WORLD);
		}
		current_index += 1;
		LOG(INFO) << "current count: "<< current_index <<", need "<<total_tree << "\n";
		
	}
	return trees;
}

void Worker(int round, int myrank, MPI_Datatype &MPI_TREE){
	int local_count=0;
	MPI_Status stat;
	//RegTreePtr vec_tree(new RegTree);
	Trainer trainer("BATCH_DATA_FILE");//to_string(myrank));
	for (int i=0; i<round;i++){
		//call train local here
		//cout<< "before traing tree on worker "<< myrank << " is "<< trainer.tree.NumTrees()<<" local_index is "<< local_count << "\n";
		trainer.TrainOneBatch();//TrainLocal();
		LOG(INFO)<< "tree on worker "<< myrank << " is "<< trainer.tree.NumTrees()<<" local_index is "<< local_count << "\n";
		//if(myrank==2) cout<<(trainer.tree.Print(local_count));

		//MessageTreePtr message_tree = vec_tree -> GetMessageTree(local_count);
		MessageTreePtr message_tree = trainer.tree.GetMessageTree(local_count);
		LOG(INFO) << "worker "<< myrank << " send tree "<< local_count << "\n";
		message_tree ->id=local_count;
		MPI_Ssend(message_tree.get(), 1, MPI_TREE, 0, 0, MPI_COMM_WORLD);

		int receive_count; 
		MPI_Recv(&receive_count, 1, MPI_INT, 0,0,MPI_COMM_WORLD, &stat);
		LOG(INFO) << "worker "<< myrank << " receive count "<< receive_count<< "\n";
		for (int j=0; j <receive_count; j++){
			MPI_Recv(message_tree.get(), 1, MPI_TREE, 0,0,MPI_COMM_WORLD, &stat);
			
			//vec_tree->Copy(message_tree);
			//if(myrank==2) cout<<message_tree->Print();
			trainer.tree.Copy(message_tree);
			//if(myrank==2) cout<<trainer.tree.Print(local_count);
			//vec_tree->Print(local_count);
			LOG(INFO) << "worker "<< myrank << " receive tree id "<< message_tree->id << ", " << local_count<<", "<< trainer.tree.NumTrees() << "\n";
			//LOG(INFO) << "worker "<< myrank << " pushed tree"<< "\n";
			local_count=local_count+1;
		}
		LOG(INFO) << "worker "<< myrank <<" round "<<i+1 << "\n";
		
	}



	return;
}



