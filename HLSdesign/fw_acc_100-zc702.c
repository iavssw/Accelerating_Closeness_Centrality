// auto cal
#define ADJBLOCK 10000 * 100

void FW_core_100(unsigned short v0[100][100], unsigned short v1[100][100], unsigned short v2[100][100]) {  // L2
#pragma HLS INLINE

#pragma HLS array_partition variable = v0 cyclic factor = 5 dim = 1
#pragma HLS array_partition variable = v0 cyclic factor = 25 dim = 2

#pragma HLS array_partition variable = v1 cyclic factor = 5 dim = 1

#pragma HLS array_partition variable = v2 cyclic factor = 25 dim = 2

    unsigned short v3[1];                           // L5
    v3[0] = 0;                                      // L6
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
                        short v17 = v16 != 65535;                       // L22
                        unsigned short v18;
                        if (v17) {                                            // L23
                            unsigned short v19 = v2[v8][(v12 + (v10 * 25))];  // L24
                            short v20 = v19 != 65535;                         // L25
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

void FW_driver(short C_row, short C_col, short A_row, short A_col, short B_row, short B_col, unsigned short adj_mat[10000 * 10000]) {

    int i, j;

	unsigned short C[100][100];
	unsigned short A[100][100];
	unsigned short B[100][100];

    int C_row_block = ADJBLOCK * C_row;
    int C_col_block = 100 * C_col;

    int A_row_block = ADJBLOCK * A_row;
    int A_col_block = 100 * A_col;

    int B_row_block = ADJBLOCK * B_row;
    int B_col_block = 100 * B_col;

    // copy C into local buffer
    for (i = 0; i < 100; i++) {
        for (j = 0; j < 100; j++) {
#pragma HLS PIPELINE
            C[i][j] = adj_mat[(C_row_block + 10000 * i) + (C_col_block + j)];
            A[i][j] = adj_mat[(A_row_block + 10000 * i) + (A_col_block + j)];
            B[i][j] = adj_mat[(B_row_block + 10000 * i) + (B_col_block + j)];
        }
    }

	FW_core_100(C, A, B);

    // copy C back to main
    for (i = 0; i < 100; i++) {
        for (j = 0; j < 100; j++) {
#pragma HLS PIPELINE
            adj_mat[(C_row_block + 10000 * i) + (C_col_block + j)] = C[i][j];
        }
    }

}

void FW_tile_top(unsigned short adj_mat[10000 * 10000]) {
#pragma HLS INTERFACE m_axi depth = 100000000 port = adj_mat offset = slave bundle = adj_port0

    int i, j, k;

    for (k = 0; k < 100; k++) {
        // Phase 1
        FW_driver(k, k, k, k, k, k, adj_mat);

        // Phase 2
        for (i = 0; i < 100; i++) {
            if (i != k) {
            	FW_driver(i, k, i, k, k, k, adj_mat);
            }
        }
        for (j = 0; j < 100; j++) {
            if (j != k) {
            	FW_driver(k, j, k, k, k, j, adj_mat);
            }
        }

        // Phase 3
        for (i = 0; i < 100; i++) {
            for (j = 0; j < 100; j++) {
                if (i != k || j != k) {
                	FW_driver(i, j, i, k, k, j, adj_mat);
                }
            }
        }
    }
}


