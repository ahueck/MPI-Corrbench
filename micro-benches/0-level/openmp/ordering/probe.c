#include "nondeterminism.h"

#include <mpi.h>
#include <stdlib.h>

#define BUFFER_LENGTH_INT 2
#define BUFFER_LENGTH_BYTE (BUFFER_LENGTH_INT * sizeof(int))

#define NUM_THREADS 2

// race between to MPI_Probe calls is possible
// if One threads porobes for a msg (A) it is possible that another thread probes for this message as well and receives
// it before the first thread calls the matching send (B) this may lead to insufficient msg buffer allocated in this
// example

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

  bool has_race = false;

#pragma omp parallel num_threads(NUM_THREADS)
  {
    if (rank == 0) {
#pragma omp sections
      {
#pragma omp section
        {
          int value[2] = {-2, -2};
          MPI_Send(value, 2, MPI_INT, 1, 0, MPI_COMM_WORLD);
        }
#pragma omp section
        {
          int value = -1;
          MPI_Send(&value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        }
      }

    } else if (rank == 1) {
      MPI_Status status;
      MPI_Probe(0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);  // A

      int count;
      MPI_Get_count(&status, MPI_INT, &count);

      int *value = (int *)malloc(sizeof(int) * count);
      MPI_Recv(value, count, MPI_INT, 0, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);  // B

      const bool thread_race = (count == 1 && value[0] != -1) || (count == 2 && value[0] != -2);

#pragma omp critical
      has_race = (has_race || thread_race);

      //#pragma omp critical
      //      printf("Status race %i: %i %i\n", has_race, count, value[0]);
      // sometimes:
      //      Status race 0: 2 -2
      //      Status race 0: 1 -1
      // often:
      //      Status race 0: 2 -2
      //      Status race 1: 2 -1

      free(value);
    }
  }

  MPI_Finalize();

  if (rank == 1) {
    has_error_manifested(has_race);
    if (has_race)
      printf("Has race\n");
  }
  return 0;
}
