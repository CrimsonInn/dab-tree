#ifndef DABTREE_CLUSTER_H_
#define DABTREE_CLUSTER_H_

#include <iostream>
#include "mpi.h"
#include "tree.h"
#include "message_tree.h"
#include <cstring>


#define DLLEXPORT extern "C"
using namespace std;

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

EnsembleMessageTreePtr Master(int round, int comm_sz, MPI_Datatype &MPI_TREE);

void Worker(int round, int myrank, MPI_Datatype &MPI_TREE);

DLLEXPORT int Dabtree(int round);


#endif
