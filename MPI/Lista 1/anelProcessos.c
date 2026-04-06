#include <stdio.h>
#include <mpi.h>



int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        if (rank == 0) {
            printf("Execute com pelo menos 2 processos: mpirun -n 4 ./anelProcessos\n");
        }
        MPI_Finalize();
        return 0;
    }

    int next = (rank + 1) % size;

    int token;

    if (rank == 0) {
        token = 0; 
        printf("MASTER: gerando token=%d e enviando para rank %d\n", token, next);
        MPI_Send(&token, 1, MPI_INT, next, 0, MPI_COMM_WORLD);

        MPI_Recv(&token, 1, MPI_INT, size-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("MASTER: recebi token de volta do rank %d com valor=%d\n", size-1, token);
    } else {

        MPI_Recv(&token, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("RANK %d: recebi token=%d do rank %d\n", rank, token, rank-1);

        token ++; 

        MPI_Send(&token, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
        printf("RANK %d: enviei token=%d para rank %d\n", rank, token, next);
    }

    MPI_Finalize();
    return 0;
}
