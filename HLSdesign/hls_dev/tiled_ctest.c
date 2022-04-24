#define ADJ_MAT_SIZE 10000

#define BLOCK_SIZE 100

// auto cal
#define ADJBLOCK ADJ_MAT_SIZE* BLOCK_SIZE
#define LOOP_SIZE ADJ_MAT_SIZE / BLOCK_SIZE

#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void FW_core_100(unsigned short v0[100][100], unsigned short v1[100][100], unsigned short v2[100][100]) {  // L2

#pragma HLS array_partition variable = v0 cyclic factor = 5 dim = 1
#pragma HLS array_partition variable = v0 cyclic factor = 25 dim = 2

#pragma HLS array_partition variable = v1 cyclic factor = 5 dim = 1

#pragma HLS array_partition variable = v2 cyclic factor = 25 dim = 2

    unsigned short v3[1];                           // L5
    v3[0] = 0.000000;                               // L6
    unsigned short v4[1];                           // L15
    unsigned short v5[1];                           // L17
    unsigned short v6[1];                           // L9
    unsigned short v7[1];                           // L11
    for (int v8 = 0; v8 < 100; v8 += 1) {           // L7
        for (int v9 = 0; v9 < 20; v9 += 1) {        // L7
            for (int v10 = 0; v10 < 4; v10 += 1) {  // L7
#pragma HLS PIPELINE
                for (int v11 = 0; v11 < 5; v11 += 1) {                  // L7
                    for (int v12 = 0; v12 < 25; v12 += 1) {             // L7
                        unsigned short v13 = v3[0];                     // L8
                        v6[0] = v13;                                    // L10
                        v7[0] = v13;                                    // L12
                        unsigned short v14 = v6[0];                     // L14
                        v4[0] = v14;                                    // L16
                        v5[0] = v14;                                    // L18
                        unsigned short v15 = v4[0];                     // L20
                        unsigned short v16 = v1[(v11 + (v9 * 5))][v8];  // L21
                        short v17 = v16 != 65535.000000;                // L22
                        unsigned short v18;
                        if (v17) {                                            // L23
                            unsigned short v19 = v2[v8][(v12 + (v10 * 25))];  // L24
                            short v20 = v19 != 65535.000000;                  // L25
                            unsigned short v21;
                            if (v20) {                                                          // L26
                                unsigned short v22 = v1[(v11 + (v9 * 5))][v8];                  // L27
                                unsigned short v23 = v2[v8][(v12 + (v10 * 25))];                // L28
                                unsigned short v24 = v22 + v23;                                 // L29
                                unsigned short v25 = v0[(v11 + (v9 * 5))][(v12 + (v10 * 25))];  // L30
                                short v26 = v24 < v25;                                          // L31
                                if (v26) {                                                      // L32
                                    v0[(v11 + (v9 * 5))][(v12 + (v10 * 25))] = v24;             // L33
                                }
                                v21 = v24;  // L35
                            } else {
                                v21 = v15;  // L37
                            }
                            v18 = v21;  // L39
                        } else {
                            v18 = v15;  // L41
                        }
                        v4[0] = v18;                 // L43
                        v5[0] = v18;                 // L44
                        unsigned short v27 = v5[0];  // L46
                        v6[0] = v27;                 // L47
                        v7[0] = v27;                 // L48
                        unsigned short v28 = v7[0];  // L50
                        v3[0] = v28;                 // L51
                    }
                }
            }
        }
    }
}

void return_data(short address[6], unsigned short C[BLOCK_SIZE][BLOCK_SIZE], unsigned short adj_mat[ADJ_MAT_SIZE * ADJ_MAT_SIZE]) {
#pragma HLS INLINE

    // do not copy data if coords are invalid
    if (address[0] == -1) {
        return;
    }

    int i, j;

    int C_row_block = ADJBLOCK * address[0];
    int C_col_block = BLOCK_SIZE * address[1];

    // copy C back to main
    for (i = 0; i < BLOCK_SIZE; i++) {
        for (j = 0; j < BLOCK_SIZE; j++) {
#pragma HLS PIPELINE
            // adj_mat[C_row_block + ADJ_MAT_SIZE * i][C_col_block + j] = C[i][j];
            adj_mat[(C_row_block + ADJ_MAT_SIZE * i) + (C_col_block + j)] = C[i][j];
        }
    }
}

