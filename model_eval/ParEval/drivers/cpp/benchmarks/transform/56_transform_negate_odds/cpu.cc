// Driver for 53_transform_negate_odds
// /* In the vector x negate the odd values and divide the even values by 2.
//    Example:
//    
//    input: [16, 11, 12, 14, 1, 0, 5]
//    output: [8, -11, 6, 7, -1, 0, -5]
// */
// void negateOddsAndHalveEvens(std::vector<int> &x) {

#include <algorithm>
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
    fillRand(ctx->x, 1, 100);
    BCAST(ctx->x, INT);
}

Context *init() {
    Context *ctx = new Context();
    ctx->x.resize(DRIVER_PROBLEM_SIZE);
    reset(ctx);
    return ctx;
}

void NO_OPTIMIZE compute(Context *ctx) {
    negateOddsAndHalveEvens(ctx->x);
}

void NO_OPTIMIZE best(Context *ctx) {
    correctNegateOddsAndHalveEvens(ctx->x);
}

bool validate(Context *ctx) {

    int rank;
    GET_RANK(rank);

    const size_t numTries = MAX_VALIDATION_ATTEMPTS;
    for (int i = 0; i < numTries; i += 1) {
        std::vector<int> input(1024);
        fillRand(input, 1, 100);
        BCAST(input, INT);

        // compute correct result
        std::vector<int> correctResult = input;
        correctNegateOddsAndHalveEvens(correctResult);

        // compute test result
        std::vector<int> testResult = input;
        negateOddsAndHalveEvens(testResult);
        SYNC();
        
        bool isCorrect = true;
        if (IS_ROOT(rank) && !std::equal(correctResult.begin(), correctResult.end(), testResult.begin())) {
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


