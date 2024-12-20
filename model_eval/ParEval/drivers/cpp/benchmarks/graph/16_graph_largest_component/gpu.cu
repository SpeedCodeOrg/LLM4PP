// Driver for 16_graph_largest_component for CUDA and HIP
// /* Compute the number of vertices in the largest component of the graph defined by the adjacency matrix A.
//    Store the result in largestComponentSize.
//    A is an NxN adjacency matrix stored in row-major.
//    Use CUDA to compute in parallel. The kernel is launched on an NxN grid of threads.
//    Example:
// 
// 	 input: [[0, 1, 0, 0], [1, 0, 0, 0], [0, 0, 0, 1], [0, 0, 1, 0]]
//    output: 2
// */
// __global__ void largestComponent(const int *A, size_t N, int *largestComponentSize) {

#include <algorithm>
#include <numeric>
#include <random>
#include <vector>

#include "utilities.hpp"
#include "baseline.hpp"
#include "generated-code.cuh"   // code generated by LLM


struct Context {
    int *d_A, *d_largestComponentSize;
    std::vector<int> h_A;
    size_t N;
    dim3 blockSize, gridSize;
};

void fillRandomUndirectedGraph(std::vector<int> &A, size_t N) {
    std::fill(A.begin(), A.end(), 0);
    for (int i = 0; i < N; i += 1) {
        A[i * N + i] = 0;
        for (int j = i + 1; j < N; j += 1) {
            A[i * N + j] = rand() % 2;
            A[j * N + i] = A[i * N + j];
        }
    }
}

void reset(Context *ctx) {
    fillRandomUndirectedGraph(ctx->h_A, ctx->N);
    COPY_H2D(ctx->d_A, ctx->h_A.data(), ctx->N * ctx->N * sizeof(int));

    int tmp = 0;
    COPY_H2D(ctx->d_largestComponentSize, &tmp, sizeof(int));
}

Context *init() {
    Context *ctx = new Context();

    ctx->N = DRIVER_PROBLEM_SIZE;
    ctx->blockSize = dim3(32,32);
    ctx->gridSize = dim3((ctx->N + ctx->blockSize.x - 1) / ctx->blockSize.x,
                         (ctx->N + ctx->blockSize.y - 1) / ctx->blockSize.y); // at least enough threads

    ctx->h_A.resize(ctx->N * ctx->N);

    ALLOC(ctx->d_A, ctx->N * ctx->N * sizeof(int));
    ALLOC(ctx->d_largestComponentSize, sizeof(int));

    reset(ctx);
    return ctx;
}

void NO_OPTIMIZE compute(Context *ctx) {
    largestComponent<<<ctx->gridSize, ctx->blockSize>>>(ctx->d_A, ctx->N, ctx->d_largestComponentSize);
}

void NO_OPTIMIZE best(Context *ctx) {
    int correct = correctLargestComponent(ctx->h_A, ctx->N);
    (void) correct;
}

bool validate(Context *ctx) {
    const size_t TEST_SIZE = 128;
    dim3 blockSize = dim3(32,32);
    dim3 gridSize = dim3((TEST_SIZE + blockSize.x - 1) / blockSize.x,
                         (TEST_SIZE + blockSize.y - 1) / blockSize.y); // at least enough threads

    std::vector<int> h_A(TEST_SIZE * TEST_SIZE);
    int *d_A, *d_largestComponentSize;

    ALLOC(d_A, TEST_SIZE * TEST_SIZE * sizeof(int));
    ALLOC(d_largestComponentSize, sizeof(int));

    const size_t numTries = MAX_VALIDATION_ATTEMPTS;
    for (int trialIter = 0; trialIter < numTries; trialIter += 1) {
        // set up input
        fillRandomUndirectedGraph(h_A, TEST_SIZE);
        COPY_H2D(d_A, h_A.data(), TEST_SIZE * TEST_SIZE * sizeof(int));

        int tmpOutput = 0;
        COPY_H2D(d_largestComponentSize, &tmpOutput, sizeof(int));

        // compute correct result
        int correct = correctLargestComponent(h_A, TEST_SIZE);

        // compute test result
        largestComponent<<<gridSize, blockSize>>>(d_A, TEST_SIZE, d_largestComponentSize);
        SYNC();

        // copy back
        int test = 0;
        COPY_D2H(&test, d_largestComponentSize, sizeof(int));
        
        if (correct != test) {
            FREE(d_A);
            FREE(d_largestComponentSize);
            return false;
        }
    }

    FREE(d_A);
    FREE(d_largestComponentSize);
    return true;
}

void destroy(Context *ctx) {
    FREE(ctx->d_A);
    FREE(ctx->d_largestComponentSize);
    delete ctx;
}
