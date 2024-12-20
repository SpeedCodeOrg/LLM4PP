// Driver for 00_dense_la_lu_decomp for CUDA and HIP
// /* Factorize the matrix A into A=LU where L is a lower triangular matrix and U is an upper triangular matrix.
//    Store the results for L and U into the original matrix A. 
//    A is an NxN matrix stored in row-major.
//    Use CUDA to compute in parallel. The kernel is launched on an NxN grid of threads.
//    Example:
// 
//    input: [[4, 3], [6, 3]]
//    output: [[4, 3], [1.5, -1.5]]
// */
// __global__ void luFactorize(double *A, size_t N) {

#include <algorithm>
#include <numeric>
#include <random>
#include <vector>

#include "utilities.hpp"
#include "baseline.hpp"
#include "generated-code.cuh"   // code generated by LLM

struct Context {
    double *d_A;
    std::vector<double> h_A;
    size_t N;
    dim3 blockSize, gridSize;
};

void reset(Context *ctx) {
    fillRand(ctx->h_A, -10.0, 10.0);
    COPY_H2D(ctx->d_A, ctx->h_A.data(), ctx->N * ctx->N * sizeof(double));
}

Context *init() {
    Context *ctx = new Context();

    ctx->N = DRIVER_PROBLEM_SIZE;
    ctx->blockSize = dim3(32, 32);
    ctx->gridSize = dim3((ctx->N + ctx->blockSize.x - 1) / ctx->blockSize.x,
                         (ctx->N + ctx->blockSize.y - 1) / ctx->blockSize.y); // at least enough threads

    ctx->h_A.resize(ctx->N * ctx->N);
    ALLOC(ctx->d_A, ctx->N * ctx->N * sizeof(double));

    reset(ctx);
    return ctx;
}

void NO_OPTIMIZE compute(Context *ctx) {
    luFactorize<<<ctx->gridSize, ctx->blockSize>>>(ctx->d_A, ctx->N);
}

void NO_OPTIMIZE best(Context *ctx) {
    correctLuFactorize(ctx->h_A, ctx->N);
}

bool validate(Context *ctx) {
    const size_t TEST_SIZE = 512;
    dim3 blockSize = dim3(32, 32);
    dim3 gridSize = dim3((TEST_SIZE + blockSize.x - 1) / blockSize.x,
                         (TEST_SIZE + blockSize.y - 1) / blockSize.y); // at least enough threads

    std::vector<double> h_A(TEST_SIZE * TEST_SIZE), test(TEST_SIZE * TEST_SIZE);
    double *d_A;
    ALLOC(d_A, TEST_SIZE * TEST_SIZE * sizeof(double));

    const size_t numTries = MAX_VALIDATION_ATTEMPTS;
    for (int trialIter = 0; trialIter < numTries; trialIter += 1) {
        // set up input
        fillRand(h_A, -10.0, 10.0);
        COPY_H2D(d_A, h_A.data(), TEST_SIZE * TEST_SIZE * sizeof(double));

        // compute correct result
        correctLuFactorize(h_A, TEST_SIZE);

        // compute test result
        luFactorize<<<gridSize, blockSize>>>(d_A, TEST_SIZE);
        SYNC();

        // copy data back
        COPY_D2H(test.data(), d_A, TEST_SIZE * TEST_SIZE * sizeof(double));
        
        if (!fequal(h_A, test, 1e-3)) {
            FREE(d_A);
            return false;
        }
    }

    FREE(d_A);
    return true;
}

void destroy(Context *ctx) {
    FREE(ctx->d_A);
    delete ctx;
}
