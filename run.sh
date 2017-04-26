#!/bin/bash
#SBATCH -J mpitest
#SBATCH -o mpitest.out
#SBATCH -e mpitest.err
#SBATCH -p general
#SBATCH -n 3
#SBATCH -t 30
#SBATCH --mem-per-cpu=40000
module load gcc/6.3.0-fasrc01 openmpi/2.1.0-fasrc01 glog/0.3.4-fasrc01
mpicxx -I include/  -o mpitest.x src/cluster/cluster.cc src/main.cc
srun -n 3 --mpi=pmi2 ./mpitest.x 3