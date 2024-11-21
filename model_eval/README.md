## Setup

### Installation
Create a conda environment. We tested on an environment with `python = 3.10.15`.
* Install a `C++` compiler using the command: `conda install -c conda-forge cxx-compiler`.
* Run `pip install -r requirements.txt`.

### Setup ParEval
To setup ParEval, execute the following command:
```
cd ParEval/drivers && make
```

This will compile the necessary files for ParEval's benchmarking code.

## Run ParEval evaluation
In the `evaluation.py` file, you will see a `get_pareval_driver()` command which returns a `ParEvalDriver` object which takes in code for a ParEval problem and executes it using the ParEval benchmarking code and returns the result. 
Right now, `evaluation.py` copies the input code as the optimized code, and therefore should get a correctness of `1.0` and a speedup close to `1.0`.

This code that does this is in `clients/pareval_client.py`. The source code to be optimized is defined in: `ParEval/prompts/code_opt.json`, which is a combination of the prompts located in `ParEval/prompts/raw/{PROBLEM_CATEGORY}/{PROBLEM_NAME}/serial` and the baseline code located in `ParEval/drivers/cpp/benchmarks/{PROBLEM_CATEGORY}/{PROBLEM_NAME}/baseline.hpp`.

Here, `PROBLEM_CATEGORY` refers to one of the twelve problem categories in the `ParEval` benchmark and `PROBLEM_NAME` refers to a specific programming problem. An example problem category is `graph` and an example problem name is `16_graph_largest_component`.

Note: some changes to the source code, such as changing the names of structs and function names may cause the code to fail the ParEval benchmark. Refer to the `ParEval/drivers` directory to see how the benchmark is run.

In `client/pareval_client.py`, there are a number of settings that are currently hardcoded to some defaults. There are a few that might be worth changing.

### Problem Sizes
The `problem-sizes.json` file determines the size of the input for each problem when benchmarking. Making this larger for some problems that require parallelism is recommended to reduce 

### Run Timeout
Adjusting the run timeout value can be helpful for problems that operate on large input sizes.

### Launch Configs
Launch configs is most relevant for optimized codes that use OpenMP as in ParEval, the number of OpenMP threads used when benchmarking is set in the launch configs file. Currently, it is set to `8` threads although that can be adjusted.

## Correctness Calculation
Correctness is based on the percentage of programs that are correct, which is tested by seeing if the provided optimized code matches a baseline on a set of random inputs.

## Speedup Calculation
Since we are given the source code as part of the input, if the optimized code does not compile or is incorrect, then the speedup is treated as `1.0` as the worst case is to simply use the source code given to us. Otherwise, if the code is correct, then the resulting speedup is `max(1, baseline_runtime / optimized_runtime)` with the same reasoning as before.

## Submission
What you will submit is a file similar to `evaluation.py` which will produce optimized code for the ParEval problems.

## Additional Information
For more information, there is documentation in the `ParEval` directory, which is a slightly modified version of the official `ParEval` benchmark to support code optimization. 
