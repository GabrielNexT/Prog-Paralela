#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FLOAT 10

int world_size;

void run(int rank) {
    char message[200];
    int total = world_size;

    do {
        MPI_Send(message, strlen(message)+1, MPI_CHAR, world_size-1, 1, MPI_COMM_WORLD);
    } while (--world_size);

    for(int p = 0; p < total; p++) {
        MPI_Status status;
        MPI_Recv(message, strlen(message)+1, MPI_CHAR, p, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        printf("Eu %d, recebi uma mensagem do processo %d\n", rank, p);
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