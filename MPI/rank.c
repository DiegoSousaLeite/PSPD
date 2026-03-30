#include <stdio.h>
#include <unistd.h>
#include <mpi.h>

int main (int argc, char ** argv) {
    MPI_Init(&argc, &argv);
    int myRank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    printf("Ola, mundo! %d tem rank %d\n", getpid(), myRank);
    MPI_Finalize();
    return 0;
} /* fim-main */