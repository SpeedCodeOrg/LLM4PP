// Driver for 06_fft_dft for Serial, OpenMP, MPI, and MPI+OpenMP
// /* Compute the discrete fourier transform of x. Store the result in output.
//    Example:
// 
//    input: [1, 4, 9, 16]
//    output: [30+0i, -8-12i, -10-0i, -8+12i]
// */
// void dft(std::vector<double> const& x, std::vector<std::complex<double>> &output) {

#include <algorithm>
#include <cmath>
#include <complex>
#include <numeric>
#include <random>
#include <vector>

#include "utilities.hpp"
#include "baseline.hpp"
#include "generated-code.hpp"   // code generated by LLM

struct Context {
    std::vector<double> x;
    std::vector<std::complex<double>> output;
};

void reset(Context *ctx) {
    fillRand(ctx->x, -1.0, 1.0);
    BCAST(ctx->x, DOUBLE);
}

Context *init() {
    Context *ctx = new Context();

    ctx->x.resize(DRIVER_PROBLEM_SIZE);
    ctx->output.resize(DRIVER_PROBLEM_SIZE);

    reset(ctx);
    return ctx;
}

void NO_OPTIMIZE compute(Context *ctx) {
    dft(ctx->x, ctx->output);
}

void NO_OPTIMIZE best(Context *ctx) {
    correctDft(ctx->x, ctx->output);
}

bool validate(Context *ctx) {
    const size_t TEST_SIZE = 1024;

    std::vector<double> x(TEST_SIZE);
    std::vector<std::complex<double>> correct(TEST_SIZE), test(TEST_SIZE);

    int rank;
    GET_RANK(rank);

    const size_t numTries = MAX_VALIDATION_ATTEMPTS;
    for (int i = 0; i < numTries; i += 1) {
        // set up input
        fillRand(x, -1.0, 1.0);
        BCAST(x, DOUBLE);

        // compute correct result
        correctDft(x, correct);

        // compute test result
        dft(x, test);
        SYNC();
        
        bool isCorrect = true;
        if (IS_ROOT(rank)) {
            for (int j = 0; j < x.size(); j += 1) {
                if (std::abs(correct[j].real() - test[j].real()) > 1e-4 || std::abs(correct[j].imag() - test[j].imag()) > 1e-4) {
                    isCorrect = false;
                    break;
                }
            }
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