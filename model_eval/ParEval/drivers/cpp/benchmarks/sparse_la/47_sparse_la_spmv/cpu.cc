// Driver for 47_sparse_la_spmv for Serial, OpenMP, MPI, and MPI+OpenMP
// struct COOElement {
//    size_t row, column;
//    double value;
// };
// 
// /* Compute y = alpha*A*x + beta*y where alpha and beta are scalars, x and y are vectors,
//    and A is a sparse matrix stored in COO format.
//    x and y are length N and A is M x N.
//    Example:
// 
//    input: alpha=0.5 beta=1.0 A=[{0,1,3}, {1,0,-1}] x=[-4, 2] y=[-1,1]
//    output: y=[2, 3]
// */
// void spmv(double alpha, std::vector<COOElement> const& A, std::vector<double> const& x, double beta, std::vector<double> &y, size_t M, size_t N) {

#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <vector>

#include "utilities.hpp"
#include "generated-code.hpp"   // code generated by LLM
#include "baseline.hpp"

struct Context {
    std::vector<COOElement> A;
    std::vector<size_t> rows, columns;
    std::vector<double> x, y, values;
    double alpha, beta;
    size_t M, N;
};

void reset(Context *ctx) {
    ctx->alpha = (rand() / (double) RAND_MAX) * 2.0 - 1.0;
    ctx->beta = (rand() / (double) RAND_MAX) * 2.0 - 1.0;

    fillRand(ctx->rows, 0UL, ctx->M);
    fillRand(ctx->columns, 0UL, ctx->N);
    fillRand(ctx->values, -1.0, 1.0);
    fillRand(ctx->x, -1.0, 1.0);
    fillRand(ctx->y, -1.0, 1.0);

    BCAST_PTR(&ctx->alpha, 1, DOUBLE);
    BCAST_PTR(&ctx->beta, 1, DOUBLE);
    BCAST(ctx->rows, UNSIGNED_LONG);
    BCAST(ctx->columns, UNSIGNED_LONG);
    BCAST(ctx->values, DOUBLE);
    BCAST(ctx->x, DOUBLE);
    BCAST(ctx->y, DOUBLE);

    for (size_t i = 0; i < ctx->A.size(); i += 1) {
        ctx->A[i] = {ctx->rows[i], ctx->columns[i], ctx->values[i]};
    }
    std::sort(ctx->A.begin(), ctx->A.end(), [](COOElement const& a, COOElement const& b) {
        return (a.row == b.row) ? (a.column < b.column) : (a.row < b.row);
    });
}

Context *init() {
    Context *ctx = new Context();

    ctx->M = DRIVER_PROBLEM_SIZE;
    ctx->N = DRIVER_PROBLEM_SIZE;
    const size_t nVals = ctx->M * ctx->N * SPARSE_LA_SPARSITY;

    ctx->A.resize(nVals);
    ctx->rows.resize(nVals);
    ctx->columns.resize(nVals);
    ctx->values.resize(nVals);
    ctx->x.resize(ctx->N);
    ctx->y.resize(ctx->M);

    reset(ctx);
    return ctx;
}

void NO_OPTIMIZE compute(Context *ctx) {
    spmv(ctx->alpha, ctx->A, ctx->x, ctx->beta, ctx->y, ctx->M, ctx->N);
}

void NO_OPTIMIZE best(Context *ctx) {
    correctSpmv(ctx->alpha, ctx->A, ctx->x, ctx->beta, ctx->y, ctx->M, ctx->N);
}

bool validate(Context *ctx) {
    const size_t TEST_SIZE = 128;
    const size_t nVals = TEST_SIZE * TEST_SIZE * SPARSE_LA_SPARSITY;

    std::vector<COOElement> A(nVals);
    std::vector<size_t> rows(nVals), columns(nVals);
    std::vector<double> values(nVals), x(TEST_SIZE), correct(TEST_SIZE), test(TEST_SIZE);

    int rank;
    GET_RANK(rank);

    const size_t numTries = MAX_VALIDATION_ATTEMPTS;
    for (int trialIter = 0; trialIter < numTries; trialIter += 1) {
        // set up input
        double alpha = (rand() / (double) RAND_MAX) * 2.0 - 1.0;
        double beta = (rand() / (double) RAND_MAX) * 2.0 - 1.0;
        fillRand(rows, 0UL, TEST_SIZE);
        fillRand(columns, 0UL, TEST_SIZE);
        fillRand(values, -1.0, 1.0);
        fillRand(x, -1.0, 1.0);
        fillRand(correct, -1.0, 1.0);

        BCAST_PTR(&alpha, 1, DOUBLE);
        BCAST_PTR(&beta, 1, DOUBLE);
        BCAST(rows, UNSIGNED_LONG);
        BCAST(columns, UNSIGNED_LONG);
        BCAST(values, DOUBLE);
        BCAST(x, DOUBLE);
        BCAST(correct, DOUBLE);
        test = correct;

        for (size_t i = 0; i < A.size(); i += 1) {
            A[i] = {rows[i], columns[i], values[i]};
        }
        std::sort(A.begin(), A.end(), [](COOElement const& a, COOElement const& b) {
            return (a.row == b.row) ? (a.column < b.column) : (a.row < b.row);
        });

        // compute correct result
        correctSpmv(alpha, A, x, beta, correct, TEST_SIZE, TEST_SIZE);

        // compute test result
        spmv(alpha, A, x, beta, test, TEST_SIZE, TEST_SIZE);
        SYNC();
        
        bool isCorrect = true;
        if (IS_ROOT(rank) && !fequal(correct, test, 1e-4)) {
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