#!/bin/bash
#SBATCH -J mpitest
#SBATCH -o mpitest.out
#SBATCH -e mpitest.err
#SBATCH -p general
#SBATCH -n 3
#SBATCH -t 30
#SBATCH --mem-per-cpu=40000
#module load gcc/6.3.0-fasrc01 openmpi/2.1.0-fasrc01 glog/0.3.4-fasrc01 cmake/3.5.2-fasrc01 protobuf/2015-fasrc01 curl/7.45.0-fasrc01
#cmake -DUSE_MPI=ON
#make
mpicxx -std=c++11 -I include/ src/cluster/cluster.cc src/core/*.cc src/trainer/trainer.cc
mpirun -n 3 ./cluster 50 true 2 50000
#srun -n 3 --mpi=pmi2 ./dabtree 10