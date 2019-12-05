#include <mpi.h>
#include <iostream>
#include <cstdlib>

int main(int argc, char **argv) {
    std::cout << "hello world" << std::endl;

    // init
    int provided;
    MPI_Init_thread(nullptr, nullptr, MPI_THREAD_MULTIPLE, &provided);
    assert(provided == MPI_THREAD_MULTIPLE);

    int nprocs;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    printf("nprocs = %d\n", nprocs);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    printf("rank = %d\n", rank);
    
    if (rank % 2 == 0) {
        int number = rank;
        MPI_Send(&number, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
    } else if (rank % 2 == 1) {
        int number;
        MPI_Recv(&number, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("rank %d received number %d\n", rank, number);
    }

    // finalize
    MPI_Finalize();
}
