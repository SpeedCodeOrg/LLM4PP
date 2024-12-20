// Driver for 54_transform_inverse_offset
// /* Replace every element of the vector x with 1-1/x.
//    Example:
// 
//    input: [2, 4, 1, 12, -2]
//    output: [0.5, 0.75, 0, 0.91666666, 1.5]
// */
// void oneMinusInverse(std::vector<double> &x) {

#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <vector>

#include "utilities.hpp"
#include "baseline.hpp"
#include "generated-code.hpp"   // code generated by LLM


struct Context {
    std::vector<double> x;
};

void reset(Context *ctx) {
    fillRand(ctx->x, -50.0, 50.0);
    BCAST(ctx->x, DOUBLE);
}

Context *init() {
    Context *ctx = new Context();
    ctx->x.resize(DRIVER_PROBLEM_SIZE);
    reset(ctx);
    return ctx;
}

void NO_OPTIMIZE compute(Context *ctx) {
    oneMinusInverse(ctx->x);
}

void NO_OPTIMIZE best(Context *ctx) {
    correctOneMinusInverse(ctx->x);
}

bool validate(Context *ctx) {

    int rank;
    GET_RANK(rank);

    const size_t numTries = MAX_VALIDATION_ATTEMPTS;
    for (int i = 0; i < numTries; i += 1) {
        std::vector<double> input(1024);
        fillRand(input, -50.0, 50.0);
        BCAST(input, DOUBLE);

        // compute correct result
        std::vector<double> correctResult = input;
        correctOneMinusInverse(correctResult);

        // compute test result
        std::vector<double> testResult = input;
        oneMinusInverse(testResult);
        SYNC();
        
        bool isCorrect = true;
        if (IS_ROOT(rank) && !fequal(correctResult, testResult, 1e-5)) {
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


