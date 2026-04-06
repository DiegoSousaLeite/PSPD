#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size, N;
    MPI_Status status;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Apenas o processo 0 lê o valor de N da linha de comando
    if (rank == 0) {
        if (argc < 2) {
            printf("Uso: mpirun -np <num_processos> %s <N>\n", argv[0]);
            MPI_Finalize();
            return 1;
        }
        N = atoi(argv[1]);
    }
    
    // Broadcast N para todos os processos usando Send/Recv
    if (rank == 0) {
        for (int i = 1; i < size; i++) {
            MPI_Send(&N, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(&N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
    }
    
    // Cálculo dos ranks vizinhos (anel circular)
    int next_rank = (rank + 1) % size;
    int prev_rank = (rank - 1 + size) % size;
    
    // Contagem sequencial com token passing em anel
    int num = rank;  // Cada processo começa com seu número (rank, rank+size, rank+2*size, ...)
    
    // Cada processo entra no loop e imprime enquanto houver números
    while (num <= N) {
        printf("Processo %d imprime: %d\n", rank, num);
        fflush(stdout);
        
        // Calcula o próximo número que este processo imprimiria
        num += size;
        
        // Evita deadlock: processos pares enviam antes de receber, impares recebem antes de enviar
        if (rank % 2 == 0) {
            // Processos pares: envia primeiro
            MPI_Send(&num, 1, MPI_INT, next_rank, 0, MPI_COMM_WORLD);
            MPI_Recv(&num, 1, MPI_INT, prev_rank, 0, MPI_COMM_WORLD, &status);
        } else {
            // Processos impares: recebe primeiro
            MPI_Recv(&num, 1, MPI_INT, prev_rank, 0, MPI_COMM_WORLD, &status);
            MPI_Send(&num, 1, MPI_INT, next_rank, 0, MPI_COMM_WORLD);
        }
    }
    
    MPI_Finalize();
    return 0;
}
