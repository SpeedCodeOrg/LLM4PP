/* Main driver for generated openmp C++ code. This relies on five externally available
* functions:
*
*   - init() -- returns a pointer to a context object
*   - compute(Context *ctx) -- runs the benchmark
*   - best(Context *ctx) -- runs the best sequential code
*   - validate(Context *ctx) -- returns true if the benchmark is valid
*   - reset(Context *ctx) -- resets the benchmark
*   - destroy(Context *ctx) -- frees the context object
*
* These functions are defined in the driver for the given benchmark and handle
* the data and calling the generated code.
*/
#define ANKERL_NANOBENCH_IMPLEMENT
#include <cstdio>
#include <string>
#include <cfloat>

#include <omp.h>
#include <chrono>
#include <random>
#include <iostream>
#include <nanobench.h>

class Context;
extern "C++" {
    /* todo -- these could all be in a class, but I'm not sure if virtual 
       overloading would incur a noticable overhead here. */
    Context *init();
    void compute(Context *ctx);
    void best(Context *ctx);
    bool validate(Context *ctx, std::mt19937& engine);
    void reset(Context *ctx, std::mt19937& engine);
    void destroy(Context *ctx);

    // newly added
    Context* copy(Context* ctx);
}

int main(int argc, char **argv) {

    /* initialize settings from arguments */
    if (argc > 2) {
        printf("Usage: %s <?num_threads>\n", argv[0]);
        exit(1);
    }

    const int NITER = 10;
    int num_threads = 1;
    if (argc > 1) {
        num_threads = std::stoi(std::string(argv[1]));
    }
    omp_set_num_threads(num_threads);

    std::mt19937 engine;

    /* initialize */
    Context *ctx = init();

    for (int i = 0; i < NITER; i += 1) {
	engine.seed(i);

        const bool isValid = validate(ctx, engine);
        if (!isValid) {
            printf("Validation: FAIL\n");
            destroy(ctx);
            return 0;
        }
    }

    printf("Validation: PASS\n");

    /* validate */
    /*
    const bool isValid = validate(ctx);
    printf("Validation: %s\n", isValid ? "PASS" : "FAIL");
    if (!isValid) {
        destroy(ctx);
        return 0;
    }
    */

    const int NUM_WARMUP_ITER = 3;

    /*

    for (int i = 0; i < NUM_WARMUP_ITER; i++) {
	engine.seed(i);
        reset(ctx, engine);
        compute(ctx);
    }


    // benchmark 
    double totalTime = 0.0;
    for (int i = 0; i < NITER; i += 1) {
	engine.seed(i);
        reset(ctx, engine);

	
        double start = omp_get_wtime();
        compute(ctx);
        totalTime += omp_get_wtime() - start;
    }

    // printf("Time: %.*f\n", DBL_DIG-1, totalTime / NITER);
    printf("Time: %.17g\n", totalTime / NITER);

    // benchmark best
    for (int i = 0; i < NUM_WARMUP_ITER; i++) {
	engine.seed(i);
        reset(ctx, engine);
        best(ctx);
    }

    totalTime = 0.0;
    for (int i = 0; i < NITER; i += 1) {
	engine.seed(i);
        reset(ctx, engine);

        double start = omp_get_wtime();
        best(ctx);
        totalTime += omp_get_wtime() - start;
    }
    printf("BestSequential: %.*f\n", DBL_DIG-1, totalTime / NITER);
    */

    ankerl::nanobench::Bench bench_llm;
    std::vector<double> runtimes_llm;
    runtimes_llm.reserve(1024);
    for (int i = 0; i < NITER; i += 1) {
	engine.seed(i);
        reset(ctx, engine);

	bench_llm.run("llm code", [&]() {
	    Context* new_ctx = copy(ctx);

	    auto begin = std::chrono::high_resolution_clock::now();
	    compute(new_ctx);
	    auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
	    runtimes_llm.push_back(duration * 1.0 / 1e6);

	    destroy(new_ctx);
	});
    }

    ankerl::nanobench::Bench bench_baseline;
    std::vector<double> runtimes_baseline;
    runtimes_baseline.reserve(1024);
    for (int i = 0; i < NITER; i += 1) {
	engine.seed(i);
        reset(ctx, engine);

	// ankerl::nanobench::Bench().run("baseline code", [&]() {
	bench_baseline.run("baseline code", [&]() {
	    Context* new_ctx = copy(ctx);

	    auto begin = std::chrono::high_resolution_clock::now();
	    best(new_ctx);
	    auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
	    runtimes_baseline.push_back(duration * 1.0 / 1e6);

	    destroy(new_ctx);
	});
    }

    std::sort(runtimes_llm.begin(), runtimes_llm.end());
    std::sort(runtimes_baseline.begin(), runtimes_baseline.end());

    auto time_llm = runtimes_llm[runtimes_llm.size() / 2];
    auto time_baseline = runtimes_baseline[runtimes_baseline.size() / 2];

    printf("Time: %.17g\n", time_llm);
    printf("BestSequential: %.*f\n", DBL_DIG-1, time_baseline);

    /* cleanup */
    destroy(ctx);

    return 0;
}
