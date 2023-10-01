#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <stdlib.h>

#define MAX_FLOAT 10

int world_size;

void master() {
    MPI_Status status;
    for(int i = 0; i < world_size - 1; i++) {
        int size;
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &size);
        int* v = malloc(sizeof(int)*size);
        int source = status.MPI_SOURCE, tag = status.MPI_TAG;
        MPI_Recv(v, size, MPI_INT, source, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Recebi %d numeros do processo %d.\n", size, source);
    }
}

void worker(int rank) {
    srand(time(NULL)+rank);
    int size = rand() % MAX_FLOAT + 1;

    int* v = malloc(size * sizeof(int));
    for(int i = 0; i < size; i++) {
        v[i] = rand() % 100;
    }

    MPI_Send(v, size, MPI_INT, 0, 0, MPI_COMM_WORLD);
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int p_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &p_rank);

    if(p_rank == 0) master();
    else worker(p_rank);

    MPI_Finalize();
}