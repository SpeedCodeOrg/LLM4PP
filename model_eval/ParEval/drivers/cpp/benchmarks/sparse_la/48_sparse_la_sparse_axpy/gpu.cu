// Driver for 48_sparse_la_sparse_axpy for CUDA and HIP
// struct Element {
// 	size_t index;
//   double value;
// };
// 
// /* Compute z = alpha*x+y where x and y are sparse vectors of size Nx and Ny. Store the result in z.
//    Use CUDA to compute in parallel. The kernel is launched with at least as many threads as values in x or y.
//    Example:
//    
//    input: x=[{5, 12}, {8, 3}, {12, -1}], y=[{3, 1}, {5, -2}, {7, 1}, {8, -3}], alpha=1
//    output: z=[{3, 1}, {5, 10}, {7, 1}, {12, -1}]
// */
// __global__ void sparseAxpy(double alpha, const Element *x, const Element *y, double *z, size_t Nx, size_t Ny, size_t N) {

#include <algorithm>
#include <numeric>
#include <random>
#include <vector>

#include "utilities.hpp"
#include "generated-code.cuh"   // code generated by LLM
#include "baseline.hpp"


struct Context {
    Element *d_x, *d_y;
    double *d_z;
    std::vector<Element> h_x, h_y;
    std::vector<double> h_z;
    size_t N, Nx, Ny;
    dim3 blockSize, gridSize;
};

void reset(Context *ctx) {
    for (size_t i = 0; i < ctx->h_x.size(); i += 1) {
        ctx->h_x[i] = {rand() % ctx->N, (rand() / (double) RAND_MAX) * 2.0 - 1.0};
        ctx->h_y[i] = {rand() % ctx->N, (rand() / (double) RAND_MAX) * 2.0 - 1.0};
    }
    std::sort(ctx->h_x.begin(), ctx->h_x.end(), [](const Element &a, const Element &b) {
        return a.index < b.index;
    });
    std::sort(ctx->h_y.begin(), ctx->h_y.end(), [](const Element &a, const Element &b) {
        return a.index < b.index;
    });

    std::fill(ctx->h_z.begin(), ctx->h_z.end(), 0.0);

    COPY_H2D(ctx->d_x, ctx->h_x.data(), ctx->h_x.size() * sizeof(Element));
    COPY_H2D(ctx->d_y, ctx->h_y.data(), ctx->h_y.size() * sizeof(Element));
    COPY_H2D(ctx->d_z, ctx->h_z.data(), ctx->h_z.size() * sizeof(double));
}

Context *init() {
    Context *ctx = new Context();

    ctx->N = DRIVER_PROBLEM_SIZE;
    ctx->Nx = ctx->N * SPARSE_LA_SPARSITY;
    ctx->Ny = ctx->N * SPARSE_LA_SPARSITY;
    ctx->blockSize = dim3(1024);
    ctx->gridSize = dim3((ctx->Nx + ctx->blockSize.x - 1) / ctx->blockSize.x); // at least enough threads

    ctx->h_x.resize(ctx->Nx);
    ctx->h_y.resize(ctx->Ny);
    ctx->h_z.resize(ctx->N);

    ALLOC(ctx->d_x, ctx->Nx * sizeof(Element));
    ALLOC(ctx->d_y, ctx->Ny * sizeof(Element));
    ALLOC(ctx->d_z, ctx->N * sizeof(double));

    reset(ctx);
    return ctx;
}

void NO_OPTIMIZE compute(Context *ctx) {
    sparseAxpy<<<ctx->gridSize, ctx->blockSize>>>(1.0, ctx->d_x, ctx->d_y, ctx->d_z, ctx->Nx, ctx->Ny, ctx->N);
}

void NO_OPTIMIZE best(Context *ctx) {
    correctSparseAxpy(1.0, ctx->h_x, ctx->h_y, ctx->h_z);
}

bool validate(Context *ctx) {
    const size_t TEST_SIZE = 1024;
    const size_t nVals = TEST_SIZE * SPARSE_LA_SPARSITY;
    dim3 blockSize = dim3(1024);
    dim3 gridSize = dim3((nVals + blockSize.x - 1) / blockSize.x); // at least enough threads

    std::vector<Element> h_x(nVals), h_y(nVals);
    std::vector<double> correct(TEST_SIZE), test(TEST_SIZE);
    Element *d_x, *d_y;
    double *d_z;

    ALLOC(d_x, nVals * sizeof(Element));
    ALLOC(d_y, nVals * sizeof(Element));
    ALLOC(d_z, TEST_SIZE * sizeof(double));

    const size_t numTries = MAX_VALIDATION_ATTEMPTS;
    for (int trialIter = 0; trialIter < numTries; trialIter += 1) {
        // set up input
        for (size_t i = 0; i < nVals; i += 1) {
            h_x[i] = {rand() % ctx->N, (rand() / (double) RAND_MAX) * 2.0 - 1.0};
            h_y[i] = {rand() % ctx->N, (rand() / (double) RAND_MAX) * 2.0 - 1.0};
        }
        std::sort(h_x.begin(), h_x.end(), [](const Element &a, const Element &b) {
            return a.index < b.index;
        });
        std::sort(h_y.begin(), h_y.end(), [](const Element &a, const Element &b) {
            return a.index < b.index;
        });
        std::fill(correct.begin(), correct.end(), 0.0);
        
        COPY_H2D(d_x, h_x.data(), h_x.size() * sizeof(Element));
        COPY_H2D(d_y, h_y.data(), h_y.size() * sizeof(Element));
        COPY_H2D(d_z, correct.data(), correct.size() * sizeof(double));

        // compute correct result
        correctSparseAxpy(1.0, h_x, h_y, correct);

        // compute test result
        sparseAxpy<<<gridSize, blockSize>>>(1.0, d_x, d_y, d_z, nVals, nVals, TEST_SIZE);
        SYNC();
        
        // copy back
        COPY_D2H(test.data(), d_z, test.size() * sizeof(double));

        // compare
        if (!fequal(correct, test, 1e-4)) {
            FREE(d_x);
            FREE(d_y);
            FREE(d_z);
            return false;
        }
    }

    FREE(d_x);
    FREE(d_y);
    FREE(d_z);
    return true;
}

void destroy(Context *ctx) {
    FREE(ctx->d_x);
    FREE(ctx->d_y);
    FREE(ctx->d_z);
    delete ctx;
}