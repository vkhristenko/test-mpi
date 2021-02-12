#include <cstdlib>
#include <cassert>

#include <iostream>
#include <vector>

#include <mpi.h>

constexpr unsigned int NVALUES = 10000u;

void test_fixed(int rank) {
    if (rank % 2 == 0) {

        // sender sends a vector of NVALUES leements
        std::vector<int> values(NVALUES);
        for (unsigned int i=0; i<values.size(); i++)
            values[i] = static_cast<int>(i);

        // do send
        MPI_Send(values.data(), values.size(), 
            MPI_INT, rank + 1, 0, MPI_COMM_WORLD);

    } else {
        
        // this guy will receive a fixed number of values
        std::vector<int> values(NVALUES);

        // do recv
        MPI_Recv(values.data(), values.size(), 
            MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("rank %d received %lu values\n", rank, values.size());

        // make sure that values match what we expect
        for (unsigned int i=0; i<values.size(); i++)
            assert(values[i] == i && "values must match");
        printf("rank %d verified received values\n", rank);
    }
}

void test_varying(int rank) {
    if (rank % 2 == 0) {

        // sender sends a vector of NVALUES leements
        std::vector<int> values(NVALUES);
        for (unsigned int i=0; i<values.size(); i++)
            values[i] = static_cast<int>(i);

        // do send
        MPI_Send(values.data(), values.size() - 10, 
            MPI_INT, rank + 1, 0, MPI_COMM_WORLD);

    } else {
        
        std::vector<int> values(NVALUES);

        // do recv
        MPI_Status status;
        MPI_Recv(values.data(), NVALUES, 
            MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        int recvSize = -1;
        MPI_Get_count(&status, MPI_INT, &recvSize);
        printf("rank %d received %d values\n", rank, recvSize);

        // make sure that values match what we expect
        for (unsigned int i=0; i<recvSize; i++)
            assert(values[i] == i && "values must match");
        printf("rank %d verified received values\n", rank);
    }
}


int main(int argc, char **argv) {
    std::cout << "hello world" << std::endl;

    // init
    //int provided;
    //MPI_Init_thread(nullptr, nullptr, MPI_THREAD_MULTIPLE, &provided);
    MPI_Init(nullptr, nullptr);

    int nprocs;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    printf("nprocs = %d\n", nprocs);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    printf("rank = %d\n", rank);

    test_fixed(rank);
    test_varying(rank);

    // finalize
    MPI_Finalize();
}
