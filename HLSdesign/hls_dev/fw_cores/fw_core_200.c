#define ADJ_MAT_SIZE 10000

#define BLOCK_SIZE 200

// auto cal
#define ADJBLOCK ADJ_MAT_SIZE* BLOCK_SIZE
#define LOOP_SIZE ADJ_MAT_SIZE / BLOCK_SIZE

#define USHRT_MAX 65535

void FW_core(unsigned short v0[200][200], unsigned short v1[200][200], unsigned short v2[200][200]) {  // L2

#pragma HLS array_partition variable=v0 cyclic factor=2 dim=1
#pragma HLS array_partition variable=v0 cyclic factor=8 dim=2

#pragma HLS array_partition variable=v1 cyclic factor=2 dim=1
#pragma HLS array_partition variable=v1 cyclic factor=8 dim=2

#pragma HLS array_partition variable=v2 cyclic factor=8 dim=1
#pragma HLS array_partition variable=v2 cyclic factor=8 dim=2

    unsigned short v3[1];                                                                 // L5
    v3[0] = 0.000000;                                                            // L6
    unsigned short v4[1];                                                                 // L15
    unsigned short v5[1];                                                                 // L17
    unsigned short v6[1];                                                                 // L9
    unsigned short v7[1];                                                                 // L11
    for (int v8 = 0; v8 < 25; v8 += 1) {                                         // L7
        for (int v9 = 0; v9 < 100; v9 += 1) {                                    // L7
            for (int v10 = 0; v10 < 25; v10 += 1) {                              // L7
                for (int v11 = 0; v11 < 8; v11 += 1) {                           // L7
                    for (int v12 = 0; v12 < 2; v12 += 1) {                       // L7
                        for (int v13 = 0; v13 < 8; v13 += 1) {                   // L7
                            unsigned short v14 = v3[0];                                   // L8
                            v6[0] = v14;                                         // L10
                            v7[0] = v14;                                         // L12
                            unsigned short v15 = v6[0];                                   // L14
                            v4[0] = v15;                                         // L16
                            v5[0] = v15;                                         // L18
                            unsigned short v16 = v4[0];                                   // L20
                            unsigned short v17 = v1[(v12 + (v9 * 2))][(v11 + (v8 * 8))];  // L21
                            short v18 = v17 != 65535.000000;                      // L22
                            unsigned short v19;
                            if (v18) {                                                // L23
                                unsigned short v20 = v2[(v11 + (v8 * 8))][(v13 + (v10 * 8))];  // L24
                                short v21 = v20 != 65535.000000;                       // L25
                                unsigned short v22;
                                if (v21) {                                                // L26
                                    unsigned short v23 = v1[(v12 + (v9 * 2))][(v11 + (v8 * 8))];   // L27
                                    unsigned short v24 = v2[(v11 + (v8 * 8))][(v13 + (v10 * 8))];  // L28
                                    unsigned short v25 = v23 + v24;                                // L29
                                    unsigned short v26 = v0[(v12 + (v9 * 2))][(v13 + (v10 * 8))];  // L30
                                    short v27 = v25 < v26;                                 // L31
                                    if (v27) {                                            // L32
                                        v0[(v12 + (v9 * 2))][(v13 + (v10 * 8))] = v25;    // L33
                                    }
                                    v22 = v25;  // L35
                                } else {
                                    v22 = v16;  // L37
                                }
                                v19 = v22;  // L39
                            } else {
                                v19 = v16;  // L41
                            }
                            v4[0] = v19;        // L43
                            v5[0] = v19;        // L44
                            unsigned short v28 = v5[0];  // L46
                            v6[0] = v28;        // L47
                            v7[0] = v28;        // L48
                            unsigned short v29 = v7[0];  // L50
                            v3[0] = v29;        // L51
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
