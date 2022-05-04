#define inf (999999999.0)
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include <vector_types.h>

__global__ void firstpass(int n, int k, float* x) //, int* qx) 
{
    __shared__ float dBlck[1024]; //, qBlck[1024];
    
    float tmp = 0.00;
    int i = (threadIdx.x >> 5), j = threadIdx.x & 31;
    int ind1 = ((k << 5) + i) * n + (k << 5) + j; //, k1 = k << 5;

    dBlck[threadIdx.x] = x[ind1];
    //qBlck[threadIdx.x] = qx[ind1];

    for (int l = 0; l < 32; ++l) 
    {
        __syncthreads();
        tmp = dBlck[(i << 5) + l] + dBlck[(l << 5) + j];
        if (dBlck[threadIdx.x] > tmp) 
        {
            dBlck[threadIdx.x] = tmp;
            //qBlck[threadIdx.x] = l + k1;
        }
    }
    x[ind1] = dBlck[threadIdx.x];
    //qx[ind1] = qBlck[threadIdx.x];
}

__global__ void secondpass(int n, int k, float* x) //, int* qx) 
{
    __shared__ float dBlck[1024], cBlock[1024]; //qcBlck[1024]

    int i = (threadIdx.x >> 5), j = threadIdx.x & 31, k1 = (k << 5), skip = 0;

    dBlck[threadIdx.x] = x[(k1 + i) * n + k1 + j];
    float tmp = 0.00;

    if (blockIdx.x >= k) // jump over block computed in first pass
    { 
        skip = 1;
    }
    if (blockIdx.y == 0) 
    {
        int ind1 = (k1 + i) * n + ((blockIdx.x + skip) << 5) + j;
        cBlock[threadIdx.x] = x[ind1];
        //qcBlck[threadIdx.x] = qx[ind1];

        for (int l = 0; l < 32; ++l) 
        {
            __syncthreads();
            tmp = dBlck[(i << 5) + l] + cBlock[(l << 5) + j];
            if (cBlock[threadIdx.x] > tmp) 
            {
                cBlock[threadIdx.x] = tmp;
                //qcBlck[threadIdx.x] = l + k1;
            }
        }
        x[ind1] = cBlock[threadIdx.x];
        //qx[ind1] = qcBlck[threadIdx.x];

    } 
    else 
    {
        int ind1 = (((blockIdx.x + skip)<<5) + i) * n + k1 + j;
        cBlock[threadIdx.x] = x[ind1];
        //qcBlck[threadIdx.x] = qx[ind1];

        for (int l = 0; l < 32; ++l) 
        {
            __syncthreads();
            tmp = cBlock[(i << 5) + l] + dBlck[(l << 5) + j];

            if (cBlock[threadIdx.x] > tmp) 
            {
                cBlock[threadIdx.x] = tmp;
                //qcBlck[threadIdx.x] = l + k1;
            }
        }
        x[ind1] = cBlock[threadIdx.x];
        //qx[ind1] = qcBlck[threadIdx.x];
    }
}

__global__ void thirdpass(int n, int k, float* x)
{
    int i = (threadIdx.x >> 5), j = threadIdx.x & 31, skipx = 0, skipy = 0;

    __shared__ float dyBlck[1024], dxBlck[1024];

    float cBlock_local = 0.00f;

    if (blockIdx.x >= k) 
    {
        skipx = 1;
    }
    if (blockIdx.y >= k) 
    {
        skipy = 1;
    }

    dxBlck[threadIdx.x] = x[((k << 5) + i) * n + ((blockIdx.y + skipy) << 5) + j];
    dyBlck[threadIdx.x] = x[(((blockIdx.x + skipx) << 5) + i) * n + (k << 5) + j];

    __syncthreads();

    int ind1 = (((blockIdx.x + skipx) << 5) + i) * n + ((blockIdx.y + skipy) << 5) + j;

    cBlock_local = x[ind1];

    float tmp = 0.00f;
    for (int l = 0; l < 32; ++l) 
    {
        tmp = dyBlck[(i << 5) + l] + dxBlck[(l << 5) + j];
        cBlock_local = fminf(cBlock_local, tmp);
    }
    x[ind1] = cBlock_local;
}

