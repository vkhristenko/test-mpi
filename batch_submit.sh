#!/bin/bash

#SBATCH --job-name=test_mpi
#SBATCH -A deep
#SBATCH --output=/work/cdeep/khristenko1/jobs_results/submission_logs/%j.log
#SBATCH --error=/work/cdeep/khristenko1/jobs_results/submission_logs/%j.err
#SBATCH --wait-all-nodes=0
#SBATCH --time=00:01:00

#SBATCH --partition=dp-cn
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1

#SBATCH packjob

#SBATCH --partition=dp-esb-ib
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1

# load the env
source /p/project/cdeep/khristenko1/test_mpi_env.sh

# execute
TEST_MPI_DIR=/p/project/cdeep/khristenko1/test-mpi
#PSP_CUDA=1 # disable/enable CUDA awareness
#PSP_UCP=1    # support GPUDirect via UCX in InfiniBand networks (e.g., this is currently true for the ESB nodes)
PSP_CUDA=1 PSP_UCP=1 srun $TEST_MPI_DIR/simple_tests/send_recv/send_recv_veccuda : $TEST_MPI_DIR/simple_tests/send_recv/send_recv_veccuda