void get_data(short reset, short address[6], unsigned short C[BLOCK_SIZE][BLOCK_SIZE], unsigned short A[BLOCK_SIZE][BLOCK_SIZE], unsigned short B[BLOCK_SIZE][BLOCK_SIZE], unsigned short adj_mat[ADJ_MAT_SIZE * ADJ_MAT_SIZE]) {
#pragma HLS INLINE

    static int total_count;
    if (reset) {
        total_count = 0;
        printf("Reset: %d\n", total_count);
        return;
    }

    // do not copy data if coords are invalid
    if (address[0] == -1) {
        return;
    }

    total_count++;
    printf("Total Number of 100-blocks: %d\n", total_count);

    int i, j;

#pragma HLS ARRAY_PARTITION variable = address complete dim = 1

    int C_row_block = ADJBLOCK * address[0];
    int C_col_block = BLOCK_SIZE * address[1];

    int A_row_block = ADJBLOCK * address[2];
    int A_col_block = BLOCK_SIZE * address[3];

    int B_row_block = ADJBLOCK * address[4];
    int B_col_block = BLOCK_SIZE * address[5];

    // copy C into local buffer
    for (i = 0; i < BLOCK_SIZE; i++) {
        for (j = 0; j < BLOCK_SIZE; j++) {
#pragma HLS PIPELINE
            // C[i][j] = adj_mat[C_row_block + ADJ_MAT_SIZE * i][C_col_block + j];
            // A[i][j] = adj_mat[A_row_block + ADJ_MAT_SIZE * i][A_col_block + j];
            // B[i][j] = adj_mat[B_row_block + ADJ_MAT_SIZE * i][B_col_block + j];

            C[i][j] = adj_mat[(C_row_block + ADJ_MAT_SIZE * i) + (C_col_block + j)];
            A[i][j] = adj_mat[(A_row_block + ADJ_MAT_SIZE * i) + (A_col_block + j)];
            B[i][j] = adj_mat[(B_row_block + ADJ_MAT_SIZE * i) + (B_col_block + j)];
        }
    }
}

