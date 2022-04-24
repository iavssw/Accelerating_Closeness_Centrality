#define ADJ_MAT_SIZE 1000

#define BLOCK_SIZE 100

#define LOOP_SIZE ADJ_MAT_SIZE / BLOCK_SIZE
// dont use 0 row and col

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void FW_tile(short C_row, short C_col, short A_row, short A_col, short B_row, short B_col, unsigned short *adj_mat) {
    int i, j, k;
    unsigned short newpath;

    unsigned short C[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short A[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short B[BLOCK_SIZE][BLOCK_SIZE];

    printf("cords: %d, %d\n", C_row, C_col);

    // copy C into local buffer
    for (i = 0; i < BLOCK_SIZE; i++) {
        for (j = 0; j < BLOCK_SIZE; j++) {
            C[i][j] = adj_mat[(ADJ_MAT_SIZE * BLOCK_SIZE * C_row + ADJ_MAT_SIZE * i) + (BLOCK_SIZE * C_col + j)];
        }
    }

    // copy A into local buffer
    for (i = 0; i < BLOCK_SIZE; i++) {
        for (j = 0; j < BLOCK_SIZE; j++) {
            A[i][j] = adj_mat[(ADJ_MAT_SIZE * BLOCK_SIZE * A_row + ADJ_MAT_SIZE * i) + (BLOCK_SIZE * A_col + j)];
        }
    }

    // copy B into local buffer
    for (i = 0; i < BLOCK_SIZE; i++) {
        for (j = 0; j < BLOCK_SIZE; j++) {
            B[i][j] = adj_mat[(ADJ_MAT_SIZE * BLOCK_SIZE * B_row + ADJ_MAT_SIZE * i) + (BLOCK_SIZE * B_col + j)];
        }
    }

    FW_core(C, A, B);

    for (k = 0; k < BLOCK_SIZE; k++) {
        for (i = 0; i < BLOCK_SIZE; i++) {
            for (j = 0; j < BLOCK_SIZE; j++) {
                // // CPU version
                // // only when valid path
                // if ((adj_mat[(ADJ_MAT_SIZE * BLOCK_SIZE * A_row + ADJ_MAT_SIZE * i) + (BLOCK_SIZE * A_col + k)] != USHRT_MAX)
                //     && (adj_mat[(ADJ_MAT_SIZE * BLOCK_SIZE * B_row + ADJ_MAT_SIZE * k) + (BLOCK_SIZE * B_col + j)] != USHRT_MAX)) {

                //     newpath = adj_mat[(ADJ_MAT_SIZE * BLOCK_SIZE * A_row + ADJ_MAT_SIZE * i) + (BLOCK_SIZE * A_col + k)]
                //     + adj_mat[(ADJ_MAT_SIZE * BLOCK_SIZE * B_row + ADJ_MAT_SIZE * k) + (BLOCK_SIZE * B_col + j)];

                //     if (newpath < adj_mat[(ADJ_MAT_SIZE * BLOCK_SIZE * C_row + ADJ_MAT_SIZE * i) + (BLOCK_SIZE * C_col + j)]) {
                //         adj_mat[(ADJ_MAT_SIZE * BLOCK_SIZE * C_row + ADJ_MAT_SIZE * i) + (BLOCK_SIZE * C_col + j)] = newpath;
                //     }
                // }

                // only when valid path
                if ((A[i][k] != USHRT_MAX) && (B[k][j] != USHRT_MAX)) {
                    newpath = A[i][k] + B[k][j];

                    if (newpath < C[i][j]) {
                        C[i][j] = newpath;
                    }
                }
            }
        }
    }

    // copy C back to main
    for (i = 0; i < BLOCK_SIZE; i++) {
        for (j = 0; j < BLOCK_SIZE; j++) {
            adj_mat[(ADJ_MAT_SIZE * BLOCK_SIZE * C_row + ADJ_MAT_SIZE * i) + (BLOCK_SIZE * C_col + j)] = C[i][j];
        }
    }
}

void FW_tile_top(unsigned short *adj_mat) {
    int i, j, k;
    float percent;

    for (k = 0; k < LOOP_SIZE; k++) {
        // Util tunction
        if (k % 5 == 0) {
            printf("k: %d \n", k);
        }

        // Phase 1
        FW_tile(k, k, k, k, k, k, adj_mat);

        // Phase 2
        for (i = 0; i < LOOP_SIZE; i++) {
            if (i != k) {
                FW_tile(i, k, i, k, k, k, adj_mat);
            }
        }
        for (j = 0; j < LOOP_SIZE; j++) {
            if (j != k) {
                FW_tile(k, j, k, k, k, j, adj_mat);
            }
        }

        // Phase 3
        for (int i = 0; i < LOOP_SIZE; i++) {
            for (int j = 0; j < LOOP_SIZE; j++) {
                if (i != k || j != k) {
                    FW_tile(i, j, i, k, k, j, adj_mat);
                }
            }
        }
    }
}

int main() {
    printf("Sanity: %d, %d, %d\n", ADJ_MAT_SIZE, BLOCK_SIZE, LOOP_SIZE);

    int i;
    clock_t t;
    double time_taken;

    // memory has to be in a contiguous portion of memory
    unsigned short *adj_mat = malloc(ADJ_MAT_SIZE * ADJ_MAT_SIZE * sizeof(unsigned short *));

    // reading from file
    FILE *file_pointer;
    file_pointer = fopen("adjmat_short(1000).bin", "r");
    // file_pointer = fopen("adjmat_short(10000).bin", "r");
    if (file_pointer == NULL) {
        printf("Error in opening the file\n");
        return -1;
    }
    int ret = fread(adj_mat, sizeof(unsigned short), ADJ_MAT_SIZE * ADJ_MAT_SIZE, file_pointer);
    fclose(file_pointer);

    ////////////////////////////////////////////////////floyd warshall
    printf("Starting f-w\n");
    unsigned short newpath;
    t = clock();

    FW_tile_top(adj_mat);

    // for (int k = 0; k < ADJ_MAT_SIZE; k++) {
    //     for (int i = 0; i < ADJ_MAT_SIZE; i++) {
    //         for (int j = 0; j < ADJ_MAT_SIZE; j++) {
    //             //only when valid path
    //             if ((adj_mat[ADJ_MAT_SIZE * i + k] != USHRT_MAX) && (adj_mat[ADJ_MAT_SIZE * k + j] != USHRT_MAX)){
    //                 newpath = adj_mat[ADJ_MAT_SIZE * i + k] + adj_mat[ADJ_MAT_SIZE * k + j];

    //                 if (newpath < adj_mat[ADJ_MAT_SIZE * i + j]) {
    //                     adj_mat[ADJ_MAT_SIZE * i + j] = newpath;
    //                 }
    //             }
    //         }
    //     }
    // }

    t = clock() - t;
    time_taken = ((double)t) / CLOCKS_PER_SEC;
    printf("Total time f-w: %f sec\n", time_taken);
    // ////////////////////////////////////////////////////floyd warshall

    ////////////////////////////////////////////////////row total
    printf("Starting Closeness Centrality\n");
    float *row_total = malloc(ADJ_MAT_SIZE * sizeof(float));
    float *row_con_count = malloc(ADJ_MAT_SIZE * sizeof(float));

    float row_total_buf;
    float row_con_count_buf;
    t = clock();
    for (int i = 0; i < ADJ_MAT_SIZE; i++) {
        row_total_buf = 0;
        row_con_count_buf = 0;
        for (int j = 0; j < ADJ_MAT_SIZE; j++) {
            if (adj_mat[ADJ_MAT_SIZE * i + j] != USHRT_MAX) {
                row_total_buf = row_total_buf + (float)adj_mat[ADJ_MAT_SIZE * i + j];
                row_con_count_buf = row_con_count_buf + 1;  // disjointed graphs
            }
        }
        row_total[i] = row_total_buf;
        row_con_count[i] = row_con_count_buf;
    }
    for (int i = 0; i < ADJ_MAT_SIZE; i++) {
        if (row_total[i] != 0) {  // divide by 0 error
            row_total[i] = (row_con_count[i] - 1) / row_total[i];
        }
    }
    t = clock() - t;
    time_taken = ((double)t) / CLOCKS_PER_SEC;
    printf("Total time CC: %f sec\n", time_taken);
    ////////////////////////////////////////////////////row total

    ////////////////////////////cc
    printf("Starting find max CC\n");
    int maxrow_idx = 0;
    float maxrow_val = row_total[0];
    t = clock();
    for (int i = 1; i < ADJ_MAT_SIZE; i++) {
        if (row_total[i] > maxrow_val) {
            maxrow_idx = i;
            maxrow_val = row_total[i];
        }
    }
    t = clock() - t;
    time_taken = ((double)t) / CLOCKS_PER_SEC;
    printf("Total time FCC: %f sec\n\n", time_taken);

    printf("C closest closest centrality: index: %d, val: %f\n", maxrow_idx, maxrow_val);

    free(adj_mat);
    return 0;
}