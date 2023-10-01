#include <stdio.h>
#include <mpi.h>
#include <time.h>
#include <stdlib.h>

#define MAX_FLOAT 10

int world_size;

void master() {
    float* vetor = malloc(MAX_FLOAT * sizeof(float));
    MPI_Status estado;
    int vetor_size;
    for(int i = 0; i < world_size - 1; i++){
        MPI_Recv(vetor, MAX_FLOAT, MPI_FLOAT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &estado);
        MPI_Get_count(&estado, MPI_INT, &vetor_size);
        printf("Processo master recebeu %d números. Origem da mensagem = %d, etiqueta = %d\n", vetor_size, estado.MPI_SOURCE, estado.MPI_TAG);
    }
}

void workers(int rank) {
    srand(time(NULL)+rank);
    int size = rand() % MAX_FLOAT + 1;

    float* vetor = malloc(size * sizeof(float));
    for(int i = 0; i < size; i++) {
        vetor[i] = rand() % 100;
    }

    MPI_Send(vetor, size, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int p_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &p_rank);

    if(p_rank == 0) master();
    else workers(p_rank);

    MPI_Finalize();
}