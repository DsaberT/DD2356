#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    int world_rank, world_size, provided;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    printf("Hello World from rank %d from %d processes!\n", world_rank, world_size);

    MPI_Finalize();
}
