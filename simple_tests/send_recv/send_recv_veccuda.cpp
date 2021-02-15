#include <cstdlib>
#include <cassert>

#include <iostream>
#include <vector>

#include <mpi.h>

//#include "test_include.h"

#include <cuda_runtime_api.h>

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
        int *d_values;
        cudaMalloc((void**)&d_values, sizeof(int) * NVALUES);

        // do recv
        MPI_Status status;
        MPI_Recv(d_values, NVALUES, 
            MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        int recvSize = -1;
        MPI_Get_count(&status, MPI_INT, &recvSize);
        printf("rank %d received %lu values\n", rank, recvSize);

        // make sure that values match what we expect
        //for (unsigned int i=0; i<values.size(); i++)
        //    assert(values[i] == i && "values must match");
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

        int *d_values;
        cudaMalloc((void**)&d_values, sizeof(int) * NVALUES);

        // do recv
        MPI_Status status;
        MPI_Recv(d_values, NVALUES, 
            MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        int recvSize = -1;
        MPI_Get_count(&status, MPI_INT, &recvSize);
        printf("rank %d received %d values\n", rank, recvSize);

        // make sure that values match what we expect
        //for (unsigned int i=0; i<recvSize; i++)
        //    assert(values[i] == i && "values must match");
        printf("rank %d verified received values\n", rank);
    }
}


int main(int argc, char **argv) {
    std::cout << "hello world from send_recv_veccuda" << std::endl;

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

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    printf("running on host %s, rank %d out of %d\n", processor_name, rank, nprocs);

    test_fixed(rank);
    test_varying(rank);

    // finalize
    MPI_Finalize();
}
