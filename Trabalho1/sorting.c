#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <mpi.h>

const int ARRAY_SIZE = 1e6 * 40;
int world_size;

int compare_int (const void* a, const void* b) {
  return *(int*)a - *(int*)b;
}

void run_quick_sort(int* array, int n) {
  qsort(array, n, sizeof(int), compare_int);
}

void print_array(int *array, int size) {
  for(int i = 0; i < size; i++) {
    printf("%d ", array[i]);
  }
  printf("\n");
}

void split_and_send_to_workers(int *values, int* begin_idx, int* end_idx) {
  int quo = ARRAY_SIZE/world_size, offset = ARRAY_SIZE % world_size;

  for(int i = 0, last = 0; i < world_size; i++) {
    begin_idx[i] = last;
    end_idx[i] = last + quo + (i == world_size - 1 ? offset : 0);
    last += quo;
  }

  for(int i = 1; i < world_size; i++) {
    int cont = end_idx[i] - begin_idx[i];
    MPI_Request request;
    MPI_Isend(values + begin_idx[i], cont, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
  }
}

void get_from_workers(int *values, int* begin_idx, int* end_idx) {
  for(int i = 1; i < world_size; i++) {
    int size;
    MPI_Request request;
    MPI_Status status;
    MPI_Probe(i, 0, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_INT, &size);

    MPI_Irecv(values + begin_idx[i], size, MPI_INT, i, 0, MPI_COMM_WORLD, &request);
  }
}

void merge(int *values, int* begin_idx, int* end_idx) {
  int menor = RAND_MAX, bucket = -1, it = 0;
  int* tmpArray = malloc(sizeof(int) * ARRAY_SIZE);

  while(1) {
    for(int i = 0; i < world_size; i++) {
      if(begin_idx[i] >= end_idx[i]) continue;

      if(values[begin_idx[i]] < menor) {
        menor = values[begin_idx[i]];
        bucket = i;
      }
    }

    if(bucket == -1) break;

    tmpArray[it++] = menor;
    begin_idx[bucket]++;

    menor = RAND_MAX;
    bucket = -1;
  }

  free(values);
  values = tmpArray;
}

void master() {
  int *values = malloc(sizeof(int) * ARRAY_SIZE);
  int *begin_idx = malloc(sizeof(int) * world_size), *end_idx = malloc(sizeof(int) * world_size);

  for(int i = 0; i < ARRAY_SIZE; i++) {
    values[i] = rand() % 10;
  }

  split_and_send_to_workers(values, begin_idx, end_idx);
  run_quick_sort(values, end_idx[0]);
  get_from_workers(values, begin_idx, end_idx);
  merge(values, begin_idx, end_idx);
}

void worker() {
  int *values;
  int size, source, tag;

  MPI_Status status;
  MPI_Request request;

  MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
  MPI_Get_count(&status, MPI_INT, &size);
  values = malloc(sizeof(int) * size);
  source = status.MPI_SOURCE, tag = status.MPI_TAG;

  MPI_Recv(values, size, MPI_INT, source, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  run_quick_sort(values, size);
  MPI_Isend(values, size, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
}

void print_elapsed_time(double begin) {
  double current_time = MPI_Wtime();
  printf("Elapsed time %.9f (s)\n", current_time-begin);
}
int main(int argc, char** argv) {
    srand(time(NULL));

    MPI_Init(&argc, &argv);

    double begin = MPI_Wtime();

    int p_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &p_rank);

    if(p_rank == 0) master();
    else worker();

    MPI_Finalize();

    if(p_rank == 0)
      print_elapsed_time(begin);
}