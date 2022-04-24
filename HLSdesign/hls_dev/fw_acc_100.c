#define ADJ_MAT_SIZE 1000

#define BLOCK_SIZE 100

// auto cal
#define ADJBLOCK ADJ_MAT_SIZE* BLOCK_SIZE
#define LOOP_SIZE ADJ_MAT_SIZE / BLOCK_SIZE

#define USHRT_MAX 65535

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

void return_data(short address[6], unsigned short C[BLOCK_SIZE][BLOCK_SIZE], unsigned short adj_mat[ADJ_MAT_SIZE][ADJ_MAT_SIZE]) {
#pragma HLS INLINE

    int i, j;

    int C_row_block = ADJBLOCK * address[0];
    int C_col_block = BLOCK_SIZE * address[1];

    // copy C back to main
    for (i = 0; i < BLOCK_SIZE; i++) {
        for (j = 0; j < BLOCK_SIZE; j++) {
#pragma HLS PIPELINE
            adj_mat[C_row_block + ADJ_MAT_SIZE * i][C_col_block + j] = C[i][j];
        }
    }
}

void get_data(short address[6], unsigned short C[BLOCK_SIZE][BLOCK_SIZE], unsigned short A[BLOCK_SIZE][BLOCK_SIZE], unsigned short B[BLOCK_SIZE][BLOCK_SIZE], unsigned short adj_mat[ADJ_MAT_SIZE][ADJ_MAT_SIZE]) {
#pragma HLS INLINE
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
            C[i][j] = adj_mat[C_row_block + ADJ_MAT_SIZE * i][C_col_block + j];
            A[i][j] = adj_mat[A_row_block + ADJ_MAT_SIZE * i][A_col_block + j];
            B[i][j] = adj_mat[B_row_block + ADJ_MAT_SIZE * i][B_col_block + j];
        }
    }
}

void FW_task_parallel_top(short address[10][6], unsigned short adj_mat[ADJ_MAT_SIZE][ADJ_MAT_SIZE]) {
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

    unsigned short C4[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short A4[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short B4[BLOCK_SIZE][BLOCK_SIZE];

    unsigned short C5[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short A5[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short B5[BLOCK_SIZE][BLOCK_SIZE];

    unsigned short C6[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short A6[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short B6[BLOCK_SIZE][BLOCK_SIZE];

    unsigned short C7[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short A7[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short B7[BLOCK_SIZE][BLOCK_SIZE];

    unsigned short C8[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short A8[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short B8[BLOCK_SIZE][BLOCK_SIZE];

    unsigned short C9[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short A9[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short B9[BLOCK_SIZE][BLOCK_SIZE];

    get_data(address[0], C0, A0, B0, adj_mat);
    get_data(address[1], C1, A1, B1, adj_mat);
    get_data(address[2], C2, A2, B2, adj_mat);
    get_data(address[3], C3, A3, B3, adj_mat);
    get_data(address[4], C4, A4, B4, adj_mat);
    get_data(address[5], C5, A5, B5, adj_mat);
    get_data(address[6], C6, A6, B6, adj_mat);
    get_data(address[7], C7, A7, B7, adj_mat);
    get_data(address[8], C8, A8, B8, adj_mat);
    get_data(address[9], C9, A9, B9, adj_mat);

    FW_core_100(C0, A0, B0);
    FW_core_100(C1, A1, B1);
    FW_core_100(C2, A2, B2);
    FW_core_100(C3, A3, B3);
    FW_core_100(C4, A4, B4);
    FW_core_100(C5, A5, B5);
    FW_core_100(C6, A6, B6);
    FW_core_100(C7, A7, B7);
    FW_core_100(C8, A8, B8);
    FW_core_100(C9, A9, B9);

    return_data(address[0], C0, adj_mat);
    return_data(address[1], C1, adj_mat);
    return_data(address[2], C2, adj_mat);
    return_data(address[3], C3, adj_mat);
    return_data(address[4], C4, adj_mat);
    return_data(address[5], C5, adj_mat);
    return_data(address[6], C6, adj_mat);
    return_data(address[7], C7, adj_mat);
    return_data(address[8], C8, adj_mat);
    return_data(address[9], C9, adj_mat);
}