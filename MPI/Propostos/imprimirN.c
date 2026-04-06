#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
	int rank, size;
	int n;
	int value;
	int ack = 1;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == 0) {
		if (argc < 2) {
			fprintf(stderr, "Uso: mpirun -np <processos> ./imprimirN <N>\n");
			n = -1;
		} else {
			char *endptr;
			long parsed = strtol(argv[1], &endptr, 10);

			if (*endptr != '\0' || parsed < 0) {
				fprintf(stderr, "Erro: N deve ser inteiro >= 0.\n");
				n = -1;
			} else {
				n = (int)parsed;
			}
		}

		for (int p = 1; p < size; p++) {
			MPI_Send(&n, 1, MPI_INT, p, 0, MPI_COMM_WORLD);
		}

		if (n < 0) {
			MPI_Finalize();
			return 1;
		}

		value = 0;
		while (value <= n) {
			int target = value % size;

			if (target == 0) {
				printf("Processo %d imprimiu: %d\n", rank, value);
				fflush(stdout);
			} else {
				MPI_Send(&value, 1, MPI_INT, target, 1, MPI_COMM_WORLD);
				MPI_Recv(&ack, 1, MPI_INT, target, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			}

			value++;
		}

		value = -1;
		for (int p = 1; p < size; p++) {
			MPI_Send(&value, 1, MPI_INT, p, 1, MPI_COMM_WORLD);
		}
	} else {
		MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		if (n < 0) {
			MPI_Finalize();
			return 1;
		}

		while (1) {
			MPI_Recv(&value, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			if (value == -1) {
				break;
			}

			printf("Processo %d imprimiu: %d\n", rank, value);
			fflush(stdout);
			MPI_Send(&ack, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
		}
	}

	MPI_Finalize();
	return 0;
}
