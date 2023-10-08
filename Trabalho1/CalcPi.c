#include <stdio.h>
#include <mpi.h>

const int N = (int) 1e9;

int main(int argc, char *argv[])
{
  int rank, size;
  double pi_local = 0.0;
  double pi = 0.0;
  double start_time, end_time;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  start_time = MPI_Wtime();

  // Divide o número de trapézios igualmente entre os processos
  long num_trapezoids = N / size;
  long start = rank * num_trapezoids;
  long end = start + num_trapezoids;

  for (long i = start; i < end; i++)
  {
    double x = (double)((i + 0.5) / N);
    pi_local += 4.0 / (1.0 + x * x);
  }

  // Reduz todas as somas locais para calcular o valor final de pi
  MPI_Reduce(&pi_local, &pi, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  end_time = MPI_Wtime();

  if (rank == 0)
  {
    pi /= N;
    printf("pi = %f\n", pi);
    printf("Tempo de execução: %f segundos\n", end_time - start_time);
  }

  MPI_Finalize();
  return 0;
}