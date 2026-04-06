#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/*
 * vetorProcessos.c
 *
 * Distribui os elementos de um vetor entre todos os processos, assumindo
 * que o número de processos é igual ao número de elementos do vetor.
 * Cada processo receberá exatamente 1 elemento (usando MPI_Scatter).
 *
 * Uso: mpirun -n <P> ./vetorProcessos  (P >= 1)
 */

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* Cada processo receberá 1 inteiro */
    int myval;

    if (rank == 0) {
        /* Mestre cria um vetor com 'size' elementos */
        int *vet = malloc(size * sizeof(int));
        if (!vet) {
            fprintf(stderr, "Erro de memória\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        for (int i = 0; i < size; ++i) {
            vet[i] = (i+1) * 10; 
        }

        /* Envia 1 elemento para cada processo (incluindo ele próprio) */
        MPI_Scatter(vet, 1, MPI_INT, &myval, 1, MPI_INT, 0, MPI_COMM_WORLD);

        free(vet);
    } else {
        /* Cada slave recebe 1 inteiro do mestre */
        MPI_Scatter(NULL, 0, MPI_DATATYPE_NULL, &myval, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }

    /* Cada processo imprime o elemento que recebeu */
    printf("RANK %d recebeu valor %d\n", rank, myval);

    MPI_Finalize();
    return 0;
}
