#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define CHUNKSIZE 10

/*
Ce programme MPI répartit le traitement d’un tableau de 100 entiers entre plusieurs processus, où le maître envoie des portions
aux esclaves pour qu’ils les élèvent au carré, puis récupère les résultats, en terminant par leur envoyer un signal d’arrêt.
*/

void generate_data(int* data, int n) {
    for (int i = 0; i < n; i++) {
        data[i] = i;
    }
}

void print_data(int* data, int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");
}

void process_data(int* data, int size) {
    for (int i = 0; i < size; i++) {
        data[i] = data[i] * data[i];
    }
}

int main(int argc, char** argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n = 100;
    int* data = NULL;
    int* buffer = (int*)malloc(CHUNKSIZE * sizeof(int));

    if (rank == 0) {
        data = (int*)malloc(n * sizeof(int));
        generate_data(data, n);

        printf("Données à distribuer : \n");
        print_data(data, n);

        int chunk_count = (n + CHUNKSIZE - 1) / CHUNKSIZE;
        int chunk_idx = 0;


        for (int i = 1; i < size && chunk_idx < chunk_count; i++, chunk_idx++) {
            int start_idx = chunk_idx * CHUNKSIZE;
            int send_size = (start_idx + CHUNKSIZE <= n) ? CHUNKSIZE : n - start_idx;
            MPI_Send(&data[start_idx], send_size, MPI_INT, i, 0, MPI_COMM_WORLD);
        }


        for (int i = 0; i < chunk_count; i++) {
            MPI_Status status;
            int received_size;
            MPI_Recv(buffer, CHUNKSIZE, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &received_size);

            printf("Maître reçoit des données traitées : \n");
            print_data(buffer, received_size);

            if (chunk_idx < chunk_count) {
                int start_idx = chunk_idx * CHUNKSIZE;
                int send_size = (start_idx + CHUNKSIZE <= n) ? CHUNKSIZE : n - start_idx;
                MPI_Send(&data[start_idx], send_size, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
                chunk_idx++;
            } else {
                // Envoyer un signal de terminaison
                MPI_Send(NULL, 0, MPI_INT, status.MPI_SOURCE, 1, MPI_COMM_WORLD);
            }
        }


        for (int i = 1; i < size; i++) {
            MPI_Send(NULL, 0, MPI_INT, i, 1, MPI_COMM_WORLD);
        }

        free(data);
    } else {
        while (1) {
            MPI_Status status;
            int received_size;
            MPI_Recv(buffer, CHUNKSIZE, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            if (status.MPI_TAG == 1) {
                break;
            }

            MPI_Get_count(&status, MPI_INT, &received_size);
            process_data(buffer, received_size);
            MPI_Send(buffer, received_size, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    free(buffer);
    MPI_Finalize();
    return 0;
}
