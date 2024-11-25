// Driver for 25_reduce_xor for Kokkos
// #include <Kokkos_Core.hpp>
//
// /* Return the logical XOR reduction of the vector of bools x.
//    Use Kokkos to reduce in parallel. Assume Kokkos is already initialized.
//    Example:
//
//    input: [false, false, false, true]
//    output: true
// */
// bool reduceLogicalXOR(Kokkos::View<const bool*> const& x) {

#include <algorithm>
#include <numeric>
#include <random>
#include <vector>

#include "kokkos-includes.hpp"

#include "utilities.hpp"
#include "baseline.hpp"
#include "generated-code.hpp"   // code generated by LLM

struct Context {
    Kokkos::View<const bool*> x;
    Kokkos::View<bool*> xNonConst;

    std::vector<bool> x_host;
};

void reset(Context *ctx) {
    for (int i = 0; i < ctx->x_host.size(); i += 1) {
        ctx->x_host[i] = rand() % 2;
        ctx->xNonConst(i) = ctx->x_host[i];
    }

    ctx->x = ctx->xNonConst;
}

Context *init() {
    Context *ctx = new Context();

    ctx->x_host.resize(DRIVER_PROBLEM_SIZE);

    ctx->xNonConst = Kokkos::View<bool*>("x", DRIVER_PROBLEM_SIZE);

    reset(ctx);
    return ctx;
}

void NO_OPTIMIZE compute(Context *ctx) {
    bool out = reduceLogicalXOR(ctx->x);
    (void)out;
}

void NO_OPTIMIZE best(Context *ctx) {
    bool out = correctReduceLogicalXOR(ctx->x_host);
    (void)out;
}

bool validate(Context *ctx) {
    const size_t TEST_SIZE = 1024;

    std::vector<bool> x_host(TEST_SIZE);
    bool correct, test;

    Kokkos::View<bool*> xNonConst("x", TEST_SIZE);
    Kokkos::View<const bool*> x;

    const size_t numTries = MAX_VALIDATION_ATTEMPTS;
    for (int trialIter = 0; trialIter < numTries; trialIter += 1) {
        // set up input
        for (int i = 0; i < x_host.size(); i += 1) {
            x_host[i] = rand() % 2;
            xNonConst(i) = x_host[i];
        }
        x = xNonConst;

        // compute correct result
        correct = correctReduceLogicalXOR(x_host);

        // compute test result
        test = reduceLogicalXOR(x);

        if (correct != test) {
            return false;
        }
    }

    return true;
}

void destroy(Context *ctx) {
    delete ctx;
}
