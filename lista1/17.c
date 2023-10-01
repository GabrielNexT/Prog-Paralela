#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <stdlib.h>

#define MAX_FLOAT 10

int world_size;

void master() {
    int* v = malloc(sizeof(int) * 5);
    for(int i = 0; i < 5; i++) {
        v[i] = rand() % 100;
    }
    MPI_Bcast(v, 5, MPI_INT, 0, MPI_COMM_WORLD);
}

void workers(int rank) {
    MPI_Status status;
    int size = 1;
    int* v = malloc(sizeof(int) * size);
    MPI_Bcast(v, size, MPI_INT, 0, MPI_COMM_WORLD);
    
    for(int i = 0; i < size; i++) {
        printf("%d ", v[i]);
    }
    printf("\n");
}

int main(int argc, char** argv) {
    srand(time(NULL));
    MPI_Init(&argc, &argv);

    int p_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &p_rank);

    if(p_rank == 0) master();
    else workers(p_rank);

    MPI_Finalize();
}