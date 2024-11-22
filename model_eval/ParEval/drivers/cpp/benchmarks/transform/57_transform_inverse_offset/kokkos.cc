// Driver for 54_transform_inverse_offset
// /* Replace every element of the vector x with 1-1/x.
//    Use Kokkos to compute in parallel. Assume Kokkos has already been initialized.
//    Example:
// 
//    input: [2, 4, 1, 12, -2]
//    output: [0.5, 0.75, 0, 0.91666666, 1.5]
// */
// void oneMinusInverse(Kokkos::View<double*> &x) {

#include <algorithm>
#include <numeric>
#include <random>
#include <vector>

#include <Kokkos_Core.hpp>
#include <Kokkos_Sort.hpp>

#include "utilities.hpp"
#include "baseline.hpp"
#include "generated-code.hpp"   // code generated by LLM


struct Context {
    Kokkos::View<double*> x;
    std::vector<double> vecX;
};

void reset(Context *ctx) {
    fillRand(ctx->x, -50.0, 50.0);
    fillRand(ctx->vecX, -50.0, 50.0);
}

Context *init() {
    Context *ctx = new Context();

    ctx->x = Kokkos::View<double*>("x", DRIVER_PROBLEM_SIZE);
    ctx->vecX.resize(DRIVER_PROBLEM_SIZE);

    reset(ctx);
    return ctx;
}

void NO_OPTIMIZE compute(Context *ctx) {
    oneMinusInverse(ctx->x);
}

void NO_OPTIMIZE best(Context *ctx) {
    correctOneMinusInverse(ctx->vecX);
}

bool validate(Context *ctx) {

    const size_t numTries = MAX_VALIDATION_ATTEMPTS;
    for (int i = 0; i < numTries; i += 1) {
        std::vector<double> input(1024);
        fillRand(input, -50.0, 50.0);

        // compute correct result
        std::vector<double> correctResult = input;
        correctOneMinusInverse(correctResult);

        // compute test result
        Kokkos::View<double*> testResult("testResult", input.size());
        copyVectorToView(input, testResult);
        oneMinusInverse(testResult);

        std::vector<double> testResultVec(input.size());
        copyViewToVector(testResult, testResultVec);
        
        if (!fequal(correctResult, testResultVec, 1e-5)) {
            return false;
        }
    }

    return true;
}

void destroy(Context *ctx) {
    delete ctx;
}


