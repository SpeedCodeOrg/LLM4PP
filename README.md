# Introduction

This repo contains a `c++` code optimization benchmark for LLMs, modified from the ParEval and PolyBench benchmarks. It uses problems from both benchmarks. Currently, we only support two modes: serial and OpenMP on the CPU.

# Installation

## Setup Environment
Create a conda environment. We tested on an environment with `python = 3.11`.
* Run `conda create -n code-opt-bench python=3.11 -y`.
* Then `conda activate code-opt-bench`.
* Install a `C++` compiler using the command: `conda install -c conda-forge cxx-compiler`. The C++ compiler must support C++20.
* Run `pip install -r requirements.txt`.

## Setup Benchmark
To setup the benchmark, execute the following command:

```
cd ParEval-PolyBench-Code-Opt/drivers/cpp && make
```
This will compile the necessary files for benchmarking code for ParEval and PolyBench.

# Run Evaluation

For example, to run gpt-4o-mini on ParEval serial mode, do

```bash
python evaluate.py strategy@_global_=evaluation_openai benchmark=ParEval mode=serial logging.loggers.main_logger.level=INFO model=gpt-4o-mini
```

`evaluate.py` is the main file to run. In the above example, `evaluate.py` calls the `run` function in `strategies/evaluation_openai.py` to evaluate on OpenAI models. We use [`hydra`](https://hydra.cc) to control different configurations.


## Choose a Model
We provide scripts to evaluate on different models. To evaluate on OpenAI models, use the following configuration, and replace the command presented in the section of [Run Evaluation](#run-evaluation):

```
strategy@_global_=evaluation_openai model=gpt-4o-mini
```

Similarly, to evaluate on Open Source models, use the following configuration, and replace the command presented in the section of [Run Evaluation](#run-evaluation):

```
strategy@_global_=evaluation_vllm model=Qwen/Qwen2.5-Coder-14B-Instruct
```

## Choose a Benchmark
Two different benchmarks are available. To evaluate on ParEval benchmark,use the following configuration, and replace the command presented in the section of [Run Evaluation](#run-evaluation):

```
benchmark=ParEval
```

Similarly, for PolyBench benchmark:

```
benchmark=PolyBench
```

## Choose a Mode
Two different modes are available. To evaluate on serial benchmark,use the following configuration, and replace the command presented in the section of [Run Evaluation](#run-evaluation):

```
mode=serial
```

Similarly, for OpenMP mode:

```
mode=OpenMP
```

# Note

Each problem has different input sizes. Check `ParEval-PolyBench-Code-Opt/drivers/problem-sizes.json` for detailed input sizes.

The settings of timeout and OpenMP threads are located in `client/pareval_client.py`(ParEval) and `client/polybench_client.py`(PolyBench). We fix timeout to 300 seconds and 8 threads for OpenMP.


# Evaluation Metrics

The results of different evaluation metrics are directly printed in the `stdout`. The optimized codes are saved in the `evaluator_results` folder.

The metrics are the following:
* ** compile **: percentage of compiled optimized programs.
* ** correctness **: percentage of correct optimized programs.
* ** geomean speedup **: geometric mean of speedup.
* ** mean speedup **: mean of speedup.
* ** speedup>=2 **: percentage of optimized programs than has speedup greater or equal to 2.

# Custom Evaluation Implementation

Feel free to implement your own evaluation strategy or pipeline on our code optimization benchmark.

We include two examples of evaluating on the benchmark: `strategies/evaluation_openai.py` and `strategies/evaluation_vllm.py`, both of them contains the `run` function. 
Notice that the user should still run `evaluate.py` as the main driver, where `evaluate.py` will use `hydra` to call the `run` function respectively. Also, check `config/strategy` for configuration yaml files.

To add a custom strategy, your custom configuration name under `config/strategy` has to be aligned with `.py` file under `strategy` folder. For instance, `config/strategy/evaluation_vllm.yaml` and `strategies/evaluation_vllm.py`. Each custom yaml file under `config/strategy` has to include `_target_: strategies.<your-strategy>.run`. For instance, inside `config/strategy/evaluation_vllm.yaml`, the `_target_` configuration `_target_: strategies.evaluation_vllm.run` is included.


# Additional Information

This benchmark is adapted from `ParEval` and `PolyBench` for code optimization and speedup evaluation. The original `ParEval` is located at https://github.com/parallelcodefoundry/ParEval and the original `PolyBench` is located at https://github.com/MatthiasJReisinger/PolyBenchC-4.2.1/tree/master.

If you have any questions, please do not hesitate to contact us.
