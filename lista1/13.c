#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FLOAT 10

int world_size;

void run(int rank) {
    int valor = 10;
    MPI_Status status;

    while(1) {
        if(rank == 0) {
            valor--;
            MPI_Send(&valor, 1, MPI_INT, (rank + 1) % world_size, 1, MPI_COMM_WORLD);
            if(valor == 0) break;
        }
        MPI_Recv(&valor, 1, MPI_INT, (rank+world_size-1) % world_size, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        if(rank != 0)
            MPI_Send(&valor, 1, MPI_INT, (rank + 1) % world_size, 1, MPI_COMM_WORLD);
            if(valor == 0) break;
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int p_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &p_rank);

    run(p_rank);

    MPI_Finalize();
}