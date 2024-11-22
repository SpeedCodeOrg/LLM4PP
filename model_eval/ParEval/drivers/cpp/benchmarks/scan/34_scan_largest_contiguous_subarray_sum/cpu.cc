// Driver for 34_scan_largest_contiguous_subarray_sum for Serial, OpenMP, MPI, and MPI+OpenMP
// /* Return the largest sum of any contiguous subarray in the vector x.
//    i.e. if x=[−2, 1, −3, 4, −1, 2, 1, −5, 4] then [4, −1, 2, 1] is the contiguous
//    subarray with the largest sum of 6.
//    Example:
//
//    input: [−2, 1, −3, 4, −1, 2, 1, −5, 4]
//    output: 6
// */
// int maximumSubarray(std::vector<int> const& x) {

#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <vector>

#include "utilities.hpp"
#include "baseline.hpp"
#include "generated-code.hpp"   // code generated by LLM

struct Context {
    std::vector<int> x;
};

void reset(Context *ctx) {
    fillRand(ctx->x, -100, 100);
    BCAST(ctx->x, INT);
}

Context *init() {
    Context *ctx = new Context();

    ctx->x.resize(DRIVER_PROBLEM_SIZE);

    reset(ctx);
    return ctx;
}

void NO_OPTIMIZE compute(Context *ctx) {
    int val = maximumSubarray(ctx->x);
    (void) val;
}

void NO_OPTIMIZE best(Context *ctx) {
    int val = correctMaximumSubarray(ctx->x);
    (void) val;
}

bool validate(Context *ctx) {
    const size_t TEST_SIZE = 1024;

    std::vector<int> x(TEST_SIZE);

    int rank;
    GET_RANK(rank);

    const size_t numTries = MAX_VALIDATION_ATTEMPTS;
    for (int trialIter = 0; trialIter < numTries; trialIter += 1) {
        // set up input
        fillRand(x, -100, 100);
        BCAST(x, INT);

        // compute correct result
        int correct = correctMaximumSubarray(x);

        // compute test result
        int test = maximumSubarray(x);
        SYNC();

        bool isCorrect = true;
        if (IS_ROOT(rank) && test != correct) {
            isCorrect = false;
        }
        BCAST_PTR(&isCorrect, 1, CXX_BOOL);
        if (!isCorrect) {
            return false;
        }
    }

    return true;
}

void destroy(Context *ctx) {
    delete ctx;
}
