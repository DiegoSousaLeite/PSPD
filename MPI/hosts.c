#include <stdio.h>
#include <unistd.h>
#include <mpi.h>

int main (int argc, char ** argv) {
    MPI_Init(&argc, &argv);
    int myRank, size;
    char maquina[200];

    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    gethostname(maquina, 199);

    if (myRank == 0) {
        for (int n=1; n<size; n++) {
            int i=10*n;
            MPI_Send(&i, 1, MPI_INT, n, 0, MPI_COMM_WORLD);
            printf("MESTRE: Enviei %d para o rank %d\n", i, n);
        } /* fim-for */
    } else {
        /* Estamos no nó escravo */
        int recebido;
        MPI_Recv(&recebido, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("ESCRAVO(%d / %s) recebeu %d\n", myRank, maquina, recebido);
    } /* fim-if */

    MPI_Finalize();
    return 0;
} /* fim-main */