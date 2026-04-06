#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/*
 * vetorCompartilhado.c
 *
 * O MASTER (rank 0) inicializa um vetor e o compartilha com todos os
 * processos usando MPI_Bcast. Em seguida cada processo imprime o vetor
 * recebido. Para manter a saída ordenada por rank, utilizamos
 * MPI_Barrier e um laço que permite que apenas o rank atual imprima.
 *
 * Uso: mpirun -n <P> ./vetorCompartilhado  (P >= 1)
 */

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int N = 8; /* tamanho fixo do vetor compartilhado */
    int *vet = malloc(N * sizeof(int));
    if (!vet) {
        if (rank == 0) fprintf(stderr, "Erro de memória\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (rank == 0) {
        /* Mestre inicializa o vetor */
        for (int i = 0; i < N; ++i) vet[i] = (i + 1) * 11; /* 11,22,33,... exemplo */
    }

    /*
     * MPI_Bcast: o processo root (rank 0) envia o buffer `vet` com N
     * inteiros para todos os processos. Todos os processos, incluindo o
     * root, devem chamar esta função. Após o retorno, todos têm o mesmo
     * conteúdo em `vet`.
     */
    MPI_Bcast(vet, N, MPI_INT, 0, MPI_COMM_WORLD);

    /*
     * Impressão ordenada: fazemos um laço de 0..size-1; apenas o processo
     * cujo índice coincide com `r` imprime, os demais esperam na barrier.
     */
    for (int r = 0; r < size; ++r) {
        if (r == rank) {
            printf("RANK %d: vetor = [", rank);
            for (int i = 0; i < N; ++i) {
                printf("%d%s", vet[i], (i+1==N)?"":", ");
            }
            printf("]\n");
            fflush(stdout);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    free(vet);
    MPI_Finalize();
    return 0;
}
