// Driver for 0_sort_non-zero
// /* Sort the vector x in ascending order ignoring elements with value 0.
//    Leave zero valued elements in-place.
//    Example:
// 
// 	  input: [8, 4, 0, 9, 8, 0, 1, -1, 7]
//    output: [-1, 1, 0, 4, 7, 0, 8, 8, 9]
// */
// void sortIgnoreZero(std::vector<int> &x) {

#include <algorithm>
#include <numeric>
#include <random>
#include <vector>

#include "kokkos-includes.hpp"

#include "utilities.hpp"
#include "baseline.hpp"
#include "generated-code.hpp"   // code generated by LLM


struct Context {
    Kokkos::View<int*> x;
    std::vector<int> cpuX;
};

void fillRandWithZeroes(std::vector<int> &x) {
    // fill x with random values, but set some to zero
    for (int i = 0; i < x.size(); i += 1) {
        x[i] = rand();
        if (rand() % 5) {
            x[i] = 0;
        }
    }
}

void fillRandWithZeroesKokkos(Kokkos::View<int*> &x) {
    // fill x with random values, but set some to zero
    for (int i = 0; i < x.size(); i += 1) {
        x(i) = rand();
        if (rand() % 5) {
            x(i) = 0;
        }
    }
}

void reset(Context *ctx) {
    fillRandWithZeroesKokkos(ctx->x);
    fillRandWithZeroes(ctx->cpuX);
}

Context *init() {
    Context *ctx = new Context();
    ctx->x = Kokkos::View<int*>("x", DRIVER_PROBLEM_SIZE);
    ctx->cpuX.resize(DRIVER_PROBLEM_SIZE);
    reset(ctx);
    return ctx;
}

void NO_OPTIMIZE compute(Context *ctx) {
    sortIgnoreZero(ctx->x);
}

void NO_OPTIMIZE best(Context *ctx) {
    correctSortIgnoreZero(ctx->cpuX);
}

bool validate(Context *ctx) {

    const size_t numTries = MAX_VALIDATION_ATTEMPTS;
    for (int i = 0; i < numTries; i += 1) {
        std::vector<int> input(1024);
        fillRandWithZeroes(input);

        // compute correct result
        std::vector<int> correctResult = input;
        correctSortIgnoreZero(correctResult);

        // compute test result
        Kokkos::View<int*> testResult("testResult", input.size());
        copyVectorToView(input, testResult);
        sortIgnoreZero(testResult);

        std::vector<int> testResultCpu(input.size());
        copyViewToVector(testResult, testResultCpu);
        
        if (!std::equal(correctResult.begin(), correctResult.end(), testResultCpu.begin())) {
            return false;
        }
    }

    return true;
}

void destroy(Context *ctx) {
    delete ctx;
}


