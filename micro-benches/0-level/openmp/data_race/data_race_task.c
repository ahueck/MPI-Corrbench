#include "nondeterminism.h"

#include <mpi.h>
#include <stdbool.h>
#include <stdlib.h>

// This test is loosely based on test DRB136 of the data race bench.
// See https://github.com/LLNL/dataracebench
int main(int argc, char *argv[]) {
  int provided;
  const int requested = MPI_THREAD_MULTIPLE;

  MPI_Init_thread(&argc, &argv, requested, &provided);
  if (provided < requested) {
    has_error_manifested(false);
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  int size;
  int rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int a = 1;
  int b = 2;
  int c = 3;
  int d = 0;

  MPI_Comm other_comm_world;
  MPI_Comm_split(MPI_COMM_WORLD, 0, rank, &other_comm_world);

  MPI_Comm final_comm_world;
  MPI_Comm_split(MPI_COMM_WORLD, 0, rank, &final_comm_world);

#pragma omp parallel
#pragma omp single
  {
#pragma omp task depend(in : a)  // depend(inout : c)
    {
      int reduced_a = 0;
      MPI_Reduce(&a, &reduced_a, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
      c += reduced_a;
    }
#pragma omp task depend(in : b)  // depend(inout : c)
    {
      int reduced_b = 0;
      MPI_Reduce(&b, &reduced_b, 1, MPI_INT, MPI_SUM, 0, other_comm_world);
      c += reduced_b;
    }
#pragma omp task depend(in : c) depend(out : d)
    { MPI_Reduce(&c, &d, 1, MPI_INT, MPI_SUM, 0, final_comm_world); }
  }

  if (rank == 0) {
    const bool error = (d != 12);
    has_error_manifested(error);
    if (error)
      printf("Error %i\n", d);
  }

  MPI_Comm_free(&other_comm_world);
  MPI_Comm_free(&final_comm_world);

  MPI_Finalize();

  return 0;
}
