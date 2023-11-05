#!/bin/bash
#
#SBATCH --job-name=ImageQuantization
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=4
#SBATCH --time=0-00:15:00

module load compiler-rt/latest
module add mkl/latest

make clean
make all

time ./main