// Driver for 28_reduce_smallest_odd_number for CUDA and HIP
// /* Return the value of the smallest odd number in the vector x.
//    Use CUDA to compute in parallel. The kernel is launched with at least as many threads as values in x.
//    Examples:
//
//    input: [7, 9, 5, 2, 8, 16, 4, 1]
//    output: 1
//
//    input: [8, 36, 7, 2, 11]
//    output: 7
// */
// __global__ void smallestOdd(const int *x, size_t N, int *smallest) {

#include <algorithm>
#include <numeric>
#include <random>
#include <vector>

#include "utilities.hpp"
#include "baseline.hpp"
#include "generated-code.cuh"   // code generated by LLM

struct Context {
    int *d_x, *d_output;
    std::vector<int> h_x;
    size_t N;
    dim3 blockSize, gridSize;
};

void reset(Context *ctx) {
    fillRand(ctx->h_x, 0.0, 100.0);
    COPY_H2D(ctx->d_x, ctx->h_x.data(), ctx->N * sizeof(int));

    int tmp = std::numeric_limits<int>::max();
    COPY_H2D(ctx->d_output, &tmp, sizeof(int));
}

Context *init() {
    Context *ctx = new Context();

    ctx->N = DRIVER_PROBLEM_SIZE;
    ctx->blockSize = dim3(1024);
    ctx->gridSize = dim3((ctx->N + ctx->blockSize.x - 1) / ctx->blockSize.x); // at least enough threads

    ctx->h_x.resize(ctx->N);
    ALLOC(ctx->d_x, ctx->N * sizeof(int));
    ALLOC(ctx->d_output, sizeof(int));

    reset(ctx);
    return ctx;
}

void NO_OPTIMIZE compute(Context *ctx) {
    smallestOdd<<<ctx->gridSize, ctx->blockSize>>>(ctx->d_x, ctx->N, ctx->d_output);
}

void NO_OPTIMIZE best(Context *ctx) {
    int output = correctSmallestOdd(ctx->h_x);
    (void)output;
}

bool validate(Context *ctx) {
    const size_t TEST_SIZE = 1024;
    dim3 blockSize = dim3(1024);
    dim3 gridSize = dim3((TEST_SIZE + blockSize.x - 1) / blockSize.x); // at least enough threads

    std::vector<int> h_x(TEST_SIZE);
    int correct, test;

    int *d_x, *d_test;
    ALLOC(d_x, TEST_SIZE * sizeof(int));
    ALLOC(d_test, sizeof(int));

    const size_t numTries = MAX_VALIDATION_ATTEMPTS;
    for (int trialIter = 0; trialIter < numTries; trialIter += 1) {
        // set up input
        fillRand(h_x, 0.0, 100.0);
        COPY_H2D(d_x, h_x.data(), TEST_SIZE * sizeof(int));

        int tmp = std::numeric_limits<int>::max();
        COPY_H2D(d_test, &tmp, sizeof(int));

        // compute correct result
        correct = correctSmallestOdd(h_x);

        // compute test result
        smallestOdd<<<gridSize, blockSize>>>(d_x, TEST_SIZE, d_test);
        SYNC();

        // copy back
        COPY_D2H(&test, d_test, sizeof(int));

        if (correct != test) {
            FREE(d_x);
            FREE(d_test);
            return false;
        }
    }

    FREE(d_x);
    FREE(d_test);
    return true;
}

void destroy(Context *ctx) {
    FREE(ctx->d_x);
    FREE(ctx->d_output);
    delete ctx;
}
