// Driver for 29_reduce_sum_of_min_of_pairs for CUDA and HIP
// /* Compute the sum of the minimum value at each index of vectors x and y for all indices.
//    i.e. sum = min(x_0, y_0) + min(x_1, y_1) + min(x_2, y_2) + ...
//    Store the result in sum.
//    Use CUDA to sum in parallel. The kernel is launched with at least as many threads as values in x.
//    Example:
//
//    input: x=[3, 4, 0, 2, 3], y=[2, 5, 3, 1, 7]
//    output: 10
// */
// __global__ void sumOfMinimumElements(const double *x, const double *y, size_t N, double *sum) {

#include <algorithm>
#include <numeric>
#include <random>
#include <vector>

#include "utilities.hpp"
#include "baseline.hpp"
#include "generated-code.cuh"   // code generated by LLM


struct Context {
    double *d_x, *d_y, *d_output;
    std::vector<double> h_x, h_y;
    size_t N;
    dim3 blockSize, gridSize;
};

void reset(Context *ctx) {
    fillRand(ctx->h_x, 0.0, 100.0);
    fillRand(ctx->h_y, 0.0, 100.0);

    COPY_H2D(ctx->d_x, ctx->h_x.data(), ctx->N * sizeof(double));
    COPY_H2D(ctx->d_y, ctx->h_y.data(), ctx->N * sizeof(double));

    double tmp = 0.0;
    COPY_H2D(ctx->d_output, &tmp, sizeof(double));
}

Context *init() {
    Context *ctx = new Context();

    ctx->N = DRIVER_PROBLEM_SIZE;
    ctx->blockSize = dim3(1024);
    ctx->gridSize = dim3((ctx->N + ctx->blockSize.x - 1) / ctx->blockSize.x); // at least enough threads

    ctx->h_x.resize(ctx->N);
    ctx->h_y.resize(ctx->N);
    ALLOC(ctx->d_x, ctx->N * sizeof(double));
    ALLOC(ctx->d_y, ctx->N * sizeof(double));
    ALLOC(ctx->d_output, sizeof(double));

    reset(ctx);
    return ctx;
}

void NO_OPTIMIZE compute(Context *ctx) {
    sumOfMinimumElements<<<ctx->gridSize, ctx->blockSize>>>(ctx->d_x, ctx->d_y, ctx->N, ctx->d_output);
}

void NO_OPTIMIZE best(Context *ctx) {
    double output = correctSumOfMinimumElements(ctx->h_x, ctx->h_y);
    (void)output;
}

bool validate(Context *ctx) {
    const size_t TEST_SIZE = 1024;
    dim3 blockSize = dim3(1024);
    dim3 gridSize = dim3((TEST_SIZE + blockSize.x - 1) / blockSize.x); // at least enough threads

    std::vector<double> h_x(TEST_SIZE), h_y(TEST_SIZE);
    double correct, test;

    double *d_x, *d_y, *d_test;
    ALLOC(d_x, TEST_SIZE * sizeof(double));
    ALLOC(d_test, sizeof(double));

    const size_t numTries = MAX_VALIDATION_ATTEMPTS;
    for (int trialIter = 0; trialIter < numTries; trialIter += 1) {
        // set up input
        fillRand(h_x, 0.0, 100.0);
        fillRand(h_y, 0.0, 100.0);

        COPY_H2D(d_x, h_x.data(), TEST_SIZE * sizeof(double));
        COPY_H2D(d_y, h_y.data(), TEST_SIZE * sizeof(double));

        double tmp = 0.0;
        COPY_H2D(d_test, &tmp, sizeof(double));

        // compute correct result
        correct = correctSumOfMinimumElements(h_x, h_y);

        // compute test result
        sumOfMinimumElements<<<gridSize, blockSize>>>(d_x, d_y, TEST_SIZE, d_test);
        SYNC();

        // copy back
        COPY_D2H(&test, d_test, sizeof(double));

        if (std::abs(correct - test) > 1e-4) {
            FREE(d_x);
            FREE(d_y);
            FREE(d_test);
            return false;
        }
    }

    FREE(d_x);
    FREE(d_y);
    FREE(d_test);
    return true;
}

void destroy(Context *ctx) {
    FREE(ctx->d_x);
    FREE(ctx->d_y);
    FREE(ctx->d_output);
    delete ctx;
}
