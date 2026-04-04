#include <stdio.h>
#include <mpi.h>

int main (int argc, char ** argv) {
    MPI_Init(&argc, &argv);
    int myRank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Dois for para ficar paralelo
    if (myRank == 0) {
        // FASE 1: Enviar o vetor para todos os slaves
        for (int n = 1; n < size; n++) {
            int vet[5] = {10*n, 20*n, 30*n, 40*n, 50*n}; 

            MPI_Send(vet, 5, MPI_INT, n, 0, MPI_COMM_WORLD);
            printf("MESTRE: Enviei o vetor para o rank %d\n", n);
        }

        // FASE 2: Receber os vetores alterados
        for (int n = 1; n < size; n++) {
            int resultado[5];
            MPI_Recv(resultado, 5, MPI_INT, n, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            printf("MESTRE: Recebi do rank %d: [%d, %d, %d, %d, %d]\n", 
                    n, resultado[0], resultado[1], resultado[2], resultado[3], resultado[4]);
        }
    } else {
        /* Nó Escravo */
        int recebido[5];
        
        MPI_Recv(recebido, 5, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        for (int i = 0; i < 5; i++) {
            recebido[i] = recebido[i] * 2;
        }
        
        MPI_Send(recebido, 5, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}