// closeness centrality kernel:
//  this is just a matrix-vector multiplication with unit vector
//  plus a reciprocal on the resulting vector
// @n: number of vertices
// @x: shortest distance matrix:
//      x[i * n + j] is the shortest distance from vertex i to vertex j
// @cc: output array of closeness centrality for each vertex
__global__ void closeness_centrality(int n, float* x, float* cc)
{
    // each thread works on one vertex
    const int tid = blockDim.x * blockIdx.x + threadIdx.x;  // assert(tid < n)

    float sum = 0.0f;
    if (tid < n) {
        for (int i = 0; i < n; i++) {
            if (tid != i) {
                float dis = x[tid * n + i];
                sum += 1 / dis; // use harmonic sum in case the graph is not strongly connected
            }
        }
        cc[tid] = sum;
    }
}

int main(int argc, char **argv) 
{

    cudaEvent_t start, stop, fw_finished;
    float *host_A, *host_D, *dev_x, *A, *D, tolerance = 0.001, sum = 0, dt_ms = 0;
    int n = atoi(argv[1]), i, j, bk11 = 1, bk21 = n/32 - 1, bk22 = 2, bk31 = n/32 - 1, bk32 = n/32 - 1, k = 0;
    int *Q;
    float *host_cc, *dev_cc, *cc;
    double t1s, t2s, t3s, t4s, t5s;
    char runcpu = argv[2][0];

    printf("==========================================\n");
    printf("Running with %d nodes \n", n);
    printf("\n");

    cudaMalloc(&dev_x, n * n * sizeof(float));
    cudaMalloc(&dev_cc, n * sizeof(float));
    //cudaMalloc(&dev_qx, n * n * sizeof(float));

    // Arrays for the CPU
    A = (float *) malloc(n * n * sizeof(float));    // weights (input graph)
    D = (float *) malloc(n * n * sizeof(float));    // shortest distances (results)
    Q = (int *) malloc(n * n * sizeof(int));        // does not affect computation result; used for debugging
    cc = (float*) malloc(n * sizeof(float));    // closeness centrality values

    // Arrays for the GPU
    host_A = (float *) malloc(n * n * sizeof(float));
    host_D = (float *) malloc(n * n * sizeof(float));
    host_cc = (float*) malloc(n * sizeof(float));

    // generate random graph
    srand(time(NULL));
    for (i = 0; i < n; ++i) 
    {
        for (j = 0; j < n; ++j) 
        {
            Q[i * n + j] = -1;
        }
        cc[i] = 0.0f;
    }
    for (i = 0; i < n; ++i) 
    {
        for (j = 0; j < n; ++j) 
        {
            if (i == j) 
            {
                A[i * n + j] = 0;
            } 
            else 
            {
                A[i * n + j] = 1200 * (float) rand() / RAND_MAX + 1;
                if (A[i * n + j] > 1000) 
                {
                    A[i * n + j] = inf;
                    Q[i * n + j] = -2;
                }
            }
            D[i * n + j] = A[i * n + j];
            host_A[i * n + j] = A[i * n + j];
        }
    }

    cudaEventCreate(&start);
    cudaEventCreate(&fw_finished);
    cudaEventCreate(&stop);

    // First copy, CPU -> GPU

    cudaEventRecord(start, 0);
    cudaMemcpy(dev_x, host_A, n * n * sizeof(float), cudaMemcpyHostToDevice);
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(start); 
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&dt_ms, start, stop);
    printf("Transfer CPU -> GPU, time: %lf ms\n", dt_ms);
    sum+=dt_ms;
    t1s = dt_ms;

    // GPU calculation
    cudaEventRecord(start, 0);
    dim3 bk2(n / 32 - 1, 2);
    dim3 bk3(n / 32 - 1, n / 32 - 1);
    int gputhreads = 1024;
    for (k = 0; k < n / 32; ++k) 
    {
        firstpass<<<1, gputhreads>>>(n, k, dev_x);
        secondpass<<<bk2, gputhreads>>>(n, k, dev_x);
        thirdpass<<<bk3, gputhreads>>>(n, k, dev_x);
    }
    cudaEventRecord(fw_finished, 0);

    closeness_centrality<<<n / 256 + 1, 256>>>(n, dev_x, dev_cc);

    cudaDeviceSynchronize(); // wait until all threads are done

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(start); 
    cudaEventSynchronize(fw_finished);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&dt_ms, start, stop);
    float fwtime, cctime;
    cudaEventElapsedTime(&fwtime, start, fw_finished);
    cudaEventElapsedTime(&cctime, fw_finished, stop);
    printf("Calculation time for GPU: %lf ms\n", dt_ms);
    printf("FW time: %lf ms; CC time: %lf ms\n", fwtime, cctime);
    sum+=dt_ms;
    t2s = dt_ms;

    // Second copy, GPU -> CPU
    
    cudaEventRecord(start, 0);
    cudaMemcpy(host_D, dev_x, n * n * sizeof (float), cudaMemcpyDeviceToHost);
    cudaMemcpy(host_cc, dev_cc, n * sizeof (float), cudaMemcpyDeviceToHost);
    cudaEventRecord(stop, 0);
    cudaEventSynchronize(start); 
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&dt_ms, start, stop);
    printf("Transfer GPU -> CPU, time: %lf ms\n", dt_ms);
    sum+=dt_ms;
    t3s = dt_ms;

    printf("Total time: %lf ms\n\n----------------------------\n", sum);
    t4s = sum;
    
   // Running sequentially on CPU now

    if(runcpu == 'y')
    {
        printf("\n");
        printf("Sequential execution on CPU (could take a while)... \n");
        cudaEventRecord(start, 0);
        for (k = 0; k < n; ++k) 
        {
            for (i = 0; i < n; ++i) 
            {
                for (j = 0; j < n; ++j) 
                {
                    if ((D[i * n + k] + D[k * n + j]) < D[i * n + j]) 
                    {
                        D[i * n + j] = D[i * n + k] + D[k * n + j];
                        Q[i * n + j] = k;
                    }
                }
            }
        }

        // compute closeness centrality
        for (i = 0; i < n; i++)
            for (j = 0; j < n; j++) {
                if (i != j) {
                    cc[i] += 1 / D[i * n + j];
                }
            }

        cudaEventRecord(stop, 0);
        cudaEventSynchronize(start); 
        cudaEventSynchronize(stop);
        cudaEventElapsedTime(&dt_ms, start, stop);
        printf("CPU time: %lf ms\n", dt_ms);
        t5s = dt_ms;
        printf("\n");

        // Result validation

        printf("FW: Comparing CPU results with GPU results...");
        for (i = 0; i < n; ++i) 
        {
            for (j = 0; j < n; ++j) 
            {
                if (abs(D[i * n + j] - host_D[i * n + j]) > tolerance) 
                {
                    printf("\nERROR: Different results in row i = %d and column j = %d, CPU result = %f, GPU result = %f \n", i, j, D[i * n + j], host_D[i * n + j]);
                }
            }
        }

        printf("CC: Comparing CPU results with GPU results...");
        for (i = 0; i < n; i++) {
            if (abs(cc[i] - host_cc[i]) > tolerance) {
                printf("\nERROR: Different results\n for vertex %d, CPU result = %f, GPU result = %f \n", i, cc[i], host_cc[i]);
            }
        }

        printf("Comparison complete! \n");
    }
    else
    {
        t5s = -1;
    }
    printf("Results are written to file resultsfwblocked.csv\n==========================================\n");
    FILE *fptr;
    fptr = fopen("resultsfwblocked.csv","a");

    fprintf(fptr,"%d,%d,%d,%d,%d,%d,%d,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n",n, bk11, bk21, bk22, bk31, bk32, gputhreads, t1s, t2s, t3s, t4s, t5s, fwtime, cctime);
    fclose(fptr);
    return 0;
}

