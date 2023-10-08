#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

const int iterations = 1e9;

int world_size;

int calc(int rank) {
    int count = 0;
    double x, y, z;
    for(int i = rank; i < iterations; i += world_size) {
        x = (double) rand() / RAND_MAX;
        y = (double) rand() / RAND_MAX;
        z = x * x + y * y;
        if( z <= 1 ) count++;
    }
    return count;
}

void master() {
    int local_count = calc(0), global_count = 0;

    MPI_Reduce(&local_count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    double pi = pi = ((double) global_count / iterations) * 4;

    printf("Aproximação de PI é = %g\n", pi);
}

void worker(int rank) {
    int count = calc(rank);

    MPI_Reduce(&count, &count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
}

void print_elapsed_time(double begin) {
    double current_time = MPI_Wtime();
    printf("Elapsed time %.9f (s)\n", current_time-begin);
}

int is_master(int rank) {
    return rank == 0;
}


int main(int argc, char** argv) {
    srand(time(NULL));

    MPI_Init(&argc, &argv);
    double begin_time = MPI_Wtime();

    int p_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &p_rank);

    if(is_master(p_rank)) 
        printf("Number of processes %d\n", world_size);

    if(is_master(p_rank)) master();
    else worker(p_rank);

    MPI_Finalize();

    if(is_master(p_rank)) print_elapsed_time(begin_time);
}
