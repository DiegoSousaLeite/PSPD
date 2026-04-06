#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        if (rank == 0) {
            printf("Execute com exatamente 2 processos (mpirun -n 2). size=%d\n", size);
        }
        MPI_Finalize();
        return 0;
    }

    if (rank == 0) {
        /* Vetor completo inicializado pelo processo 0 (mestre) */
        int vet[8] = {1,2,3,4,5,6,7,8};

        /*
         * Mestre -> envia apenas a segunda metade do vetor (índices 4..7)
         * para o slave (rank 1). Esta mensagem contém os dados que o
         * slave deve imprimir.
         */
        MPI_Send(&vet[4], 4, MPI_INT, 1, 0, MPI_COMM_WORLD);

        /* Mestre imprime a primeira metade (índices 0..3) localmente */
        printf("MESTRE: Parte 0-3: [%d, %d, %d, %d]\n",
               vet[0], vet[1], vet[2], vet[3]);

        /*
         * `cmd` é uma mensagem de controle (comando). Aqui usamos um
         * inteiro arbitrário para sinalizar ao slave: "vc pode imprimir".
         * Não carrega dados do vetor — só coordena a ordem/fluxo.
         */
        int cmd = 1;
        MPI_Send(&cmd, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);

        /*
         * `ack` (acknowledgement) é a confirmação enviada pelo slave quando
         * ele terminou de imprimir. O mestre espera esse `ack` para
         * garantir que o slave concluiu antes de prosseguir.
         */
        int ack;
        MPI_Recv(&ack, 1, MPI_INT, 1, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
        /*
         * Slave: recebe os 4 inteiros (segunda metade) enviados pelo mestre.
         * Estes são apenas os dados que o slave vai imprimir.
         */
        int parte[4];
        MPI_Recv(parte, 4, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        /*
         * Espera o comando (`cmd`) do mestre para saber quando imprimir.
         * Poderíamos imprimir imediatamente, mas usar `cmd` permite controle
         * explícito da ordem e garante sincronização mais fina.
         */
        int cmd;
        MPI_Recv(&cmd, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        /* imprime sua parte do vetor (recebida previamente) */
        printf("SLAVE: Parte 4-7: [%d, %d, %d, %d]\n",
               parte[0], parte[1], parte[2], parte[3]);

        /*
         * Envia `ack` de volta ao mestre indicando que a impressão foi feita.
         * Esse ack é só um inteiro usado como confirmação/sincronização.
         */
        int ack = 1;
        MPI_Send(&ack, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
