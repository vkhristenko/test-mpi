#include <mpi.h>
#include <iostream>
#include <cstdlib>
#include <thread>
#include <vector>

int main(int argc, char **argv) {
    std::cout << "hello from server" << std::endl;
    int const nthreadsClient = std::stoi(argv[1]);
    int const niters = std::stoi(argv[2]);

    // init
    int provided;
    MPI_Init_thread(nullptr, nullptr, MPI_THREAD_MULTIPLE, &provided);
    assert(provided == MPI_THREAD_MULTIPLE);

    // total processes in this comm
    int nprocs;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    printf("server: nprocs = %d\n", nprocs);

    // our rank (process id)
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    printf("server: rank = %d\n", rank);
    assert(rank == nprocs-1 && "our server should be the last in the ranks");

    // total threads for teh server
    int const nthreads = nthreadsClient * rank;
    printf("server: nthreads = %d\n", nthreads);

    // do work function
    auto doWork = [&rank, &nthreads, &nthreadsClient, &niters](int const tid) {
        // buffer
        std::vector<int> data(100, 0);

        // compute from who receive and to who we send the results
        int const srank = tid/nthreadsClient;
        int const stid = tid%nthreadsClient;

        for (int i=0; i<niters; i++) {
            // receive input
            MPI_Recv(data.data(), data.size(), MPI_INT, srank, stid, 
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // process
            int sum = 0;
            for (auto const value : data)
                sum += value;

            printf("server thread %d iter %d: sum = %d\n", tid, i, sum);

            // send it back
            MPI_Send(&sum, 1, MPI_INT, srank, stid, MPI_COMM_WORLD);
        }
    };

    // init threads
    std::vector<std::thread> threads;
    for (int i=0; i<nthreads; i++)
        threads.push_back(std::thread{doWork, i});

    // collect threads
    for (auto& t : threads)
        t.join();

    // finalize
    MPI_Finalize();
}