void FW_parallel_portcore(short address[4][6], unsigned short adj_mat[ADJ_MAT_SIZE * ADJ_MAT_SIZE]) {
#pragma HLS DATAFLOW

#pragma HLS ARRAY_PARTITION variable = address complete dim = 1

    unsigned short C0[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short A0[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short B0[BLOCK_SIZE][BLOCK_SIZE];

    unsigned short C1[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short A1[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short B1[BLOCK_SIZE][BLOCK_SIZE];

    unsigned short C2[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short A2[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short B2[BLOCK_SIZE][BLOCK_SIZE];

    unsigned short C3[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short A3[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short B3[BLOCK_SIZE][BLOCK_SIZE];

    get_data(0, address[0], C0, A0, B0, adj_mat);
    get_data(0, address[1], C1, A1, B1, adj_mat);
    get_data(0, address[2], C2, A2, B2, adj_mat);
    get_data(0, address[3], C3, A3, B3, adj_mat);

    FW_core_100(C0, A0, B0);
    FW_core_100(C1, A1, B1);
    FW_core_100(C2, A2, B2);
    FW_core_100(C3, A3, B3);

    return_data(address[0], C0, adj_mat);
    return_data(address[1], C1, adj_mat);
    return_data(address[2], C2, adj_mat);
    return_data(address[3], C3, adj_mat);
}

void FW_task_parallel_top(short address[20][6], unsigned short adj_mat0[ADJ_MAT_SIZE * ADJ_MAT_SIZE], unsigned short adj_mat1[ADJ_MAT_SIZE * ADJ_MAT_SIZE], unsigned short adj_mat2[ADJ_MAT_SIZE * ADJ_MAT_SIZE], unsigned short adj_mat3[ADJ_MAT_SIZE * ADJ_MAT_SIZE]) {
#pragma HLS DATAFLOW

    int i, j, k;

    short address0[4][6];
    short address1[4][6];
    short address2[4][6];
    short address3[4][6];

    for (k = 0; k < 4; k++) {
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 6; j++) {
                if (k == 0) {
                    address0[i][j] = address[4 * k + i][j];
                } else if (k == 1) {
                    address1[i][j] = address[4 * k + i][j];
                } else if (k == 2) {
                    address2[i][j] = address[4 * k + i][j];
                } else if (k == 3) {
                    address3[i][j] = address[4 * k + i][j];
                }
            }
        }
    }

    FW_parallel_portcore(address0, adj_mat0);
    FW_parallel_portcore(address1, adj_mat1);
    FW_parallel_portcore(address2, adj_mat2);
    FW_parallel_portcore(address3, adj_mat3);
}

void FW_driver(short reset, short force_run, short C_row, short C_col, short A_row, short A_col, short B_row, short B_col, unsigned short adj_mat0[ADJ_MAT_SIZE * ADJ_MAT_SIZE], unsigned short adj_mat1[ADJ_MAT_SIZE * ADJ_MAT_SIZE], unsigned short adj_mat2[ADJ_MAT_SIZE * ADJ_MAT_SIZE], unsigned short adj_mat3[ADJ_MAT_SIZE * ADJ_MAT_SIZE]) {
    int i;
    static short count;
    static short address[16][6];

    // keep count of jobs
    if (reset) {
        count = 0;
        return;
    }
    // compute if at end of phase
    if (force_run) {
        for (i = count; i < 16; i++) {
            address[i][0] = (short)-1;
            address[i][1] = (short)-1;
            address[i][2] = (short)-1;
            address[i][3] = (short)-1;
            address[i][4] = (short)-1;
            address[i][5] = (short)-1;
        }

        FW_task_parallel_top(address, adj_mat0, adj_mat1, adj_mat2, adj_mat3);
        count = 0;

        return;
    }

    address[count][0] = C_row;
    address[count][1] = C_col;
    address[count][2] = A_row;
    address[count][3] = A_col;
    address[count][4] = B_row;
    address[count][5] = B_col;

    count++;

    if (count == 16) {
        FW_task_parallel_top(address, adj_mat0, adj_mat1, adj_mat2, adj_mat3);
        count = 0;
    } else {
        // Error
        return;
    }
}

void FW_tile_top(unsigned short adj_mat0[ADJ_MAT_SIZE * ADJ_MAT_SIZE], unsigned short adj_mat1[ADJ_MAT_SIZE * ADJ_MAT_SIZE], unsigned short adj_mat2[ADJ_MAT_SIZE * ADJ_MAT_SIZE], unsigned short adj_mat3[ADJ_MAT_SIZE * ADJ_MAT_SIZE]) {
    int i, j, k;

    short dumA[6];
    unsigned short dum0[100][100];
    unsigned short dum1[100][100];
    unsigned short dum2[100][100];
    get_data(1, dumA, dum0, dum1, dum2, adj_mat0);

    for (k = 0; k < LOOP_SIZE; k++) {
        // Phase 1
        FW_driver(1, 0, -1, -1, -1, -1, -1, -1, adj_mat0, adj_mat1, adj_mat2, adj_mat3);  // init counter
        FW_driver(0, 0, k, k, k, k, k, k, adj_mat0, adj_mat1, adj_mat2, adj_mat3);
        FW_driver(0, 1, -1, -1, -1, -1, -1, -1, adj_mat0, adj_mat1, adj_mat2, adj_mat3);  // force compute

        // Phase 2
        for (i = 0; i < LOOP_SIZE; i++) {
            if (i != k) {
                // printf("raw _ cords: %d, %d\n", i, k);
                FW_driver(0, 0, i, k, i, k, k, k, adj_mat0, adj_mat1, adj_mat2, adj_mat3);
            }
        }
        for (j = 0; j < LOOP_SIZE; j++) {
            if (j != k) {
                FW_driver(0, 0, k, j, k, k, k, j, adj_mat0, adj_mat1, adj_mat2, adj_mat3);
            }
            if (j == LOOP_SIZE - 1) {  // force compute
                FW_driver(0, 1, -1, -1, -1, -1, -1, -1, adj_mat0, adj_mat1, adj_mat2, adj_mat3);
            }
        }

        // Phase 3
        for (int i = 0; i < LOOP_SIZE; i++) {
            for (int j = 0; j < LOOP_SIZE; j++) {
                if ((i != k) || (j != k)) {
                    FW_driver(0, 0, i, j, i, k, k, j, adj_mat0, adj_mat1, adj_mat2, adj_mat3);
                }
                if ((i == LOOP_SIZE - 1) && (j == LOOP_SIZE - 1)) {  // force compute
                    FW_driver(0, 1, -1, -1, -1, -1, -1, -1, adj_mat0, adj_mat1, adj_mat2, adj_mat3);
                }
            }
        }
    }
}

int main() {
    printf("SanityCheck: %d, %d, %d\n", ADJ_MAT_SIZE, BLOCK_SIZE, LOOP_SIZE);

    int i, j, k;
    clock_t t;
    double time_taken;

    // memory has to be in a contiguous portion of memory
    unsigned short* adj_mat = malloc(ADJ_MAT_SIZE * ADJ_MAT_SIZE * sizeof(unsigned short*));

    // reading from file
    FILE* file_pointer;
    file_pointer = fopen("adjmat_short(10000).bin", "r");
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

    FW_tile_top(adj_mat, adj_mat, adj_mat, adj_mat);

    t = clock() - t;
    time_taken = ((double)t) / CLOCKS_PER_SEC;
    printf("Total time f-w: %f sec\n", time_taken);
    // ////////////////////////////////////////////////////floyd warshall

    ////////////////////////////////////////////////////row total
    printf("Starting Closeness Centrality\n");
    float* row_total = malloc(ADJ_MAT_SIZE * sizeof(float));
    float* row_con_count = malloc(ADJ_MAT_SIZE * sizeof(float));

    float row_total_buf;
    float row_con_count_buf;
    t = clock();
    for (i = 0; i < ADJ_MAT_SIZE; i++) {
        row_total_buf = 0;
        row_con_count_buf = 0;
        for (j = 0; j < ADJ_MAT_SIZE; j++) {
            if (adj_mat[ADJ_MAT_SIZE * i + j] != USHRT_MAX) {
                row_total_buf = row_total_buf + (float)adj_mat[ADJ_MAT_SIZE * i + j];
                row_con_count_buf = row_con_count_buf + 1;  // disjointed graphs
            }
        }
        row_total[i] = row_total_buf;
        row_con_count[i] = row_con_count_buf;
    }
    for (i = 0; i < ADJ_MAT_SIZE; i++) {
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
    for (i = 1; i < ADJ_MAT_SIZE; i++) {
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
