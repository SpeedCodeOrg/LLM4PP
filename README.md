# Introduction

This is the repository for `c++` code optimization benchmark, modified from ParEval and PolyBench. It contains problems from both benchmarks. Currently, we only support two modes: serial and OpenMP.

# Setup

## Installation
Create a conda environment. We tested on an environment with `python = 3.11`.
* Run `conda create -n LLM4PP python=3.11 -y`.
* Install a `C++` compiler using the command: `conda install -c conda-forge cxx-compiler`.
* Run `pip install -r requirements.txt`.

## Setup Benchmark
To setup the benchmark, execute the following command:
```
cd ParEval-PolyBench-Code-Opt/drivers/cpp && make
```
This will compile the necessary files for benchmarking code for ParEval and PolyBench.

## Running Evaluation
We provide a few sample evaluation files to evaluate your solutions on this code optimization benchmark.
We use [`hydra`](https://hydra.cc) to control benchmark configurations and assign different strategies for evaluation.
Inside `strategies` folder, there are:
* `evaluation_vllm.py` uses `vllm` to run inference on an LLM to obtain optimized code. This correspondings to `config/strategy/evaluation_vllm.yaml`.
* `evaluation_openai.py` uses the OpenAI API to generate optimized code. This correspondings to `config/strategy/evaluation_openai.yaml`.

Inside `config` folder, `main.yaml` controls strategy configuration yaml files insider `config/strategy` folder, and `base.yaml` controls common configurations like benchmark and logging level. The following are the entries of benchmark and logging:
* `benchmark`: `ParEval` or `PolyBench`.
* `mode`: `serial` or `OpenMP`. During evaluation, we use 1 cpu for `serial` mode, and 8 cpus for `OpenMP` mode.
* `logging.loggers.main_logger.level`: `INFO` or `DEBUG`.

### Evaluation on Closed-Source Models
You must run `export OPENAI_API_KEY=<your api key here>`. Then, to run gpt-4o-mini on ParEval serial mode, do
```bash
python evaluate.py strategy@_global_=evaluation_openai benchmark=ParEval mode=serial logging.loggers.main_logger.level=INFO model=gpt-4o-mini
```

### Evaluation on Open-Source Models
We use `vllm` to inference open-source models. For instance, to run `Qwen/Qwen2.5-Coder-14B-Instruct` on PolyBench OpenMP mode, do
```bash
python evaluate.py strategy@_global_=evaluation_vllm benchmark=ParEval mode=OpenMP logging.loggers.main_logger.level=INFO model=Qwen/Qwen2.5-Coder-14B-Instruct
```

## ParEval Evaluation
ParEval consists of 12 problem categories (fft, graph, geometry etc.) with 5 problems in each category for 60 problems in total.
The code that runs ParEval evaluation is in: `clients/pareval_client.py`. How it runs is as follows.

* The client reads in source code which is currently defined in: `ParEval-PolyBench-Code-Opt/prompts/pareval_code_opt.json` for ParEval, under the field: `src_code`.
  * The source code for each problem is obtained by taking a combination of the prompt located in `ParEval/prompts/raw/` and the baseline code `baseline.hpp` located in `ParEval-PolyBench-Code-Opt/drivers/cpp/benchmarks`.
* The client will then take in the optimized code provided by you, and save it in a format compatible with ParEval's benchmarking platform.
* The client then runs the code in `ParEval/drivers` to obtain relevant information such as if the code compiled, if the code is correct, and the runtime of the code.

Note: some changes to the source code, such as changing the names of structs and function names may cause the code to fail the ParEval benchmark. Refer to the `ParEval/drivers` directory to see how the benchmark is run.

In `client/pareval_client.py`, there are a number of settings that are currently hardcoded to some defaults. There are a few that might be worth changing.

## PolyBench Evaluation

PolyBench contains 30 numerical tasks with static control flows from domains like linear algebra, image processing, physics, and statistics
The code that runs PolyBench evaluation is in: `clients/polybench_client.py`. 
* The baseline codes `baseline.hpp` are located in `ParEval-PolyBench-Code-Opt/drivers/cpp/benchmark/polybench`

### Problem Sizes
The `problem-sizes.json` file determines the size of the input for each problem when benchmarking. Making this larger for some problems that require parallelism is recommended so that the overhead of parallelism does not dominate.

### Run Timeout
Adjusting the run timeout value can be helpful for problems that operate on large input sizes.

### Launch Configs inside ParEval-PolyBench-Code-Opt
Launch configs is most relevant for optimized codes that use OpenMP as in ParEval, the number of OpenMP threads used when benchmarking is set in the launch configs file. Currently, it is set to `8` threads although that can be adjusted.

## Correctness Calculation
Correctness is based on the percentage of programs that are correct, which is tested by seeing if the provided optimized code matches a baseline on a set of random inputs.

## Speedup Calculation
Since we are given the source code as part of the input, if the optimized code does not compile or is incorrect, then the speedup is treated as `1.0` as the worst case is to simply use the source code given to us. Otherwise, if the code is correct, then the resulting speedup is `max(1, baseline_runtime / optimized_runtime)` with the same reasoning as before. The mean of speedup is also provided, where the speedup is treated as is if `< 1.0`.

## Speedup >= 2
We also record the percentage of problems such the optimized code reach a speedup greater or equals to `2.0`, where such optimization is significant.

## Submission
What you will submit is a file similar to the various versions of `evaluation.py` that we have provided. You are given a list of problems, and then asked to produce optimized code for each of the problems. We will be running the code that you submit on our end.

## Additional Information
For more information, there is documentation in the `ParEval` directory here, which is a slightly modified version of the official `ParEval` benchmark located [here](https://github.com/parallelcodefoundry/ParEval) to support code optimization.

If you have any questions, please do not hesitate to contact us.