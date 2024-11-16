// Driver for 47_sparse_la_spmv for CUDA and HIP
// struct COOElement {
//    size_t row, column;
//    double value;
// };
// 
// /* Compute y = alpha*A*x + beta*y where alpha and beta are scalars, x and y are vectors,
//    and A is a sparse matrix stored in COO format with sizeA elements.
//    x and y are length N and A is M x N.
//    Use CUDA to parallelize. The kernel will be launched with at least sizeA threads.
//    Example:
// 
//    input: alpha=0.5 beta=1.0 A=[{0,1,3}, {1,0,-1}] x=[-4, 2] y=[-1,1]
//    output: y=[2, 3]
// */
// __global__ void spmv(double alpha, const COOElement *A, size_t sizeA, const double *x, double beta, double *y, size_t M, size_t N) {

#include <algorithm>
#include <numeric>
#include <random>
#include <vector>

#include "utilities.hpp"
#include "generated-code.cuh"   // code generated by LLM
#include "baseline.hpp"

struct Context {
    COOElement *d_A;
    double *d_x, *d_y;
    std::vector<COOElement> h_A;
    std::vector<double> h_x, h_y;
    double alpha, beta;
    size_t M, N;
    dim3 blockSize, gridSize;
};

void reset(Context *ctx) {
    ctx->alpha = (rand() / (double) RAND_MAX) * 2.0 - 1.0;
    ctx->beta = (rand() / (double) RAND_MAX) * 2.0 - 1.0;
    fillRand(ctx->h_x, -1.0, 1.0);
    fillRand(ctx->h_y, -1.0, 1.0);
    for (size_t i = 0; i < ctx->h_A.size(); i += 1) {
        ctx->h_A[i] = {rand() % ctx->M, rand() % ctx->N, (rand() / (double) RAND_MAX) * 2.0 - 1.0};
    }
    std::sort(ctx->h_A.begin(), ctx->h_A.end(), [](const COOElement &a, const COOElement &b) {
        return (a.row == b.row) ? (a.column < b.column) : (a.row < b.row);
    });

    COPY_H2D(ctx->d_A, ctx->h_A.data(), ctx->h_A.size() * sizeof(COOElement));
    COPY_H2D(ctx->d_x, ctx->h_x.data(), ctx->h_x.size() * sizeof(double));
    COPY_H2D(ctx->d_y, ctx->h_y.data(), ctx->h_y.size() * sizeof(double));
}

Context *init() {
    Context *ctx = new Context();

    ctx->N = DRIVER_PROBLEM_SIZE;
    ctx->M = DRIVER_PROBLEM_SIZE;
    const size_t nVals = ctx->N * ctx->M * SPARSE_LA_SPARSITY;
    ctx->blockSize = dim3(1024);
    ctx->gridSize = dim3((nVals + ctx->blockSize.x - 1) / ctx->blockSize.x); // at least enough threads

    ctx->h_A.resize(nVals);
    ctx->h_x.resize(ctx->N);
    ctx->h_y.resize(ctx->M);

    ALLOC(ctx->d_A, nVals * sizeof(COOElement));
    ALLOC(ctx->d_x, ctx->N * sizeof(double));
    ALLOC(ctx->d_y, ctx->M * sizeof(double));

    reset(ctx);
    return ctx;
}

void NO_OPTIMIZE compute(Context *ctx) {
    spmv<<<ctx->gridSize, ctx->blockSize>>>(ctx->alpha, ctx->d_A, ctx->h_A.size(), ctx->d_x, ctx->beta, ctx->d_y, ctx->M, ctx->N);
}

void NO_OPTIMIZE best(Context *ctx) {
    correctSpmv(ctx->alpha, ctx->h_A, ctx->h_x, ctx->beta, ctx->h_y, ctx->M, ctx->N);
}

bool validate(Context *ctx) {
    const size_t TEST_SIZE = 128;
    const size_t nVals = TEST_SIZE * TEST_SIZE * SPARSE_LA_SPARSITY;
    dim3 blockSize = dim3(1024);
    dim3 gridSize = dim3((nVals + blockSize.x - 1) / blockSize.x); // at least enough threads

    std::vector<COOElement> h_A(nVals);
    std::vector<double> h_x(TEST_SIZE);
    std::vector<double> correct(TEST_SIZE), test(TEST_SIZE);

    COOElement *d_A;
    double *d_x, *d_y;

    ALLOC(d_A, nVals * sizeof(COOElement));
    ALLOC(d_x, TEST_SIZE * sizeof(double));
    ALLOC(d_y, TEST_SIZE * sizeof(double));

    const size_t numTries = MAX_VALIDATION_ATTEMPTS;
    for (int trialIter = 0; trialIter < numTries; trialIter += 1) {
        // set up input
        fillRand(h_x, -1.0, 1.0);
        fillRand(correct, -1.0, 1.0);
        for (size_t i = 0; i < h_A.size(); i += 1) {
            h_A[i] = {rand() % TEST_SIZE, rand() % TEST_SIZE, (rand() / (double) RAND_MAX) * 2.0 - 1.0};
        }
        std::sort(h_A.begin(), h_A.end(), [](const COOElement &a, const COOElement &b) {
            return (a.row == b.row) ? (a.column < b.column) : (a.row < b.row);
        });

        COPY_H2D(d_A, h_A.data(), h_A.size() * sizeof(COOElement));
        COPY_H2D(d_x, h_x.data(), h_x.size() * sizeof(double));
        COPY_H2D(d_y, correct.data(), correct.size() * sizeof(double));

        // compute correct result
        correctSpmv(ctx->alpha, h_A, h_x, ctx->beta, correct, TEST_SIZE, TEST_SIZE);

        // compute test result
        spmv<<<gridSize, blockSize>>>(ctx->alpha, d_A, h_A.size(), d_x, ctx->beta, d_y, TEST_SIZE, TEST_SIZE);
        SYNC();

        // copy back
        COPY_D2H(test.data(), d_y, test.size() * sizeof(double));
        
        if (!fequal(correct, test, 1e-4)) {
            return false;
        }
    }

    return true;
}

void destroy(Context *ctx) {
    delete ctx;
}