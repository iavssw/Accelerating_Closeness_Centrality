#define ADJ_MAT_SIZE 10000

#define BLOCK_SIZE 100

// auto cal
#define ADJBLOCK ADJ_MAT_SIZE * BLOCK_SIZE
#define LOOP_SIZE ADJ_MAT_SIZE / BLOCK_SIZE

#define USHRT_MAX 65535

void FW_core(unsigned short C[100][100], unsigned short A[100][100], unsigned short B[100][100]) {

#pragma HLS ARRAY_PARTITION variable=C block factor=10 dim=1
#pragma HLS ARRAY_PARTITION variable=A block factor=10 dim=1

    int i, j, k;
    unsigned short newpath = 0;

    for (k = 0; k < 100; k++) {
        for (i = 0; i < 100; i++) {
#pragma HLS UNROLL factor=10
            for (j = 0; j < 100; j++) {
#pragma HLS PIPELINE
                // only when valid path
                if ((A[i][k] != 65535) && (B[k][j] != 65535)){
                    newpath = A[i][k] + B[k][j];

                    if (newpath < C[i][j]) {
                        C[i][j] = newpath;
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
//#pragma HLS PIPELINE
        for (j = 0; j < BLOCK_SIZE; j++) {
#pragma HLS PIPELINE
            adj_mat[C_row_block + ADJ_MAT_SIZE * i][C_col_block + j] = C[i][j];
        }
    }
}

void get_data(short address[6], unsigned short C[BLOCK_SIZE][BLOCK_SIZE], unsigned short A[BLOCK_SIZE][BLOCK_SIZE], unsigned short B[BLOCK_SIZE][BLOCK_SIZE], unsigned short adj_mat[ADJ_MAT_SIZE][ADJ_MAT_SIZE]) {
#pragma HLS INLINE
    int i, j;

#pragma HLS ARRAY_PARTITION variable=address complete dim=1

    int C_row_block = ADJBLOCK * address[0];
    int C_col_block = BLOCK_SIZE * address[1];

    int A_row_block = ADJBLOCK * address[2];
    int A_col_block = BLOCK_SIZE * address[3];

    int B_row_block = ADJBLOCK * address[4];
    int B_col_block = BLOCK_SIZE * address[5];

	// copy C into local buffer
    for (i = 0; i < BLOCK_SIZE; i++) {
//#pragma HLS PIPELINE
    	for (j = 0; j < BLOCK_SIZE; j++) {
#pragma HLS PIPELINE
            C[i][j] = adj_mat[C_row_block + ADJ_MAT_SIZE * i][C_col_block + j];
            A[i][j] = adj_mat[A_row_block + ADJ_MAT_SIZE * i][A_col_block + j];
            B[i][j] = adj_mat[B_row_block + ADJ_MAT_SIZE * i][B_col_block + j];
        }
    }
}

void FW_task_parallel_top (short address[20][6], unsigned short adj_mat[ADJ_MAT_SIZE][ADJ_MAT_SIZE]) {
#pragma HLS DATAFLOW

#pragma HLS ARRAY_PARTITION variable=address complete dim=1

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

    unsigned short C10[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short A10[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short B10[BLOCK_SIZE][BLOCK_SIZE];

    unsigned short C11[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short A11[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short B11[BLOCK_SIZE][BLOCK_SIZE];

    unsigned short C12[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short A12[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short B12[BLOCK_SIZE][BLOCK_SIZE];

    unsigned short C13[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short A13[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short B13[BLOCK_SIZE][BLOCK_SIZE];

    unsigned short C14[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short A14[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short B14[BLOCK_SIZE][BLOCK_SIZE];

    unsigned short C15[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short A15[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short B15[BLOCK_SIZE][BLOCK_SIZE];

    unsigned short C16[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short A16[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short B16[BLOCK_SIZE][BLOCK_SIZE];

    unsigned short C17[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short A17[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short B17[BLOCK_SIZE][BLOCK_SIZE];

    unsigned short C18[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short A18[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short B18[BLOCK_SIZE][BLOCK_SIZE];

    unsigned short C19[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short A19[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short B19[BLOCK_SIZE][BLOCK_SIZE];

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
    get_data(address[10], C10, A10, B10, adj_mat);
    get_data(address[11], C11, A11, B11, adj_mat);
    get_data(address[12], C12, A12, B12, adj_mat);
    get_data(address[13], C13, A13, B13, adj_mat);
    get_data(address[14], C14, A14, B14, adj_mat);
    get_data(address[15], C15, A15, B15, adj_mat);
    get_data(address[16], C16, A16, B16, adj_mat);
    get_data(address[17], C17, A17, B17, adj_mat);
    get_data(address[18], C18, A18, B18, adj_mat);
    get_data(address[19], C19, A19, B19, adj_mat);

    // FW_core(C0, A0, B0);
    // FW_core(C1, A1, B1);
    // FW_core(C2, A2, B2);
    // FW_core(C3, A3, B3);
    // FW_core(C4, A4, B4);
    // FW_core(C5, A5, B5);
    // FW_core(C6, A6, B6);
    // FW_core(C7, A7, B7);
    // FW_core(C8, A8, B8);
    // FW_core(C9, A9, B9);
    // FW_core(C10, A10, B10);
    // FW_core(C11, A11, B11);
    // FW_core(C12, A12, B12);
    // FW_core(C13, A13, B13);
    // FW_core(C14, A14, B14);
    // FW_core(C15, A15, B15);
    // FW_core(C16, A16, B16);
    // FW_core(C17, A17, B17);
    // FW_core(C18, A18, B18);
    // FW_core(C19, A19, B19);

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
    return_data(address[10], C10, adj_mat);
    return_data(address[11], C11, adj_mat);
    return_data(address[12], C12, adj_mat);
    return_data(address[13], C13, adj_mat);
    return_data(address[14], C14, adj_mat);
    return_data(address[15], C15, adj_mat);
    return_data(address[16], C16, adj_mat);
    return_data(address[17], C17, adj_mat);
    return_data(address[18], C18, adj_mat);
    return_data(address[19], C19, adj_mat);
}

//void FW_tile(short C_row, short C_col, short A_row, short A_col, short B_row, short B_col, unsigned short adj_mat[ADJ_MAT_SIZE * ADJ_MAT_SIZE]) {
//    //#pragma HLS inline
//
//    int i, j, k;
//    unsigned short newpath;
//
//    unsigned short C[BLOCK_SIZE][BLOCK_SIZE];
//    unsigned short A[BLOCK_SIZE][BLOCK_SIZE];
//    unsigned short B[BLOCK_SIZE][BLOCK_SIZE];
//
//    // copy C into local buffer
//    for (i = 0; i < BLOCK_SIZE; i++) {
//        for (j = 0; j < BLOCK_SIZE; j++) {
//#pragma HLS PIPELINE
//            C[i][j] = adj_mat[(ADJ_MAT_SIZE * BLOCK_SIZE * C_row + ADJ_MAT_SIZE * i) + (BLOCK_SIZE * C_col + j)];
//        }
//    }
//
//    // copy A into local buffer
//    for (i = 0; i < BLOCK_SIZE; i++) {
//        for (j = 0; j < BLOCK_SIZE; j++) {
//#pragma HLS PIPELINE
//            A[i][j] = adj_mat[(ADJ_MAT_SIZE * BLOCK_SIZE * A_row + ADJ_MAT_SIZE * i) + (BLOCK_SIZE * A_col + j)];
//        }
//    }
//
//    // copy B into local buffer
//    for (i = 0; i < BLOCK_SIZE; i++) {
//        for (j = 0; j < BLOCK_SIZE; j++) {
//#pragma HLS PIPELINE
//            B[i][j] = adj_mat[(ADJ_MAT_SIZE * BLOCK_SIZE * B_row + ADJ_MAT_SIZE * i) + (BLOCK_SIZE * B_col + j)];
//        }
//    }
//
//    FW_core(C, A, B);
//
//    // copy C back to main
//    for (i = 0; i < BLOCK_SIZE; i++) {
//        for (j = 0; j < BLOCK_SIZE; j++) {
//            adj_mat[(ADJ_MAT_SIZE * BLOCK_SIZE * C_row + ADJ_MAT_SIZE * i) + (BLOCK_SIZE * C_col + j)] = C[i][j];
//        }
//    }
//}
//
//void FW_tile_top(unsigned short adj_mat[ADJ_MAT_SIZE * ADJ_MAT_SIZE]) {
//    int i, j, k;
//
//    for (k = 0; k < LOOP_SIZE; k++) {
//        // Phase 1
//        FW_tile(k, k, k, k, k, k, adj_mat);
//
//        // Phase 2
//        for (i = 0; i < LOOP_SIZE; i++) {
//            //#pragma HLS UNROLL factor=8
//            if (i != k) {
//                FW_tile(i, k, i, k, k, k, adj_mat);
//            }
//        }
//        for (j = 0; j < LOOP_SIZE; j++) {
//            //#pragma HLS UNROLL factor=8
//            if (j != k) {
//                FW_tile(k, j, k, k, k, j, adj_mat);
//            }
//        }
//
//        // Phase 3
//        for (int i = 0; i < LOOP_SIZE; i++) {
//            for (int j = 0; j < LOOP_SIZE; j++) {
//                //#pragma HLS UNROLL factor=8
//                if (i != k && j != k) {
//                    FW_tile(i, j, i, k, k, j, adj_mat);
//                }
//            }
//        }
//    }
//}
