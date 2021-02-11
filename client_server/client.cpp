#include <iostream>
#include <cstdlib>
#include <thread>
#include <vector>

#include <mpi.h>

int main(int argc, char **argv) {
    printf("hello from client\n");
    int const nthreads = std::stoi(argv[1]);
    int const niters = std::stoi(argv[2]);

    // init
    int provided;
    MPI_Init_thread(nullptr, nullptr, MPI_THREAD_MULTIPLE, &provided);
    assert(provided == MPI_THREAD_MULTIPLE);

    // rank
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    printf("client: rank = %d\n", rank);

    int nprocs;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    printf("client %d: nprocs = %d\n", rank , nprocs);
    assert(rank < nprocs-1 && "clients should all have rank < nprocs-1");

    auto doWork = [&rank, &nthreads, &niters, &nprocs](int const tid) {
        std::vector<int> data(100, 0);
        int tmp = 0;
        for (int i=0; i<100; i++) { data[i] = i; tmp+=i; }

        for (int i=0; i<niters; i++) {
            // send
            MPI_Send(data.data(), data.size(), MPI_INT, nprocs-1, tid, MPI_COMM_WORLD);

            // 
            int sum = -1;
            MPI_Recv(&sum, 1, MPI_INT, nprocs-1, tid, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            assert(sum == tmp);

            printf("client %d thread %d iter %d: sum is verified = %d\n", 
                rank, tid, i, sum);
        }
    };

    std::vector<std::thread> threads;
    for (int i=0; i<nthreads; i++)
        threads.push_back(std::thread{doWork, i});

    for (auto& t : threads) t.join();

    MPI_Finalize();

    return 0;
}
