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

int main() {
    printf("Sanity: %d, %d, %d\n", ADJ_MAT_SIZE, BLOCK_SIZE, LOOP_SIZE);

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
