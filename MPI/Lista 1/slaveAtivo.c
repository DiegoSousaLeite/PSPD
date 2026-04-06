#include <stdio.h>
#include <mpi.h>

int main (int argc, char ** argv) {
    MPI_Init(&argc, &argv);
    int myRank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (myRank == 0) {
            printf("MESTRE: Não há slaves (size=%d).\n", size);
        }
        MPI_Finalize();
        return 0;
    }

    if (myRank == 0) {

        for (int n = 1; n < size; n++) {
            int sinal;
            MPI_Recv(&sinal, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("MESTRE: Recebi sinal de vida do rank %d \n", sinal);
        }

    } else {
        int sinal = myRank; 
        MPI_Send(&sinal, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        printf("SLAVE %d: Enviei sinal de vida ao mestre\n", myRank);
    }

    MPI_Finalize();
    return 0;
}