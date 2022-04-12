#define SIZE 1000
// dont use 0 row and col

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

int main(int argc, char**argv) {

    if (argc < 2) {
        printf("Error: Need the size of the adjacency matrix \n");
        return -1;
    }
    int array_size = atoi(argv[1]);
    // printf("argv: %d\n", array_size);

    int i;
    clock_t t;
    double time_taken;

    // static unsigned short adj_mat[SIZE][SIZE];

    // memory has to be in a contiguous portion of memory
    unsigned short* adj_mat = malloc(array_size * array_size * sizeof(unsigned short*));

    // reading from file
    FILE* file_pointer;
    // file_pointer = fopen("adjmat_short.bin", "r");
    file_pointer = fopen("adjmat_short.bin", "r");
    if (file_pointer == NULL) {
        printf("Error in opening the file\n");
        return -1;
    }
    int ret = fread(adj_mat, sizeof(unsigned short), array_size * array_size, file_pointer);
    fclose(file_pointer);
    // printf("finished reading\n");

    // printf("\n");
    // for (int i = 0; i < SIZE; i++) {
    //     for (int j = 0; j < SIZE; j++) {
    //         printf("%d ", adj_mat[i][j]);
    //     }
    //     printf("\n");
    // }

////////////////////////////////////////////////////floyd warshall
    printf("Starting f-w\n");
    unsigned short newpath;
    t = clock();
    for (int k = 0; k < array_size; k++) {
        for (int i = 0; i < array_size; i++) {
            for (int j = 0; j < array_size; j++) {
                //only when valid path
                if ((adj_mat[array_size * i + k] != USHRT_MAX) && (adj_mat[array_size * k + j] != USHRT_MAX)){
                    newpath = adj_mat[array_size * i + k] + adj_mat[array_size * k + j];

                    if (newpath < adj_mat[array_size * i + j]) {
                        adj_mat[array_size * i + j] = newpath;
                    }
                }
            }
        }
    }
    t = clock() - t;
    time_taken = ((double)t) / CLOCKS_PER_SEC;
    printf("Total time f-w: %f sec\n", time_taken);
// ////////////////////////////////////////////////////floyd warshall 

//     file_pointer = fopen("raw_fw.out", "w");
//     if (file_pointer == NULL) {
//         printf("Error in opening the file\n");
//         return -1;
//     }
//     ret = fwrite(adj_mat, sizeof(unsigned short), array_size * array_size, file_pointer);
//     fclose(file_pointer);
//     printf("finished fw out\n");

////////////////////////////////////////////////////row total
    printf("Starting Closeness Centrality\n");
    float *row_total = malloc(array_size * sizeof(float));
    float *row_con_count = malloc(array_size * sizeof(float));

    float row_total_buf;
    float row_con_count_buf;
    t = clock();
    for (int i = 0; i < array_size; i++) {
        row_total_buf = 0;
        row_con_count_buf = 0;
        for (int j = 0; j < array_size; j++) {
            if (adj_mat[array_size * i + j] != USHRT_MAX) {
                row_total_buf = row_total_buf + (float)adj_mat[array_size * i + j];
                row_con_count_buf = row_con_count_buf + 1; //disjointed graphs
            }
        }
        row_total[i] = row_total_buf;
        row_con_count[i] = row_con_count_buf;
    }
    for (int i = 0; i < array_size; i++) {
        if (row_total[i] != 0) { //divide by 0 error
            row_total[i] = (row_con_count[i] - 1) / row_total[i];
        }
    }
    t = clock() - t;
    time_taken = ((double)t) / CLOCKS_PER_SEC;
    printf("Total time CC: %f sec\n", time_taken);
////////////////////////////////////////////////////row total

    // file_pointer = fopen("cc.out", "w");
    // if (file_pointer == NULL) {
    //     printf("Error in opening the file\n");
    //     return -1;
    // }
    // ret = fwrite(row_total, sizeof(float), array_size, file_pointer);
    // fclose(file_pointer);
    // printf("finished cc out\n");

////////////////////////////cc
    printf("Starting find max CC\n");
    int maxrow_idx = 0;
    float maxrow_val = row_total[0];
    t = clock();
    for (int i = 1; i < array_size; i++) {
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