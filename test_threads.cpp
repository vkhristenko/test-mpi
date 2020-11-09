#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <thread>

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

    auto fun = [&rank](int tag) {
        if (rank==0) {
            int number = tag+10;
            MPI_Send(&number, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);
        } else if (rank == 1) {
            int number;
            MPI_Recv(&number, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("thread %d received number %d\n", tag, number);
        } else if (rank % 2 == 0) {
            int number = tag;
            MPI_Send(&number, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
        } else {
            int number;
            MPI_Recv(&number, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("thread %d of rank %d received number %d\n", tag, rank, number);
        }
    };

    std::thread t0{fun, 0}, t1{fun, 1}, t2{fun, 2}, t3{fun, 3};
    t0.join(); t1.join(); t2.join(); t3.join();

    // finalize
    MPI_Finalize();
}
