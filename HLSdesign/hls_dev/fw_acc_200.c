#define ADJ_MAT_SIZE 1000

#define BLOCK_SIZE 200

// auto cal
#define ADJBLOCK ADJ_MAT_SIZE* BLOCK_SIZE
#define LOOP_SIZE ADJ_MAT_SIZE / BLOCK_SIZE

#define USHRT_MAX 65535

void FW_core(unsigned short v0[200][200], unsigned short v1[200][200], unsigned short v2[200][200]) {  // L2

#pragma HLS array_partition variable = v0 cyclic factor = 2 dim = 1
#pragma HLS array_partition variable = v0 cyclic factor = 8 dim = 2

#pragma HLS array_partition variable = v1 cyclic factor = 2 dim = 1
#pragma HLS array_partition variable = v1 cyclic factor = 8 dim = 2

#pragma HLS array_partition variable = v2 cyclic factor = 8 dim = 1
#pragma HLS array_partition variable = v2 cyclic factor = 8 dim = 2

    unsigned short v3[1];                                                                 // L5
    v3[0] = 0.000000;                                                                     // L6
    unsigned short v4[1];                                                                 // L15
    unsigned short v5[1];                                                                 // L17
    unsigned short v6[1];                                                                 // L9
    unsigned short v7[1];                                                                 // L11
    for (int v8 = 0; v8 < 25; v8 += 1) {                                                  // L7
        for (int v9 = 0; v9 < 100; v9 += 1) {                                             // L7
            for (int v10 = 0; v10 < 25; v10 += 1) {                                       // L7
#pragma HLS pipeline II=129
                for (int v11 = 0; v11 < 8; v11 += 1) {                                    // L7
                    for (int v12 = 0; v12 < 2; v12 += 1) {                                // L7
                        for (int v13 = 0; v13 < 8; v13 += 1) {                            // L7
                            unsigned short v14 = v3[0];                                   // L8
                            v6[0] = v14;                                                  // L10
                            v7[0] = v14;                                                  // L12
                            unsigned short v15 = v6[0];                                   // L14
                            v4[0] = v15;                                                  // L16
                            v5[0] = v15;                                                  // L18
                            unsigned short v16 = v4[0];                                   // L20
                            unsigned short v17 = v1[(v12 + (v9 * 2))][(v11 + (v8 * 8))];  // L21
                            short v18 = v17 != 65535.000000;                              // L22
                            unsigned short v19;
                            if (v18) {                                                         // L23
                                unsigned short v20 = v2[(v11 + (v8 * 8))][(v13 + (v10 * 8))];  // L24
                                short v21 = v20 != 65535.000000;                               // L25
                                unsigned short v22;
                                if (v21) {                                                         // L26
                                    unsigned short v23 = v1[(v12 + (v9 * 2))][(v11 + (v8 * 8))];   // L27
                                    unsigned short v24 = v2[(v11 + (v8 * 8))][(v13 + (v10 * 8))];  // L28
                                    unsigned short v25 = v23 + v24;                                // L29
                                    unsigned short v26 = v0[(v12 + (v9 * 2))][(v13 + (v10 * 8))];  // L30
                                    short v27 = v25 < v26;                                         // L31
                                    if (v27) {                                                     // L32
                                        v0[(v12 + (v9 * 2))][(v13 + (v10 * 8))] = v25;             // L33
                                    }
                                    v22 = v25;  // L35
                                } else {
                                    v22 = v16;  // L37
                                }
                                v19 = v22;  // L39
                            } else {
                                v19 = v16;  // L41
                            }
                            v4[0] = v19;                 // L43
                            v5[0] = v19;                 // L44
                            unsigned short v28 = v5[0];  // L46
                            v6[0] = v28;                 // L47
                            v7[0] = v28;                 // L48
                            unsigned short v29 = v7[0];  // L50
                            v3[0] = v29;                 // L51
                        }
                    }
                }
            }
        }
    }
}

void top(short address[20][6], unsigned short adj_mat[ADJ_MAT_SIZE][ADJ_MAT_SIZE]) {
    unsigned short C0[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short A0[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short B0[BLOCK_SIZE][BLOCK_SIZE];

    FW_core(C0, A0, B0);
}

void return_data(short address[6], unsigned short C[BLOCK_SIZE][BLOCK_SIZE], unsigned short adj_mat[ADJ_MAT_SIZE][ADJ_MAT_SIZE]) {
#pragma HLS INLINE

    int i, j;

    // copy C back to main
    for (i = 0; i < BLOCK_SIZE; i++) {
        for (j = 0; j < BLOCK_SIZE; j++) {
#pragma HLS PIPELINE
            adj_mat[address[0] + ADJ_MAT_SIZE * i][address[1] + j] = C[i][j];
        }
    }
}

void get_data(short address[6], unsigned short C[BLOCK_SIZE][BLOCK_SIZE], unsigned short A[BLOCK_SIZE][BLOCK_SIZE], unsigned short B[BLOCK_SIZE][BLOCK_SIZE], unsigned short adj_mat[ADJ_MAT_SIZE][ADJ_MAT_SIZE]) {
#pragma HLS INLINE
    int i, j;

    // copy C into local buffer
    for (i = 0; i < BLOCK_SIZE; i++) {
        for (j = 0; j < BLOCK_SIZE; j++) {
#pragma HLS PIPELINE
            C[i][j] = adj_mat[address[0] + ADJ_MAT_SIZE * i][address[1] + j];
            A[i][j] = adj_mat[address[2] + ADJ_MAT_SIZE * i][address[3] + j];
            B[i][j] = adj_mat[address[4] + ADJ_MAT_SIZE * i][address[5] + j];
        }
    }
}

void FW_task_parallel_top(short address[10][6], unsigned short adj_mat[ADJ_MAT_SIZE][ADJ_MAT_SIZE]) {
#pragma HLS DATAFLOW

#pragma HLS ARRAY_PARTITION variable = address complete dim = 1
#pragma HLS ARRAY_PARTITION variable = address complete dim = 2

    int i, j;

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

    for (i = 0; i < 10; i++) {
#pragma HLS UNROLL factor = 10
        address[i][0] = ADJBLOCK * address[i][0];
        address[i][1] = BLOCK_SIZE * address[i][1];

        address[i][2] = ADJBLOCK * address[i][2];
        address[i][3] = BLOCK_SIZE * address[i][3];

        address[i][4] = ADJBLOCK * address[i][4];
        address[i][5] = BLOCK_SIZE * address[i][5];
    }

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

    FW_core(C0, A0, B0);
    FW_core(C1, A1, B1);
    FW_core(C2, A2, B2);
    FW_core(C3, A3, B3);
    FW_core(C4, A4, B4);
    FW_core(C5, A5, B5);
    FW_core(C6, A6, B6);
    FW_core(C7, A7, B7);
    FW_core(C8, A8, B8);
    FW_core(C9, A9, B9);

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
