#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/*
 * imprimirN.c
 *
 * Lê N do argv (N > 0) e imprime os N primeiros números naturais
 * distribuindo o trabalho entre os processos. Assume-se P = número de
 * processos <= N (se P > N, alguns ranks ficarão sem trabalho).
 *
 * Distribuição: base = N / P; rem = N % P; os ranks < rem recebem base+1
 * elementos; os demais recebem base elementos. Impressão é ordenada por
 * rank usando MPI_Barrier para sincronizar.
 *
 * Uso: mpirun -n <P> ./imprimirN N
 */

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int N = 0;
    if (rank == 0) {
        if (argc < 2) {
            fprintf(stderr, "Uso: mpirun -n <P> %s N\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        N = atoi(argv[1]);
        if (N <= 0) {
            fprintf(stderr, "N deve ser inteiro positivo\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    /* transmite N a todos */
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (size > N && rank == 0) {
        fprintf(stderr, "Aviso: numero de processos (%d) > N (%d). Alguns ranks ficarão sem trabalho.\n", size, N);
    }

    /*
     * Distribuição balanceada dos N elementos entre `size` processos:
     * - `base` é a quantidade mínima de elementos por processo (divisão inteira).
     * - `rem` é o resto da divisão; os primeiros `rem` ranks recebem 1 elemento a mais.
     *
     * Exemplo: N=10, size=4 -> base=2, rem=2
     *  - ranks 0..rem-1 (0 e 1) recebem base+1 = 3 elementos
     *  - ranks rem..size-1 (2 e 3) recebem base = 2 elementos
     */
    int base = N / size;    /* elementos mínimos por processo */
    int rem = N % size;     /* quantos processos receberão +1 elemento */

    /*
     * `my_count` = quantos elementos este processo deve imprimir.
     * Se rank < rem, recebe (base+1), caso contrário recebe base.
     */
    int my_count = (rank < rem) ? (base + 1) : base;

    
    int my_start;
    if (rank < rem) my_start = rank * (base + 1);
    else my_start = rem * (base + 1) + (rank - rem) * base;

    /* imprimir ordenadamente por rank */
    for (int r = 0; r < size; ++r) {
        MPI_Barrier(MPI_COMM_WORLD);
        if (r == rank) {
            if (my_count > 0) {
                printf("RANK %d: imprimindo %d numeros (indices %d..%d):", rank, my_count, my_start, my_start + my_count - 1);
                for (int i = 0; i < my_count; ++i) {
                    int val = my_start + i + 1; /* números naturais: 1..N */
                    printf(" %d", val);
                }
                printf("\n");
            } else {
                printf("RANK %d: sem elementos para imprimir\n", rank);
            }
            fflush(stdout);
        }
    }

    MPI_Finalize();
    return 0;
}
