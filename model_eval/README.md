## Setup

### Installation
Create a conda environment. We tested on an environment with `python = 3.10.15`.
* Install a `C++` compiler using the command: `conda install -c conda-forge cxx-compiler`.
* Run `pip install -r requirements.txt`.

### Setup ParEval
To setup ParEval, execute the following command:
```
cd ParEval/drivers/cpp && make
```

This will compile the necessary files for ParEval's benchmarking code.

## Running Evaluation
We provide a few sample evaluation files to evaluate your solutions on ParEval. 
* `evaluation.py` is a dummy evaluation script: it copies the input code as the optimized code.
* `evaluation_vllm.py` uses `vllm` to run inference on an LLM to obtain optimized code.
* `evaluation_openai.py` uses the OpenAI API to generate optimized code.

### Evaluation on Closed-Source Models
You must run `export OPENAI_API_KEY=<your api key here>`. Then run `python evaluation_openai.py` to run the sample evaluation code that calls the OpenAI API to generate optimized code.

### Evaluation on Open-Source Models
Fill in `MODEL_PATH` in `evaluation_vllm.py` and then run `python evaluation_vllm.py`. This sample evaluation code calls `vllm` on a LLM to generate optimized code.

## ParEval Evaluation
ParEval consists of 12 problem categories (fft, graph, geometry etc.) with 5 problems in each category for 60 problems in total.
The code that runs ParEval evaluation is in: `clients/pareval_client.py`. How it runs is as follows.

* The client reads in source code which is currently defined in: `ParEval/prompts/code_opt.json`, under the field: `src_code`.
  * The source code for each problem is obtained by taking a combination of the prompt located in `ParEval/prompts/raw/` and the baseline code `baseline.hpp` located in `ParEval/drivers/cpp/benchmarks`.
* The client will then take in the optimized code provided by you, and save it in a format compatible with ParEval's benchmarking platform.
* The client then runs the code in `ParEval/drivers` to obtain relevant information such as if the code compiled, if the code is correct, and the runtime of the code.

Note: some changes to the source code, such as changing the names of structs and function names may cause the code to fail the ParEval benchmark. Refer to the `ParEval/drivers` directory to see how the benchmark is run.

In `client/pareval_client.py`, there are a number of settings that are currently hardcoded to some defaults. There are a few that might be worth changing.

### Problem Sizes
The `problem-sizes.json` file determines the size of the input for each problem when benchmarking. Making this larger for some problems that require parallelism is recommended so that the overhead of parallelism does not dominate.

### Run Timeout
Adjusting the run timeout value can be helpful for problems that operate on large input sizes.

### Launch Configs
Launch configs is most relevant for optimized codes that use OpenMP as in ParEval, the number of OpenMP threads used when benchmarking is set in the launch configs file. Currently, it is set to `8` threads although that can be adjusted.

## Correctness Calculation
Correctness is based on the percentage of programs that are correct, which is tested by seeing if the provided optimized code matches a baseline on a set of random inputs.

## Speedup Calculation
Since we are given the source code as part of the input, if the optimized code does not compile or is incorrect, then the speedup is treated as `1.0` as the worst case is to simply use the source code given to us. Otherwise, if the code is correct, then the resulting speedup is `max(1, baseline_runtime / optimized_runtime)` with the same reasoning as before.

## Submission
What you will submit is a file similar to the various versions of `evaluation.py` that we have provided. You are given a list of problems, and then asked to produce optimized code for each of the problems. We will be running the code that you submit on our end.

## Additional Information
For more information, there is documentation in the `ParEval` directory here, which is a slightly modified version of the official `ParEval` benchmark located [here](https://github.com/parallelcodefoundry/ParEval) to support code optimization.

If you have any questions, please do not hesitate to contact us.
