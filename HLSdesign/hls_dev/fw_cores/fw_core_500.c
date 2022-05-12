#define ADJ_MAT_SIZE 10000

#define BLOCK_SIZE 500

// auto cal
#define ADJBLOCK ADJ_MAT_SIZE * BLOCK_SIZE
#define LOOP_SIZE ADJ_MAT_SIZE / BLOCK_SIZE

#define USHRT_MAX 65535


void FW_core_500(unsigned short v0[500][500], unsigned short v1[500][500], unsigned short v2[500][500]) {  // L2

#pragma HLS array_partition variable=v0 cyclic factor=25 dim=2

#pragma HLS array_partition variable=v1 cyclic factor=5 dim=2

#pragma HLS array_partition variable=v2 cyclic factor=5 dim=1
#pragma HLS array_partition variable=v2 cyclic factor=25 dim=2

    unsigned short v3[1];                                               // L5
    v3[0] = 0.000000;                                          // L6
    unsigned short v4[1];                                               // L15
    unsigned short v5[1];                                               // L17
    unsigned short v6[1];                                               // L9
    unsigned short v7[1];                                               // L11
    for (int v8 = 0; v8 < 100; v8 += 1) {                      // L7
        for (int v9 = 0; v9 < 500; v9 += 1) {                  // L7
            for (int v10 = 0; v10 < 20; v10 += 1) {            // L7
#pragma HLS PIPELINE            
                for (int v11 = 0; v11 < 5; v11 += 1) {         // L7
                    for (int v12 = 0; v12 < 25; v12 += 1) {    // L7
                        unsigned short v13 = v3[0];                     // L8
                        v6[0] = v13;                           // L10
                        v7[0] = v13;                           // L12
                        unsigned short v14 = v6[0];                     // L14
                        v4[0] = v14;                           // L16
                        v5[0] = v14;                           // L18
                        unsigned short v15 = v4[0];                     // L20
                        unsigned short v16 = v1[v9][(v11 + (v8 * 5))];  // L21
                        short v17 = v16 != 65535.000000;        // L22
                        unsigned short v18;
                        if (v17) {                                                 // L23
                            unsigned short v19 = v2[(v11 + (v8 * 5))][(v12 + (v10 * 25))];  // L24
                            short v20 = v19 != 65535.000000;                        // L25
                            unsigned short v21;
                            if (v20) {                                                 // L26
                                unsigned short v22 = v1[v9][(v11 + (v8 * 5))];                  // L27
                                unsigned short v23 = v2[(v11 + (v8 * 5))][(v12 + (v10 * 25))];  // L28
                                unsigned short v24 = v22 + v23;                                 // L29
                                unsigned short v25 = v0[v9][(v12 + (v10 * 25))];                // L30
                                short v26 = v24 < v25;                                  // L31
                                if (v26) {                                             // L32
                                    v0[v9][(v12 + (v10 * 25))] = v24;                  // L33
                                }
                                v21 = v24;  // L35
                            } else {
                                v21 = v15;  // L37
                            }
                            v18 = v21;  // L39
                        } else {
                            v18 = v15;  // L41
                        }
                        v4[0] = v18;        // L43
                        v5[0] = v18;        // L44
                        unsigned short v27 = v5[0];  // L46
                        v6[0] = v27;        // L47
                        v7[0] = v27;        // L48
                        unsigned short v28 = v7[0];  // L50
                        v3[0] = v28;        // L51
                    }
                }
            }
        }
    }
}

void top_500(short address[20][6], unsigned short adj_mat[ADJ_MAT_SIZE][ADJ_MAT_SIZE]) {

    unsigned short C0[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short A0[BLOCK_SIZE][BLOCK_SIZE];
    unsigned short B0[BLOCK_SIZE][BLOCK_SIZE];

    FW_core_500(C0, A0, B0);

}
